#pragma once

#include <vector>

#include "VBOModel.h"

class VBOModel_StreetElements{

public:

	static void clearVBOModel_StreetElements();
	static void initStreetElements();

	static void renderOneStreetElement(int programId,ModelSpec& strEleStr);

	static std::vector<VBOModel> streetElementsModels;
	static bool streetElements_wasInitialized;
};
		
