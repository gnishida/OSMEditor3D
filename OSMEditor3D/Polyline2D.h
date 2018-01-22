#pragma once

#include "common.h"

class Polyline2D : public std::vector<QVector2D> {
public:
	Polyline2D() {}
	~Polyline2D() {}

	const QVector2D& last() const;
	QVector2D& last();
	const QVector2D& nextLast() const;
	QVector2D& nextLast();

	void translate(const QVector2D &offset);
	void translate(float x, float y, Polyline2D &ret) const;
	void rotate(float degree);
	void rotate(float degree, const QVector2D &orig);
	void scale(float factor);

	float length() const;
	float length(int index) const;
};

/**
 * use Polyline2D as boost Linestring
 */
BOOST_GEOMETRY_REGISTER_LINESTRING(Polyline2D)
