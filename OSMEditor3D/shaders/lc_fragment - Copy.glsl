#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec3 outColor;
in vec3 outUV;
in vec3 origVertex;// L
//in vec3 lightF;

//in vec4 lightVertexPosition;
//in vec3 modelPosition;

in vec3 varyingNormal;
in vec3 varyingLightDirection;
in vec4 varyingLightVertexPosition;//position respect the camera view
//in vec3 varyingViewerDirection; 

out vec4 outputF;

uniform sampler2D tex0;

uniform sampler2D shadowMap;
uniform int shadowEnable;
//uniform sampler2DShadow shadowMap;
 
uniform int mode;

uniform mat4 light_mvpMatrix;
uniform mat4 light_biasMatrix;	//transfrom to [0,1] tex coordinates

const float ambientColor=0.1;
const float diffuseColor=1.0;
const float specularColor=1.0;


/*vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);*/

vec2 poissonDisk[16] = vec2[]( 
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
	float visibility=1.0f;
	for (int i=0;i<8;i++){

		int index = int(16.0*random(origVertex.xyz, i))%16;
		if ( texture2D( shadowMap, UVCoords + poissonDisk[index]/3500.0 ).z  <  z ){
			visibility-=0.1;
		}
	}
	return visibility;
}

void main(){
	//outputF=vec4(1.0,0,0,1.0);

	//if(mode==1||mode==2)
	outputF = vec4(outColor,1.0);

	if(mode==8){//water
		outputF = vec4(65.0/255.0,105.0/255.0,1.0,1.0);
		outputF = vec4(varyingNormal,1.0);//return;
	}

	if(mode==3||mode==4||mode==11)
		outputF = texture( tex0, outUV.rg );

	if(mode==0||mode==1||mode==3||mode==7||mode==12){// no lights
		return;
	}

	vec3 normal = normalize(varyingNormal);
	vec3 lightDirection = normalize(varyingLightDirection);
	vec4 ambientIllumination = ambientColor*vec4(1.0,1.0,1.0,1.0);
	vec4 diffuseIllumination = (diffuseColor*vec4(1.0,1.0,1.0,1.0)) * max(0.0, dot(lightDirection, normal));

	float shadow_coef=0.95;

	if(shadowEnable==1)
		shadow_coef= shadowCoef();

	if(mode==11){
		outputF=(shadow_coef+0.05)*outputF;
		return;
	}

	outputF=(ambientIllumination+(shadow_coef+0.05)*diffuseIllumination)*outputF;

}//

