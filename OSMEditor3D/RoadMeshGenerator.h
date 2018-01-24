#pragma once

#include "VBORenderManager.h"
#include "RoadGraph.h"
#include <vector>
#include <QVector3D>

class RoadMeshGenerator {
public:
	RoadMeshGenerator() {}

	static void generateRoadMesh(VBORenderManager& rendManager, RoadGraph& roads);
};

