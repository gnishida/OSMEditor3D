#include "RoadEdge.h"

RoadEdge::RoadEdge(unsigned int type, unsigned int lanes, bool oneWay, bool link, bool roundabout) {
	this->type = type;
	this->lanes = lanes;
	this->oneWay = oneWay;
	this->link = link;
	this->roundabout = roundabout;

	// initialize other members
	this->valid = true;
}

RoadEdge::~RoadEdge() {
}

float RoadEdge::getLength() {
	float length = 0.0f;
	for (int i = 0; i < polyline.size() - 1; i++) {
		length += (polyline[i + 1] - polyline[i]).length();
	}

	return length;
}

/**
 * Add a point to the polyline of the road segment.
 *
 * @param pt		new point to be added
 */
void RoadEdge::addPoint(const QVector2D &pt) {
	polyline.push_back(pt);
}

float RoadEdge::getWidth(float widthPerLane) {
	switch (type) {
	case TYPE_HIGHWAY:
		return widthPerLane * 2.0f * 0.01f;
	case TYPE_BOULEVARD:
	case TYPE_AVENUE:
		return widthPerLane * 1.5f * 0.01f;
	case TYPE_STREET:
		return widthPerLane * 0.01f;
	default:
		return 0.0f;
	}
}


