#pragma once

#include "glew.h"

#include <QGenericMatrix>
#include "common.h"
#include "Polyline2D.h"

class Util {
	static const double MTC_FLOAT_TOL;

protected:
	Util();

public:
	static float pointSegmentDistanceXY(const QVector3D &a, const QVector3D &b, const QVector3D &c, bool segmentOnly = true);
	static QVector2D projLatLonToMeter(const QVector2D &latLon, const QVector2D &centerLatLon);
	static QVector2D projLatLonToMeter(double longitude, double latitude, const QVector2D &centerLatLon);

	static bool segmentSegmentIntersectXY(const QVector2D& a, const QVector2D& b, const QVector2D& c, const QVector2D& d, double *tab, double *tcd, bool segmentOnly, QVector2D &intPoint);
	static bool segmentSegmentIntersectXY3D(const QVector3D& a, const QVector3D& b, const QVector3D& c, const QVector3D& d, double *tab, double *tcd, bool segmentOnly, QVector3D &intPoint);
	static float pointSegmentDistanceXY(const QVector2D& a, const QVector2D& b, const QVector2D& c, QVector2D& closestPtInAB);

	// angle computatiopn
	static float deg2rad(float deg);
	static float rad2deg(float rad);
	static float normalizeAngle(float angle);
	static float diffAngle(const QVector2D& dir1, const QVector2D& dir2, bool absolute = true);
	static float diffAngle(const QVector3D& dir1, const QVector3D& dir2, bool absolute = true);
	static float diffAngle(float angle1, float angle2, bool absolute = true);
	static bool withinAngle(float angle, float angle1, float angle2);
	static float angleThreePoints(const QVector3D& pa, const QVector3D& pb, const QVector3D& pc);
	static double angleBetweenVectors(const QVector2D& vec1, const QVector2D& vec2);
	static double angleBetweenVectors(const QVector3D& vec1, const QVector3D& vec2);
	static QVector2D rotate(const QVector2D &pt, float rad);
	static QVector2D rotate(const QVector2D &pt, float rad, const QVector2D &orig);

	// coordinate conversion
	static void cartesian2polar(const QVector2D &pt, float &radius, float &theta);
	static QVector2D transform(const QVector2D &pt, const QVector2D &sourcePt, float rad, const QVector2D &targetPt);

	// curvature
	static float curvature(const Polyline2D &polyline);

	// random
	static float genRand();
	static float genRand(float a, float b);
	static int sampleFromCdf(std::vector<float> &cdf);
	static int sampleFromPdf(std::vector<float> &pdf);

	// Barycentric interpolation
	static float barycentricInterpolation(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector2D& p);

	static bool getIrregularBisector(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, float d01, float d12, QVector3D& intPt);
	static bool getIrregularBisector(const QVector2D& p0, const QVector2D& p1, const QVector2D& p2, float d01, float d12, QVector2D& intPt);
};
