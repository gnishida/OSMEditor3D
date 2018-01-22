#pragma once

#include <vector>
#include <QVector2D>
#include <QVector3D>
#include <QColor>
#include <boost/polygon/polygon.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp> 
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/geometry/multi/geometries/multi_point.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/shared_ptr.hpp>

#ifndef M_PI
#define M_PI		3.141592653589793238
#endif

#ifndef SQR
#define SQR(x)		((x) * (x))
#endif


/**
 * use QVector2D as Boost point
 */
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QVector2D, float, boost::geometry::cs::cartesian, x, y, setX, setY)

/**
 * use QVector3D as Boost point
 */
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QVector3D, float, boost::geometry::cs::cartesian, x, y, setX, setY)



