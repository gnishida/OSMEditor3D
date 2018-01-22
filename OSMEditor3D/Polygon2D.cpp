#include "common.h"
#include "Polygon2D.h"
#include "Polyline2D.h"
#include "Util.h"

void Polygon2D::correct() {
	boost::geometry::correct(*this);
}

float Polygon2D::area() const {
	Polygon2D closedPoints = *this;
	if (this->front().x() != this->back().x() || this->front().y() != this->back().y()) {
		closedPoints.push_back(closedPoints.front());
	}
	closedPoints.correct();

	return fabs(boost::geometry::area(closedPoints));
}

QVector2D Polygon2D::centroid() const {
	QVector2D ret;
	boost::geometry::centroid(*this, ret);

	return ret;
}

bool Polygon2D::contains(const QVector2D &pt) const {
	Polygon2D closedPoints = *this;
	if (this->front().x() != this->back().x() || this->front().y() != this->back().y()) {
		closedPoints.push_back(closedPoints.front());
	}
	closedPoints.correct();
	return boost::geometry::within(pt, closedPoints);
}

bool Polygon2D::contains(const Polygon2D &polygon) const {
	for (int i = 0; i < polygon.size(); ++i) {
		if (!contains(polygon[i])) return false;
	}

	return true;
}

Polygon2D Polygon2D::convexHull() const {
	Polygon2D hull;
	Polygon2D poly;
	
	boost::geometry::convex_hull(*this, hull);

	return hull;
}

BBox Polygon2D::envelope() const {
	BBox bbox;
	boost::geometry::envelope(*this, bbox);
	return bbox;
}

/**
 * compute the intersection of this polygon with the given line segment. If they intersect each other, then return true. Otherwise return false.
 *
 * @param a		One point of the given line segment
 * @param b		Another point of the given line segment
 * @param intPt	The intersection
 * @return		true if they intersect each other. false otherwise.
 */ 
bool Polygon2D::intersects(const QVector2D& a, const QVector2D& b, QVector2D& intPt) const {
	Polyline2D polyline;
	polyline.push_back(a);
	polyline.push_back(b);

	std::vector<QVector2D> points;
	boost::geometry::intersection(*this, polyline, points);
	if (points.size() > 0) {
		intPt = points[0];
		return true;
	} else {
		return false;
	}
}

/**
 * translate this polygon.
 */
void Polygon2D::translate(float x, float y) {
	Polygon2D temp = *this;
	this->clear();

	/*boost::geometry::strategy::transform::translate_transformer<QVector2D, QVector2D> translate(x, y);
    boost::geometry::transform(temp, *this, translate);
	*/
}

/**
 * translate this polygon.
 *
 * @param x			x coordinate of the translation
 * @param y			y coordinate of the translation
 * @ret				the translated polygon
 */
void Polygon2D::translate(float x, float y, Polygon2D &ret) const {
	/*boost::geometry::strategy::transform::translate_transformer<QVector2D, QVector2D> translate(x, y);
    boost::geometry::transform(*this, ret, translate);
	*/
}

/**
 * rotate this polygon clockwise around the origin.
 *
 * @param angle		angle [degree]
 */
void Polygon2D::rotate(float angle) {
	Polygon2D temp = *this;
	this->clear();

	/*boost::geometry::strategy::transform::rotate_transformer<QVector2D, QVector2D, boost::geometry::degree> rotate(angle);
    boost::geometry::transform(temp, *this, rotate);
	*/
}

/**
 * rotate this polygon clockwise around the origin.
 *
 * @param angle		angle [degree]
 * @ret				the rotated polygon
 */
void Polygon2D::rotate(float angle, Polygon2D &ret) const {
	/*
	boost::geometry::strategy::transform::rotate_transformer<QVector2D, QVector2D, boost::geometry::degree> rotate(angle);
    boost::geometry::transform(*this, ret, rotate);
	*/
}

/**
 * rotate this polygon clockwise around the given point.
 *
 * @param angle		angle [degree]
 * @param orig		the point around which this polygon rotates
 */
void Polygon2D::rotate(float angle, const QVector2D &orig) {
	translate(-orig.x(), -orig.y());
	rotate(angle);
	translate(orig.x(), orig.y());
}

std::vector<Polygon2D> Polygon2D::union_(const Polygon2D &polygon) {
	typedef boost::geometry::model::polygon<QVector2D> boostPolygon;
	boostPolygon pgon1, pgon2;
	for (int i = 0; i < size(); ++i) {
		pgon1.outer().push_back(at(i));
	}
	boost::geometry::correct(pgon1);
	for (int i = 0; i < polygon.size(); ++i) {
		pgon2.outer().push_back(polygon[i]);
	}
	boost::geometry::correct(pgon2);

	std::vector<boostPolygon> output;
    boost::geometry::union_(pgon1, pgon2, output);

	std::vector<Polygon2D> ret;
	for (int i = 0; i < output.size(); ++i) {
		Polygon2D pg;
		for (int j = output[i].outer().size() - 2; j >= 0; --j) {
			pg.push_back(output[i].outer()[j]);
		}
		ret.push_back(pg);
	}

	return ret;
}

void Polygon2D::simplify(float threshold) {
	Polyline2D polyline;
	for (int i = 0; i < size(); i++) {
		polyline.push_back(at(i));
	}

	Polyline2D simplified;
	boost::geometry::simplify(polyline, simplified, threshold);

	clear();

	for (int i = 0; i < simplified.size(); i++) {
		push_back(simplified[i]);
	}
}

/**
 * tessellate this polygon.
 * assume that the polygon is open and the vertices are in CCW order.
 */
std::vector<Polygon2D> Polygon2D::tessellate() {
	std::vector<Polygon2D> trapezoids;

	if (size() < 3) return trapezoids;

	// create 2D polygon data
	std::vector<boost::polygon::point_data<float>  > polygon;
	polygon.resize(size());
	for (int i = 0; i < size(); i++) {
		polygon[i] = boost::polygon::construct<boost::polygon::point_data<float> >(at(i).x(), at(i).y());
	}

	// create 2D polygon with holes data
	boost::polygon::polygon_with_holes_data<float> temp;
	boost::polygon::set_points(temp, polygon.begin(), polygon.end());

	// create 2D polygon set
	boost::polygon::polygon_set_data<float> polygonSet;
	polygonSet.insert(temp);

	// tessellation
	std::vector<boost::polygon::polygon_with_holes_data<float> > results;
	polygonSet.get_trapezoids(results);

	for (int i = 0; i < results.size(); i++) {
		boost::polygon::polygon_with_holes_data<float>::iterator_type it = results[i].begin();
		Polygon2D trapezoid;
		while (it != results[i].end()) {
			float z = 0.0f;
			bool done = false;

			trapezoid.push_back(QVector2D((*it).x(), (*it).y()));
			it++;
		}

		if (trapezoid.size() < 3) continue;

		//Polygon2D::reorientFace(trapezoid);

		// The resulting polygon is usually closed, so remove the last point.
		if ((trapezoid[trapezoid.size() - 1] - trapezoid[0]).lengthSquared() == 0) {
			//trapezoid.pop_back();
		}

		if (trapezoid.size() >= 3) trapezoids.push_back(trapezoid);
	}

	return trapezoids;
}

/**
* Get polygon oriented bounding box
**/
QVector2D Polygon2D::getOBB(Polygon2D &obb) const {
	QVector2D minSize;
	float minArea = std::numeric_limits<float>::max();

	int cSz = this->size();
	for (int i=0; i<cSz; ++i) {
		QVector2D dir = (at((i+1) % cSz) - at(i)).normalized();
		Polygon2D rotBBox;
		QVector2D size = getOBB(dir, rotBBox);

		float area = rotBBox.area();
		if (area < minArea) {
			minArea = area;
			minSize = size;
			obb = rotBBox;
		}
	}

	return minSize;
}

/**
 * 指定されたX軸方向を使って、AABBを計算する。
 *
 * @dir			指定されたX軸方向
 * @obb			求められたOBB
 * @bbox		求められたOBBの、ローカル座標におけるbbox
 **/
QVector2D Polygon2D::getOBB(const QVector2D& dir, Polygon2D& obb) const {
	// もともとのポリゴンの中心を保存する
	QVector2D center = this->centroid();

	// 指定された方向がX軸になるよう、ポリゴンを回転する
	float alpha = atan2f(dir.y(), dir.x());
	Polygon2D rotPolygon;
	rotate(Util::rad2deg(alpha), rotPolygon);

	// 回転したポリゴンの中心が原点になるよう、移動する
	Polygon2D rotPolygon2;
	QVector2D center2 = rotPolygon.centroid();
	rotPolygon.translate(-center2.x(), -center2.y());

	// 回転したポリゴンに対して、Bounding Boxを求める
	BBox bbox = rotPolygon.envelope();

	// Bounding Boxのサイズを計算する
	QVector2D size(bbox.dx(), bbox.dy());

	// Bounding Boxをポリゴンオブジェクトにコンバートする
	boost::geometry::convert(bbox, obb);

	// Bounding Boxを、再度回転して戻す
	obb.rotate(Util::rad2deg(-alpha));

	// 再度、もともとのポリゴンの中心に戻す
	obb.translate(center.x(), center.y());

	return size;
}

boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > Polygon2D::convertToBoostPolygon() const {
	boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > ret;

	for (int i = 0; i < size(); ++i) {
		ret.outer().push_back(boost::geometry::model::d2::point_xy<double>(at(i).x(), at(i).y()));
	}

	return ret;
}
