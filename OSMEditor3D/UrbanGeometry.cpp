#include "UrbanGeometry.h"
#include <limits>
#include <iostream>
#include <QFile>
#include "common.h"
#include "global.h"
#include "MainWindow.h"
#include "Util.h"
#include "RoadMeshGenerator.h"
#include "BlockMeshGenerator.h"
#include "Pm.h"
#include "PmBlocks.h"
#include "PmParcels.h"
#include "PmBuildings.h"
#include "PmVegetation.h"
#include <QDir>
#include <QProcess>
#include "OSMRoadsParser.h"

UrbanGeometry::UrbanGeometry(MainWindow* mainWin) {
	this->mainWin = mainWin;
}

void UrbanGeometry::generateBlocks() {
	PmBlocks::generateBlocks(&mainWin->glWidget->vboRenderManager, roads, blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateParcels() {
	PmParcels::generateParcels(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateBuildings() {
	PmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateVegetation() {
	PmVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateAll(bool updateGeometry) {
	clear();

	// 2016/11/10
	// We decided to remove randomness.
	srand(0);

	// warm up the random numbers
	for (int i = 0; i < 100; ++i) rand();

	PmBlocks::generateBlocks(&mainWin->glWidget->vboRenderManager, roads, blocks);
	PmParcels::generateParcels(mainWin->glWidget->vboRenderManager, blocks.blocks);
	PmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);	

	if (updateGeometry) {
		update(mainWin->glWidget->vboRenderManager);
	}
}

/**
 * Update all the geometry
 *
 * This function may take time, so don't call this function so often.
 * It is recommnded to call this function only when the geometry is changed, added, or the 2d/3d mode is changed.
 */
void UrbanGeometry::update(VBORenderManager& vboRenderManager) {
	// Remove all the geometry
	vboRenderManager.removeStaticGeometry("3d_blocks");
	vboRenderManager.removeStaticGeometry("3d_parks");
	vboRenderManager.removeStaticGeometry("3d_parcels");
	vboRenderManager.removeStaticGeometry("3d_roads");
	vboRenderManager.removeStaticGeometry("3d_building");
	vboRenderManager.removeStaticGeometry("tree");
	vboRenderManager.removeStaticGeometry("streetLamp");

	if (G::getBool("shader2D")) {
		RoadMeshGenerator::generate2DRoadMesh(vboRenderManager, roads);
		BlockMeshGenerator::generate2DParcelMesh(vboRenderManager, blocks);
	}
	else {
		RoadMeshGenerator::generateRoadMesh(vboRenderManager, roads);
		BlockMeshGenerator::generateBlockMesh(vboRenderManager, blocks);
		BlockMeshGenerator::generateParcelMesh(vboRenderManager, blocks);
		Pm::generateBuildings(mainWin->glWidget->vboRenderManager, blocks);
		PmVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	}
}

void UrbanGeometry::loadRoads(const QString& filename) {
	roads.clear();

	OSMRoadsParser parser(&roads);
	QXmlSimpleReader reader;
	reader.setContentHandler(&parser);
	QFile file(filename);
	QXmlInputSource source(&file);
	reader.parse(source);

	roads.planarify();
	roads.reduce();
	roads = roads.clone();

	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::saveRoads(const QString &filename) {
	/*
	glm::vec2 offset = (maxBound + minBound) * 0.5f;

	gs::Shape shape(wkbLineString);
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		gs::ShapeObject shapeObject;
		shapeObject.parts.resize(1);
		for (int k = 0; k < roads.graph[*ei]->polyline.size(); ++k) {
			const QVector2D& pt = roads.graph[*ei]->polyline[k];
			float z = mainWin->glWidget->vboRenderManager.getTerrainHeight(pt.x(), pt.y());
			shapeObject.parts[0].points.push_back(glm::vec3(pt.x() + offset.x, pt.y() + offset.y, z));
		}

		shape.shapeObjects.push_back(shapeObject);
	}

	shape.save(filename);
	*/
}

void UrbanGeometry::clear() {
	roads.clear();
	blocks.clear();
	update(mainWin->glWidget->vboRenderManager);
}


