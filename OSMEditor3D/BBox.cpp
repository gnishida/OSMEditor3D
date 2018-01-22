#include "BBox.h"
#include "Polyline2D.h"
#include "Polygon2D.h"

BBox::BBox() {
	minPt = QVector2D(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	maxPt = QVector2D(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
}

void BBox::addPoint(const QVector2D& pt) {
	boost::geometry::expand(*this, pt);
}

bool BBox::contains(const QVector2D& pt) const {
	return boost::geometry::within(pt, *this);
}

QVector2D BBox::midPt() const {
	QVector2D ret;
	boost::geometry::centroid(*this, ret);

	return ret;
}

float BBox::dx() const {
	return maxPt.x() - minPt.x();
}

float BBox::dy() const {
	return maxPt.y() - minPt.y();
}

float BBox::area() const {
	return boost::geometry::area(*this);
}

bool BBox::intersects(const QVector2D& a, const QVector2D& b, QVector2D& intPt) const {
	Polyline2D polyline;
	polyline.push_back(a);
	polyline.push_back(b);

	Polygon2D polygon;
	boost::geometry::convert(*this, polygon);

	std::vector<QVector2D> points;
	boost::geometry::intersection(polygon, polyline, points);
	if (points.size() > 0) {
		intPt = points[0];
		return true;
	} else {
		return false;
	}
}
