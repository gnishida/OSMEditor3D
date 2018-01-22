#include "Polyline2D.h"
#include "Util.h"

const QVector2D& Polyline2D::last() const {
	return at(size() - 1);
}

QVector2D& Polyline2D::last() {
	return at(size() - 1);
}

const QVector2D& Polyline2D::nextLast() const {
	return at(size() - 2);
}

QVector2D& Polyline2D::nextLast() {
	return at(size() - 2);
}

/**
 * translate this polyline.
 */
void Polyline2D::translate(const QVector2D &offset) {
	for (int i = 0; i < size(); ++i) {
		(*this)[i] += offset;
	}
}

void Polyline2D::translate(float x, float y, Polyline2D &ret) const {
	ret.clear();
	ret.resize(size());

	for (int i = 0; i < size(); ++i) {
		ret[i].setX((*this)[i].x() + x);
		ret[i].setY((*this)[i].y() + y);
	}
}

/**
 * rotate this polygon clockwise around the origin.
 *
 * @param angle		angle [degree]
 */
void Polyline2D::rotate(float degree) {
	for (int i = 0; i < size(); ++i) {
		(*this)[i] = Util::rotate(at(i), -Util::deg2rad(degree));
	}
}

/**
 * rotate this polygon clockwise around the given point.
 *
 * @param angle		angle [degree]
 * @param orig		rotation center
 */
void Polyline2D::rotate(float degree, const QVector2D &orig) {
	for (int i = 0; i < size(); ++i) {
		(*this)[i] = Util::rotate(at(i), -Util::deg2rad(degree), orig);
	}
}

/**
 * scale this polyline.
 */
void Polyline2D::scale(float factor) {
	for (int i = 0; i < size(); ++i) {
		(*this)[i] = QVector2D(at(i).x() * factor, at(i).y() * factor);
	}
}

/**
 * return the length of this polyline
 */
float Polyline2D::length() const {
	return length(size() - 1);
}

/**
 * return the length of this polyline
 */
float Polyline2D::length(int index) const {
	float length = 0.0f;

	for (int i = 0; i < index; ++i) {
		length += (at(i + 1) - at(i)).length();
	}

	return length;
}
