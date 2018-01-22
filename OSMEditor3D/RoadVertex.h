#pragma once

#include <vector>
#include <QVector2D>
#include <QVector3D>
#include <QHash>
#include <QVariant>
#include <boost/shared_ptr.hpp>

class RoadVertex {
public:
	QVector2D pt;
	bool valid;

public:
	RoadVertex();
	RoadVertex(const QVector2D &pt);
};

typedef boost::shared_ptr<RoadVertex> RoadVertexPtr;
