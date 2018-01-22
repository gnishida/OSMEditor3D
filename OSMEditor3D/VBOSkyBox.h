#pragma once

#include "glew.h"


	class VBORenderManager;

	class VBOSkyBox {

	public:
		VBOSkyBox();
		~VBOSkyBox();

		//void render(VBORenderManager& rendManager);
		void init(VBORenderManager& rendManager);

	};

