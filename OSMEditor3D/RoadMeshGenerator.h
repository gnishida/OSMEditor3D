#pragma once

#include "VBORenderManager.h"
#include "RoadGraph.h"
#include <vector>
#include <QVector3D>

class RoadMeshGenerator {
public:
	RoadMeshGenerator() {}

	static void generateRoadMesh(VBORenderManager& rendManager, RoadGraph& roads);
	static void generate2DRoadMesh(VBORenderManager& rendManager, RoadGraph& roads);

private:
	static std::vector<QVector3D> generateCurvePoints(const QVector3D& intPoint, const QVector3D& p1, const QVector3D& p2);
};

