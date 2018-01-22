#include "VBOSkyBox.h"
#include <QFileInfo>
#include "VBOUtil.h"

#include "VBORenderManager.h"


	VBOSkyBox::VBOSkyBox() {
		//initialized=0;
	}

	VBOSkyBox::~VBOSkyBox() {
	}

	void VBOSkyBox::init(VBORenderManager& rendManager){

		rendManager.removeStaticGeometry(QString("sky"));//in case of skychange

		float scale = rendManager.size.x();//5000.0f;
		QString imgName="data/sky/skyCombined.png";
		VBOUtil::check_gl_error("Init SkyboxVBO");

		float r = 1.0005f; // If you have border issues change this to 1.005f
		// Common Axis X - LEFT side
		std::vector<Vertex> sky_Vert;
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f,-r,-r),QVector3D(0.0f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f, r,-r),QVector3D(0.5f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f, r, r),QVector3D(0.5f,1.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f,-r, r),QVector3D(0.0f,1.0f,0)));	

		// Common Axis Z - FRONT side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,1.0f,-r),QVector3D(0.5f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D( r,1.0f,-r),QVector3D(1.0f,2/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r,1.0f, r),QVector3D(1.0f,1.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,1.0f, r),QVector3D(0.5f,1.0f,0)));

		// Common Axis X - Right side

		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f, r,-r),QVector3D(0.0f,1/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f,-r,-r),QVector3D(0.5f,1/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f,-r, r),QVector3D(0.5f,2/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f, r, r),QVector3D(0.0f,2/3.0f,0)));

		// Common Axis Z - BACK side
	
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-1.0f,-r),QVector3D(0.5f,1/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-1.0f,-r),QVector3D(1.0f,1/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-1.0f, r),QVector3D(1.0f,2/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-1.0f, r),QVector3D(0.5f,2/3.0f,0)));

		// Common Axis Y - Draw Up side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r, r,1.0f),QVector3D(0.0f,0.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r, r,1.0f),QVector3D(0.5f,0.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-r,1.0f),QVector3D(0.5f,1/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-r,1.0f),QVector3D(0.0f,1/3.0f,0)));


		// Common Axis Y - Draw Down side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-r,-1.0f),QVector3D(0.5f,0.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-r,-1.0f),QVector3D(1.0f,0.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r, r,-1.0f),QVector3D(1.0f,1/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r, r,-1.0f),QVector3D(0.5f,1/3.0f,0)));

		//rendManager.createVAO(sky_Vert,skyVBO,skyVAO,numVertex);
		rendManager.addStaticGeometry(QString("sky"),sky_Vert,imgName,GL_QUADS,2);// texture not lighting


		//initialized=1;
		printf("Loading SkyBox images... loaded %d\n",sky_Vert.size());


		//initialized=1;
	}//

