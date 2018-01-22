#include "VBOModel_StreetElements.h"
#include "VBORenderManager.h"


	std::vector<VBOModel> VBOModel_StreetElements::streetElementsModels;
	bool VBOModel_StreetElements::streetElements_wasInitialized=false;


	void VBOModel_StreetElements::clearVBOModel_StreetElements(){
		//destroy models
		for(int mN=0;mN<streetElementsModels.size();mN++){
			streetElementsModels[mN].clearModel();
		}
		streetElementsModels.clear();
	}//

	void VBOModel_StreetElements::initStreetElements(){
		printf("initStreetElements...\n");
		// init models
		// tree
		VBOModel m;
		int ind=streetElementsModels.size();
		streetElementsModels.push_back(m);
		std::vector<QString> fileNames;

		fileNames.push_back("data/models/nacho_leaves2.obj");
		fileNames.push_back("data/models/nacho_trunk2.obj");
		
		streetElementsModels[ind].initModel(fileNames);
		//streetElementsModels[ind].initScale(0.1f);//trees too big
		streetElementsModels[ind].initScale(0.04f);
		streetElementsModels[ind].loadModel();

		// street light
		ind=streetElementsModels.size();
		streetElementsModels.push_back(m);
		fileNames.clear();
		fileNames.push_back("data/models/street_lamp.obj");//3m
		streetElementsModels[ind].initModel(fileNames);
		streetElementsModels[ind].initScale(4.0f);
		streetElementsModels[ind].loadModel();

		/*// sun
		ind=streetElementsModels.size();
		streetElementsModels.push_back(m);
		fileNames.clear();
		fileNames.push_back("data\\models\\sun.obj");//3m
		streetElementsModels[ind].initModel(fileNames);
		streetElementsModels[ind].initScale(1000.0f);
		streetElementsModels[ind].loadModel();*/

		streetElements_wasInitialized=true;
	}


	void VBOModel_StreetElements::renderOneStreetElement(int programId,ModelSpec& treeStr){
		if(streetElements_wasInitialized==false){
			initStreetElements();
		}
		glCullFace(GL_FRONT);
		glUniform1i (glGetUniformLocation (programId, "mode"), 5|mode_Lighting|mode_AdaptTerrain);//model obj: one color

		streetElementsModels[treeStr.type].renderModel(programId, treeStr);
		
		glCullFace(GL_BACK);
		VBOUtil::check_gl_error("RenderOneStreetElement");
	}//
