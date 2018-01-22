#include "Parcel.h"

Parcel::Parcel() {
	isPark = false;
}

/**
* Compute Parcel Buildable Area
**/
void Parcel::computeBuildingFootprint(const Polygon3D& contour, float frontSetback, float rearSetback, float sideSetback, const std::vector<int> &frontEdges, const std::vector<int> &rearEdges, const std::vector<int> &sideEdges, Loop3D &footprint) {
	footprint.clear();

	// workaround for 0 offset
	if (frontSetback == 0.0f) frontSetback = 0.0000001f;
	if (rearSetback == 0.0f) rearSetback = 0.0000001f;
	if (sideSetback == 0.0f) sideSetback = 0.0000001f;

	if (contour.size() < 3) return;

	std::vector<float> offsetValues(contour.size());

	// set the offset values
	for (int i = 0; i < frontEdges.size(); ++i) {
		if (frontEdges[i] < offsetValues.size()) {
			offsetValues[frontEdges[i]] = frontSetback;
		}
	}
	for (int i = 0; i < rearEdges.size(); ++i) {
		if (rearEdges[i] < offsetValues.size()) {
			offsetValues[rearEdges[i]] = rearSetback;
		}
	}
	for (int i = 0; i < sideEdges.size(); ++i) {
		if (sideEdges[i] < offsetValues.size()) {
			offsetValues[sideEdges[i]] = sideSetback;
		}
	}

	// compute offset
	std::vector<Loop3D> offsetPolygons;
	contour.offsetInside(offsetValues, offsetPolygons);

	if (offsetPolygons.size() > 0) {
		// currently only one building per parcel is supported
		footprint = offsetPolygons[0];
	}
}
