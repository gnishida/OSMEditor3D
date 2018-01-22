#version 420

in vec3 vertex;
in vec3 color;
in vec3 normal;
in vec3 uv;

// instant
in mat4 inst_transfModel;
in vec3 inst_color;


out vec3 outColor;
out vec3 outUV;
out vec3 origVertex;// L

out vec3 varyingNormal;
out vec3 varyingLightDirection;
out vec4 varyingLightVertexPosition;//position respect the camera view

uniform mat4 mvpMatrix;//modelViewProjectionMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 light_mvpMatrix;
//uniform mat4 light_mvMatrix;	//modelviewmatrix for the light
uniform mat4 light_biasMatrix;	//transfrom to [0,1] tex coordinates
uniform vec3 lightPosition;  //model-space

uniform vec3 justOneColor;
uniform mat4 modelTransf;

uniform int mode;
// MODE 0--> ligh mode
// MODE 1--> att color no lighting
// MODE 2--> att color lighting
// MODE 3--> tex no lighting
// MODE 4--> tex lighting
// MODE 5--> 
// MODE 6--> model: just one color lighting/transform
// MODE 7--> model: light mode
// MODE 8--> water
// MODE 9--> inst mode
// MODE 10--> inst mode: light mode
// MODE 11--> model texture
// MODE 12--> model texture: light mode

void main(){

	/////////////////////////////////////
	// LIGHT MODE

	if(mode==0){//light mode
		outColor=color;//
		gl_Position = light_mvpMatrix * vec4(vertex,1.0);
		return;
	}

	vec3 currVert=vertex;

	if(mode==7||mode==12){// MODEL
		outColor=justOneColor;
		if(mode==7)
			currVert=currVert.xzy;//change model Y<->Z
		currVert=(modelTransf*vec4(currVert,1.0)).xyz;//note 1.0
		gl_Position = light_mvpMatrix * vec4(currVert,1.0);
		return;
	}

	if(mode==10){// INST
		outColor=inst_color;
		currVert=currVert.xzy;//change model Y<->Z
		currVert=(inst_transfModel*vec4(currVert,1.0)).xyz;//note 1.0
		gl_Position = light_mvpMatrix * vec4(currVert,1.0);
		return;
	}

	/////////////////////////////////////
	// NORMAL MODE

	
	varyingNormal=normal;


	if(mode==6||mode==9||mode==11){
		if(mode==6||mode==11){
			outColor=justOneColor;
			if(mode==6)
				currVert=currVert.xzy;//change model Y<->Z

			currVert=(modelTransf*vec4(currVert,1.0)).xyz;//note 1.0
			varyingNormal=normal.xzy;
			varyingNormal=(modelTransf*vec4(varyingNormal,0.0)).xyz;//note 0.0
			//if(mode==11)
			//	varyingNormal*=-1.0;
		}else{
			outColor=inst_color;
			currVert=currVert.xzy;//change model Y<->Z

			currVert=(inst_transfModel*vec4(currVert,1.0)).xyz;//note 1.0
			varyingNormal=normal.xzy;
			varyingNormal=(inst_transfModel*vec4(varyingNormal,0.0)).xyz;//note 0.0
		}
	}else{	
		outColor=color;
	}

	outUV=uv;
	origVertex=currVert;// L

	gl_Position = mvpMatrix * vec4(currVert,1.0);
	
	
	varyingLightVertexPosition=light_biasMatrix*light_mvpMatrix*vec4(currVert,1.0);
	//vec4 eyeVertex = mvMatrix * vec4(currVert,1.0);
	//eyeVertex /= eyeVertex.w;
	//varyingNormal = normalMatrix * normal;
	/*varyingLightDirection=vec3(mvMatrix*vec4(lightPosition,1.0));
	varyingLightDirection = varyingLightDirection - eyeVertex.xyz;*/
	//vec4 lightVertex= mvMatrix * vec4(lightPosition,1.0);
	//lightVertex /= lightVertex.w;
	//vec4 lightVertex=-vec4(lightPosition,1.0);
	//varyingLightDirection = lightVertex.xyz - eyeVertex.xyz;
	//varyingLightDirection = normalMatrix * normalize(lightPosition);//directional
	
	varyingLightDirection=lightPosition;
	//varyingLightDirection = lightPosition - eyeVertex.xyz;
	//varyingViewerDirection = -eyeVertex.xyz;
	//varyingTextureCoordinate = textureCoordinate;
	

	/*lightVertexPosition=lightModelViewProjectionMatrix*vec4(position,1.0);
	modelPosition=vec3(modelViewMatrix*vec4(position,1.0));
	outNormal=normalMatrix*normal;
	lightPosition=vec3(lightMatrix*vec4(inLightPosition,1.0));
	*/
	/*
	gl_Position = projMatrix*viewMatrix* vec4(position,1.0);

	if(mode==1||mode==3){
		// Light
		vec3 lightDir=normalize(-vec3(50.0f, 100.0, 100.0));
		vec3 lightDif=vec3(1.0, 1.0, 1.0);
		vec3 lightAmb=vec3(0.75, 0.75, 0.75);

		//vec4 normV=normalize(normalMatrix*vec4(aa,1.0));
		//vec4 normV=normalize(vec4(aa,1.0));
		//vec4 normV=vec4(aa,1.0);

		float NdotL=max(dot(aa,lightDir),0.0);
		lightF=NdotL*lightDif+lightAmb;
	}*/


}