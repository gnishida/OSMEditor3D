#pragma once

#include "glew.h"

#include "VBORenderManager.h"
#include "RoadGraph.h"
#include "BlockSet.h"
#include <glm/glm.hpp>
#include "Building.h"

class MainWindow;

class UrbanGeometry {
public:
	int width;
	int depth;
	MainWindow* mainWin;
	RoadGraph roads;
	BlockSet blocks;
	glm::vec2 minBound;
	glm::vec2 maxBound;

public:
	UrbanGeometry(MainWindow* mainWin);

	void generateBlocks();
	void generateParcels();
	void generateBuildings();
	void generateVegetation();
	void generateAll(bool updateGeometry);

	void update(VBORenderManager& vboRenderManager);

	void loadRoads(const QString& filename);
	void saveRoads(const QString& filename);
	void clear();
};
