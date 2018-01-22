#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec3 outColor;
in vec3 outUV;
in vec3 origVertex;// L

in vec3 varyingNormal;
in vec3 varyingLightDirection;
in vec4 varyingLightVertexPosition;//position respect the camera view

out vec4 outputF;

uniform int mode;
uniform sampler2D tex0;
uniform sampler2DArray tex_3D;
//uniform sampler3D tex_3D;


uniform sampler2D shadowMap;
uniform int shadowState;
 
uniform mat4 light_mvpMatrix;
uniform mat4 light_biasMatrix;	//transfrom to [0,1] tex coordinates

const float ambientColor=0.1;
const float diffuseColor=1.0;
const float specularColor=1.0;

vec2 poissonDisk4[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

vec2 poissonDisk16[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCoef(){
	vec4 shadow_coord2=light_mvpMatrix*vec4(origVertex,1.0);
	vec3 ProjCoords = shadow_coord2.xyz / shadow_coord2.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
	
	/// D
	const bool HDShadow=false;
	float visibility=1.0f;
	if(HDShadow==true){
		// HDShadow
		for (int i=0;i<8;i++){
			int index = int(16.0*random(origVertex.xyz, i))%16;
			if ( texture2D( shadowMap, UVCoords + poissonDisk16[index]/3500.0 ).z  <  z ){
				visibility-=0.1;
			}
		}
	}else{
		// Low Shadow
		for (int i=0;i<4;i++){
			int index = int(4.0*random(origVertex.xyz, i))%4;
			if ( texture2D( shadowMap, UVCoords + poissonDisk4[index]/3500.0 ).z  <  z ){
				visibility-=0.2;
			}
		}

	}
	return visibility;
}

void main(){

	outputF =vec4(outColor,1.0);

	// SHADOW: From light
	if(shadowState==2){// no tex/color/shadows
		return;
	}

	// TEXTURE
	if((mode&0xFF)==2||(mode&0xFF)==4||(mode&0xFF)==6){// tex / water / model texture
		outputF = texture( tex0, outUV.rg );
	}
	
	//////////////
	// TERRAIN
	if((mode&0xFF)==3){

		/*vec4 terrainColor=vec4(0,0,0,1.0);
		float factor;
		float height=(origVertex.z)/255.0;//0-1
		height=clamp(height,0,1);
	
		if(height<=0.33){
			//terrainColor=vec4(1.0,1.0,1.0,1.0);
			factor=height/0.33;//0-1
			terrainColor+=(1-factor)*texture( tex_3D, vec3(outUV.rg,0.0));
			terrainColor+=factor*texture( tex_3D, vec3(outUV.rg+vec2(0.1,0.1),1.0));
		}else{
			if(height<=0.66){
				factor=(height-0.33)/0.33f;//0-1
				terrainColor+=(1-factor)*texture( tex_3D, vec3(outUV.rg+vec2(0.1,0.1),1.0));
				terrainColor+=factor*texture( tex_3D, vec3(outUV.rg+vec2(0.45,0.45),2.0));
			}else{
				//terrainColor=vec4(0,0,1.0,1.0);
				factor=(height-0.66)/0.33f;//0-1
				//terrainColor+=(1-factor)*texture( tex_3D, vec3(outUV.rg,2.0));
				//terrainColor+=factor*texture( tex_3D, vec3(outUV.rg,3.0));
				terrainColor+=(1-factor)*texture( tex_3D, vec3(outUV.rg+vec2(0.45,0.45),2.0));
				terrainColor+=factor*texture( tex_3D, vec3(outUV.rg+vec2(0.3,0.3),3.0));

			}
		}*/
		float height=100.0f*(origVertex.z)/255.0;//0-100
		height=clamp(height,0.0,99.999999);//0-0.99
		int texInd=int(height/25);
		float interpTex=mod(height,25.0);
		outputF=mix(
			texture( tex_3D, vec3(outUV.rg,texInd) ),
			texture( tex_3D, vec3(outUV.rg,texInd+1) ),
			interpTex/25.0);
	}

	// LIGHTING
	vec4 ambientIllumination=vec4(0.05,0.05,0.05,0.0);
	vec4 diffuseIllumination=vec4(0.95,0.95,0.95,1.0);
	if((mode&0x0200)==0x0200){
		vec3 normal = normalize(varyingNormal);
		//vec3 lightDirection = normalize(vec3(0.5,0.5,0.5));//normalize(varyingLightDirection);
		vec3 lightDirection = normalize(varyingLightDirection);
		ambientIllumination = ambientColor*vec4(1.0,1.0,1.0,1.0);
		diffuseIllumination = (diffuseColor*vec4(1.0,1.0,1.0,1.0)) * max(0.0, dot(lightDirection, normal));
		//outputF=(ambientIllumination+diffuseIllumination)*outputF;
	}

	// SHADOW Disable
	if(shadowState==0){// 0 SHADOW Disable
		outputF=(ambientIllumination+diffuseIllumination)*outputF;
		return;
	}

	// SHADOW Render normal with shadows
	if(shadowState==1){// 1 SHADOW Render Normal
		float shadow_coef=0.95;
		shadow_coef= shadowCoef();
		outputF=(ambientIllumination+(shadow_coef+0.05)*diffuseIllumination)*outputF;
		return;
	}

}//

