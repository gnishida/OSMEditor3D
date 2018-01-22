#version 420

layout(location = 0)in vec3 vertex;
layout(location = 1)in vec4 color;
layout(location = 2)in vec3 normal;
layout(location = 3)in vec3 uv;


out vec4 outColor;
out vec3 outUV;
out vec3 origVertex;// L

out vec3 varyingNormal;

// UNIFORM
uniform int mode;
uniform int terrainMode;//0 FLAT 1 Mountains

// MODE 1--> color
// MODE 2--> texture
// MODE 3--> terrain
// MODE 4--> water
// MODE 5--> model obj: one color
// MODE 6--> model obj: texture

         // // MODE 9--> hatch
		// // MODE 10--> hatch

//0x0100 --> adapt terrain
//0x0200 --> lighting

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

uniform mat4 light_mvpMatrix;
uniform mat4 light_biasMatrix;	//transfrom to [0,1] tex coordinates
//uniform vec3 lightPosition;  //model-space

uniform int shadowState;
// 0 DISABLE
// 1 SHADOW Render Normal
// 2 SHADOW Render from light

// terrain
uniform vec4 terrain_size;
uniform sampler2D terrain_tex;

// model
uniform vec3 justOneColor;
uniform mat4 modelTransf;

void main(){
	
	outColor=color;
	outUV=uv;
	origVertex=vertex;
	//////////////////////////////////////
	// 1. TRANSFORM MODEL
	if(((mode&0x0FF)==0x05)||((mode&0xFF)==0x06)){
		outColor=vec4(justOneColor, 1.0);
		origVertex=origVertex.xzy;//change model Y<->Z
		origVertex=(modelTransf*vec4(origVertex,1.0)).xyz;//note 1.0

	}
	//////////////////////////////////////
	// 2. ADAPT TO TERRAIN
	if(((mode&0xFF)==0x03)&&(terrainMode==0)){//flat terrain--> Compute height
		vec2 terrainTexCoord=vec2(
			(origVertex.x-terrain_size.x)/terrain_size.z,
			(origVertex.y-terrain_size.y)/terrain_size.w
			);
		float height = texture(terrain_tex,terrainTexCoord.rg).r;
		outColor.r=height;
	}

	if((((mode&0xFF)==0x03)||((mode&0x0100)==0x0100))&&terrainMode==1){// terrain or adapt to terrain (and terrainMode=1)
		vec2 terrainTexCoord=vec2(
			(origVertex.x-terrain_size.x)/terrain_size.z,
			(origVertex.y-terrain_size.y)/terrain_size.w
			);
		//float height=255.0f*length(texture(terrain_tex,terrainTexCoord.rg));
		float height = texture(terrain_tex,terrainTexCoord.rg).r;

		origVertex.z+=height;

		if((mode&0xFF)==0x03){// terrain
			// computer normal from heightmap
			const vec2 size = vec2(1.0,0.0);
			const ivec3 off = ivec3(-1,0,1);

			float s01 = textureOffset(terrain_tex, terrainTexCoord.rg, off.xy).r;
			float s21 = textureOffset(terrain_tex, terrainTexCoord.rg, off.zy).r;
			float s10 = textureOffset(terrain_tex, terrainTexCoord.rg, off.yx).r;
			float s12 = textureOffset(terrain_tex, terrainTexCoord.rg, off.yz).r;

			//vec3 va = normalize(vec3(size.xy,10*(s21-s01)));
			//vec3 vb = normalize(vec3(size.yx,10*(s12-s10)));

			// GEN 1/17/2015
			// Originally UCHAR image is used and the texture data is [0, 1]
			// Now I use FLOAT image and the texture data is [0, infinate]
			// 0.039 is just a good number to make the things look 3D
			vec3 va = normalize(vec3(size.xy, (s21-s01) * 0.039));
			vec3 vb = normalize(vec3(size.yx, (s12-s10) * 0.039));

			varyingNormal=cross(va,vb);
		}
	}
	//////////////////////////////////////
	// SHADOW: From light
	if(shadowState==2){
		gl_Position = light_mvpMatrix * vec4(origVertex,1.0);
		return;
	}
	//////////////////////////////////////
	// WATER
	if((mode&0xFF)==0x04){
		vec3 u = normalize( vec3(mvMatrix * vec4(origVertex,1.0)) );
		vec3 n = normalize( normalMatrix * normal );
		vec3 r = reflect( u, n );
		float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
		m*=2.0;// NACHO
		outUV.s = r.x/m + 0.5;
		outUV.t = r.y/m + 0.5;
	}
	//////////////////////////////////////
	// LIGHTING
	if((mode&0x0200)==0x0200){
		//varyingNormal=normalMatrix*normal;
		varyingNormal=normalize(normal);//here to avoid doing it in the fragment
		// TRANSFORM MODEL (it should change its normal too)
		if(((mode&0x0FF)==0x05)||((mode&0xFF)==0x06)){
			varyingNormal=normal.xzy;
			varyingNormal=(modelTransf*vec4(varyingNormal,0.0)).xyz;//note 0.0
			//varyingNormal=normalize(varyingNormal);
		}
	}

	gl_Position = mvpMatrix * vec4(origVertex,1.0);

}