#pragma once

#include "Block.h"

class PmBuildings {
public:
	static bool generateBuildings(VBORenderManager& rendManager, std::vector<Block>& blocks);
	static bool generateBuilding(VBORenderManager& rendManager, Block& block, Parcel& parcel);
};

