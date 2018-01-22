/************************************************************************************************
*		Polygon 3D
*		@author igarciad
************************************************************************************************/
#pragma once

#include "glew.h"
#include "common.h"
#include "BBox.h"
#include <vector>
#include <QVector3D>

#ifndef Q_MOC_RUN
#include <boost/graph/adjacency_list.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/multi/multi.hpp>
#endif

#ifndef BOOST_TYPEOF_SILENT
#define BOOST_TYPEOF_SILENT
#endif//BOOST_TYPEOF_SILENT


/**
* Geometry.
* Classes and functions for geometric data
**/
class Loop3D : public std::vector<QVector3D> {
public:
	bool contains(const QVector3D& pt) const;
	bool contains(const Loop3D& polygon) const;
	bool isClockwise() const;
	float area() const;
	bool isSelfIntersecting() const;
	float distanceXYToPoint(const QVector3D& pt) const;
	void simplify(float threshold);
};

/**
* Stores a polygon in 3D represented by its
*     exterior contour.
**/
class Polygon3D {
public:
	Loop3D contour;

public:
	Polygon3D() {}

	void clear() { contour.clear(); }
	bool isClockwise() const ;
	void correct();

	QVector3D& operator[](const int idx) {	
		return contour.at(idx);
	}

	const QVector3D& operator[](const int idx) const {
		return contour.at(idx);
	}

	size_t size() const { return contour.size(); }

	void push_back(const QVector3D &pt) {
		contour.push_back(pt);
	}

	void getBBox3D(QVector3D &ptMin, QVector3D &ptMax) const;
	bool isSelfIntersecting() const;
	BBox envelope() const;
	float area() const;
	void simplify(float threshold);

	//Only works for polygons with no holes in them
	bool splitMeWithPolyline(const std::vector<QVector3D> &pline, Loop3D &pgon1, Loop3D &pgon2);
	bool split(const std::vector<QVector3D> &pline, std::vector<Polygon3D>& pgons);

	void offsetOutside(float offsetDistance, Loop3D& polygonOffset) const;
	void offsetOutsideCGAL(float offsetDistance, Loop3D& polygonOffset) const;
	void offsetInside(float offsetDistance, std::vector<Loop3D>& pgonInsets) const;
	void offsetInside(std::vector<float>& offsetDistances, std::vector<Loop3D>& pgonInsets) const;
	void offsetInsideCGAL(float offsetDistance, std::vector<Loop3D>& pgonInsets) const;

	bool contains(const QVector3D& pt) const {
		return contour.contains(pt);
	}

	static void transformLoop(const Loop3D& pin, Loop3D& pout, const QMatrix4x4& transformMat);
	static QVector3D getLoopAABB(Loop3D &pin, QVector3D &minCorner, QVector3D &maxCorner);
	static void getLoopOBB(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat);
	static void getLoopOBB2(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat);
	static Loop3D getLoopOBB3(const Loop3D &pin);

	void getMyOBB(QVector3D &size, QMatrix4x4 &xformMat);
	float distanceXYToPoint(const QVector3D &pt) const;
	bool isTooNarrow(float ratio, float min_side) const;
};	

class BBox3D {
public:
	QVector3D minPt;
	QVector3D maxPt;

public:
	BBox3D(){
		this->resetMe();	
	}

	inline void resetMe(void) {
		minPt.setX(FLT_MAX);
		minPt.setY(FLT_MAX);
		minPt.setZ(FLT_MAX);
		maxPt.setX(-FLT_MAX);
		maxPt.setY(-FLT_MAX);
		maxPt.setZ(-FLT_MAX);
	}

	inline bool overlapsWithBBox3D(BBox3D &other){
		return  
			( (this->minPt.x() <= other.maxPt.x()) && (this->maxPt.x() >= other.minPt.x()) ) &&
			( (this->minPt.y() <= other.maxPt.y()) && (this->maxPt.y() >= other.minPt.y()) ) &&
			( (this->minPt.z() <= other.maxPt.z()) && (this->maxPt.z() >= other.minPt.z()) );
	}

	void combineWithBBox3D(BBox3D &other){
		minPt.setX(qMin(minPt.x(), other.minPt.x()));
		minPt.setY(qMin(minPt.y(), other.minPt.y()));
		minPt.setZ(qMin(minPt.z(), other.minPt.z()));

		maxPt.setX(qMax(maxPt.x(), other.maxPt.x()));
		maxPt.setY(qMax(maxPt.y(), other.maxPt.y()));
		maxPt.setZ(qMax(maxPt.z(), other.maxPt.z()));

		return;
	}

	void addPoint(QVector3D &newPt){
		minPt.setX(qMin(minPt.x(), newPt.x()));
		minPt.setY(qMin(minPt.y(), newPt.y()));
		minPt.setZ(qMin(minPt.z(), newPt.z()));

		maxPt.setX(qMax(maxPt.x(), newPt.x()));
		maxPt.setY(qMax(maxPt.y(), newPt.y()));
		maxPt.setZ(qMax(maxPt.z(), newPt.z()));
	}

	inline bool overlapsWithBBox3DXY(BBox3D &other){
		return  
			( (this->minPt.x() <= other.maxPt.x()) && (this->maxPt.x() >= other.minPt.x()) ) &&
			( (this->minPt.y() <= other.maxPt.y()) && (this->maxPt.y() >= other.minPt.y()) );					
	}

	inline QVector3D midPt(void) {
		return (0.5*(minPt + maxPt));
	}
};


// We can conveniently use macro's to register point and ring
//BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QVector3D, double, boost::geometry::cs::cartesian, x, y, setX, setY)
BOOST_GEOMETRY_REGISTER_RING(Loop3D)

	namespace boost {
		namespace geometry {

			namespace traits {

				template<> struct tag<Polygon3D> { typedef polygon_tag type; };

			} // namespace traits

			template<> struct ring_type<Polygon3D> { typedef Loop3D type; };

		} // namespace geometry
} // namespace boost

