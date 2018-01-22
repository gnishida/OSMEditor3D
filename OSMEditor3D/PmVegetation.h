#pragma once

#include "Block.h"
#include "VBOModel_StreetElements.h"

class VBORenderManager;

class PmVegetation {
public:
	PmVegetation();
	~PmVegetation();
	
	static ModelSpec addTree(const QVector3D& pos);
	static ModelSpec addStreetLap(const QVector3D& pos, const QVector3D& contourDir);
	static bool generateVegetation(VBORenderManager& rendManager, const std::vector<Block>& blocks);

};
