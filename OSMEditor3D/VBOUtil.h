#pragma once

#include "glew.h"
#include "qstring.h"
#include "qvector3d.h"
#include <QColor>
#include <vector>

/////////////////////////
// Contaisn the Vertex structure to do the rendering
struct Vertex{
	float info[16];	

	// Contructors
	Vertex() {
	}

	Vertex(float x,float y,float z,float r,float g,float b,float a,float nX,float nY,float nZ,float tS,float tT,float tW){
		info[0]=x;
		info[1]=y;
		info[2]=z;

		info[4]=r;
		info[5]=g;
		info[6]=b;
		info[7]=a;

		info[8]=nX;
		info[9]=nY;
		info[10]=nZ;

		info[12]=tS;
		info[13]=tT;
		info[14]=tW;
	}

	Vertex(float x, float y, float z, const QColor& color, float nX, float nY, float nZ, float tS, float tT, float tW) {
		info[0] = x;
		info[1] = y;
		info[2] = z;
		info[4] = color.redF();
		info[5] = color.greenF();
		info[6] = color.blueF();
		info[7] = color.alphaF();
		info[8] = nX;
		info[9] = nY;
		info[10] = nZ;
		info[12] = tS;
		info[13] = tT;
		info[14] = tW;
	}

	Vertex(QVector3D pos,QColor color,QVector3D normal,QVector3D tex) {
		info[0]=pos.x();
		info[1]=pos.y();
		info[2]=pos.z();

		info[4]=color.redF();
		info[5]=color.greenF();
		info[6]=color.blueF();
		info[7]=color.alphaF();

		info[8]=normal.x();
		info[9]=normal.y();
		info[10]=normal.z();

		info[12]=tex.x();
		info[13]=tex.y();
		info[14]=tex.z();
	}//
	Vertex(QVector3D* pos,QColor* color=0,QVector3D* normal=0,QVector3D* tex=0){
		if(pos!=0){
			info[0]=pos->x();
			info[1]=pos->y();
			info[2]=pos->z();
		}
		if(color!=0){
			info[4]=color->redF();
			info[5]=color->greenF();
			info[6]=color->blueF();
			info[7]=color->alphaF();
		}
		if(normal!=0){
			info[8]=normal->x();
			info[9]=normal->y();
			info[10]=normal->z();
		}
		if(tex!=0){
			info[12]=tex->x();
			info[13]=tex->y();
			info[14]=tex->z();
		}
	}//
	Vertex(QVector3D pos,QVector3D tex){
		info[0]=pos.x();
		info[1]=pos.y();
		info[2]=pos.z();

		info[7]=1.0f;
		info[12]=tex.x();
		info[13]=tex.y();
		info[14]=tex.z();
	}//
	// Overload bracket operator
	float operator [](int i) const    {return info[i];}
	float & operator [](int i) {return info[i];}
	void print(){
		printf("pos %f %f %f col %f %f %f normal %f %f %f tex %f %f %f\n",info[0],info[1],info[2],info[3],info[4],info[5],info[6],info[7],info[8],info[9],info[10],info[11]);
	}
};


class VBOUtil{

public:

	static GLuint loadImage(const QString fileName,bool mirroredHor=false,bool mirroredVert=false);
	static GLuint loadImageArray(std::vector<QString> filaNames);

	static void check_gl_error(QString sourceTag);
	static void disaplay_memory_usage();
};

