#pragma once

#include "Block.h"

class PmParcels {
public:
	static bool generateParcels(VBORenderManager& rendManager, std::vector<Block>& blocks);

private:
	static void subdivideBlockIntoParcels(const Polygon3D& contour, std::vector<Parcel>& parcels);
	static bool subdivideParcel(Parcel& parcel, float areaMean, float areaVar, float splitIrregularity, std::vector<Parcel>& outParcels);
};
