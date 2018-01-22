#include "PmBuildings.h"
#include "global.h"
#include "Util.h"

bool PmBuildings::generateBuildings(VBORenderManager& rendManager, std::vector<Block>& blocks) {
	for (int i = 0; i < blocks.size(); ++i) {
		for (int j = 0; j < blocks[i].parcels.size(); ++j) {
			if (!generateBuilding(rendManager, blocks[i], blocks[i].parcels[j])) {
				blocks[i].parcels[j].isPark = true;
			}
		}
	}

	return true;
}

/**
 * Generate a building in a parcel.
 * If the parcel is too small, return false.
 *
 * @param rendManager	render manager
 * @param block			block
 * @param parcel		parcel
 * @return				true if the building is generated, false otherwise
 */
bool PmBuildings::generateBuilding(VBORenderManager& rendManager, Block& block, Parcel& parcel) {
	Loop3D pContourCpy;
	
	if (parcel.isPark) return false;

	// Compute parcel frontage
	std::vector<int> frontEdges;
	std::vector<int> rearEdges;
	std::vector<int> sideEdges;

	// simplify the contour in order to obtain a simple building footprint
	Polygon3D contour = parcel.parcelContour;
	contour.simplify(0.5);

	block.findParcelFrontAndBackEdges(contour, frontEdges, rearEdges, sideEdges);

	// Compute building footprint
	Loop3D footprint;
	Parcel::computeBuildingFootprint(contour, G::getFloat("parcel_setback_front"), G::getFloat("parcel_setback_rear"), G::getFloat("parcel_setback_sides"), frontEdges, rearEdges, sideEdges, footprint);
	if (footprint.size() == 0) return false;
	if (footprint.isSelfIntersecting()) {
		return false;
	}

	parcel.building.buildingFootprint.contour = footprint;

	// Cancel the building if the dimension is too small
	QVector3D obbSize;
	QMatrix4x4 obbMat;
	parcel.building.buildingFootprint.getMyOBB(obbSize, obbMat);
	if (obbSize.x() < G::getFloat("building_min_dimension") || obbSize.y() < G::getFloat("building_min_dimension")) return false;

	// set the elevation
	for (int i = 0; i < parcel.building.buildingFootprint.size(); ++i) {
		parcel.building.buildingFootprint[i].setZ(0.0f);
	}

	// Set building attributes
	parcel.building.numStories = Util::genRand(G::getInt("building_stories_min"), G::getInt("building_stories_max") + 1);
	float c = rand() % 192;
	parcel.building.color = QColor(c, c, c);
	parcel.building.bldType = 1;

	return true;
}
