#pragma once

#include "common.h"

class Polyline3D : public std::vector<QVector3D> {
public:
	Polyline3D() {}
	~Polyline3D() {}

	const QVector3D & last() const;
	QVector3D & last();

	void translate(const QVector2D &offset);
	void rotate(float angle, const QVector2D &orig);
	void scale(float factor);
	float length() const;
};

/**
 * Linestringを定義
 */
BOOST_GEOMETRY_REGISTER_LINESTRING(Polyline3D)
