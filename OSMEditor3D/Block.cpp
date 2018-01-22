#include "Block.h"
#include <QVector2D>

void Block::clear() {
	this->blockContours.clear();
	this->sidewalkContour.clear();
	this->sidewalkContourRoadsWidths.clear();
	this->parcels.clear();
}

void Block::findParcelFrontAndBackEdges(const Polygon3D& parcelContour, std::vector<int>& frontEdges, std::vector<int>& rearEdges, std::vector<int>& sideEdges) {
	frontEdges.clear();
	rearEdges.clear();
	sideEdges.clear();

	float kDistTol = 0.5f;

	for (int i = 0; i < parcelContour.size(); ++i) {
		int next = ((i + 1)) % parcelContour.size();

		QVector3D midPt = (parcelContour[i] + parcelContour[next]) * 0.5f;

		float distPtThis = distanceXYToPoint(parcelContour[i]);
		float distPtNext = distanceXYToPoint(parcelContour[next]);
		float distPtMid = distanceXYToPoint(midPt);
		int numPtsThatAreClose = (int)(distPtThis < kDistTol) + (int)(distPtMid < kDistTol) + (int)(distPtNext < kDistTol);

		bool midPtIsClose = (distPtThis < kDistTol);

		switch (numPtsThatAreClose) {
		case 0:
			rearEdges.push_back(i);
			break;
		case 1:					
		case 2:
			sideEdges.push_back(i);
			break;
		case 3:
			frontEdges.push_back(i);
			break;
		}
	}
}

float Block::distanceXYToPoint(const QVector3D& pt) {
	float min_dist = std::numeric_limits<float>::max();
	for (int i = 0; i < blockContours.size(); ++i) {
		float dist = blockContours[i].distanceXYToPoint(pt);
		if (dist < min_dist) min_dist = dist;
	}

	return min_dist;
}