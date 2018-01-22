#pragma once

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "Block.h"
#include "Parcel.h"
#include "Building.h"
#include "RoadGraph.h"
#include "BlockSet.h"

class VBORenderManager;

class PmBlocks {
public:
	static bool generateBlocks(VBORenderManager* renderManager, RoadGraph& roadGraph, BlockSet& blocks);

	static bool removeIntersectingEdges(RoadGraph& roadGraph);
	static void buildEmbedding(RoadGraph& roads, std::vector<std::vector<RoadEdgeDesc>>& embedding);
	static void checkValidness(VBORenderManager* renderManager, BlockSet& blocks);
	static void generateSideWalk(VBORenderManager* renderManager, BlockSet& blocks);
	//static void saveBlockImage(const RoadGraph& roads, const Polygon3D& contour, const char* filename);
};


