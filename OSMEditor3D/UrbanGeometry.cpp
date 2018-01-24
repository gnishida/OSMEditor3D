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
}

void UrbanGeometry::generateParcels() {
	PmParcels::generateParcels(mainWin->glWidget->vboRenderManager, blocks.blocks);
}

void UrbanGeometry::generateBuildings() {
	PmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);
}

void UrbanGeometry::generateVegetation() {
	PmVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
}

void UrbanGeometry::generateAll(bool updateGeometry) {
	srand(0);

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

	if (!G::getBool("shader2D")) {
		RoadMeshGenerator::generateRoadMesh(vboRenderManager, roads);
		BlockMeshGenerator::generateBlockMesh(vboRenderManager, blocks);
		BlockMeshGenerator::generateParcelMesh(vboRenderManager, blocks);
		Pm::generateBuildings(mainWin->glWidget->vboRenderManager, blocks);
		PmVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	}
}

void UrbanGeometry::clear() {
	roads.clear();
	blocks.clear();
	update(mainWin->glWidget->vboRenderManager);
}


