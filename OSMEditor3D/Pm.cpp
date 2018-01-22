#include "Pm.h"
#include "Polygon3D.h"

#include <qdir.h>
#include <QStringList>
#include <QTime>

#include "PmBlocks.h"
#include "PmParcels.h"
#include "PmBuildings.h"
#include "BlockSet.h"
#include "GeoBuilding.h"
#include "PmVegetation.h"
#include "Polygon3D.h"
#include "Util.h"

bool Pm::generateBuildings(VBORenderManager& rendManager, BlockSet& blocks) {
	rendManager.removeStaticGeometry("3d_building");
		
	for (int bN = 0; bN < blocks.size(); bN++) {
		if (blocks[bN].isPark) continue;

		for (int pN = 0; pN < blocks[bN].parcels.size(); ++pN) {
			if (blocks[bN].parcels[pN].isPark) continue;
			if (blocks[bN].parcels[pN].building.buildingFootprint.size() < 3) continue;

			GeoBuilding::generateBuilding(rendManager, blocks[bN].parcels[pN].building);
		}
	}
	printf("Building generation is done.\n");

	return true;
}

bool Pm::generateVegetation(VBORenderManager& rendManager, BlockSet& blocks) {
	PmVegetation::generateVegetation(rendManager, blocks.blocks);

	return true;
}
