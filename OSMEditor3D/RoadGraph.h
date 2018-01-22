#pragma once

#include <QVector2D>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/shared_ptr.hpp>
#include "RoadVertex.h"
#include "RoadEdge.h"

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, RoadVertexPtr, RoadEdgePtr> BGLGraph;
typedef graph_traits<BGLGraph>::vertex_descriptor RoadVertexDesc;
typedef graph_traits<BGLGraph>::edge_descriptor RoadEdgeDesc;
typedef graph_traits<BGLGraph>::vertex_iterator RoadVertexIter;
typedef graph_traits<BGLGraph>::edge_iterator RoadEdgeIter;
typedef graph_traits<BGLGraph>::out_edge_iterator RoadOutEdgeIter;
typedef graph_traits<BGLGraph>::in_edge_iterator RoadInEdgeIter;

class RoadGraph {
private:
	static double EPS;

public:
	BGLGraph graph;
	QVector2D centerLonLat;

	// for rendering (These variables should be updated via setZ() function only!!
	float highwayHeight;
	float avenueHeight;
	float widthBase;
	float curbRatio;

	QColor colorHighway;
	QColor colorBoulevard;
	QColor colorAvenue;
	QColor colorStreet;
	bool showHighways;
	bool showBoulevard;
	bool showAvenues;
	bool showLocalStreets;

public:
	RoadGraph();
	~RoadGraph();

	void clear();
	RoadGraph clone();
	int getDegree(RoadVertexDesc v);
	void reduce();
	bool reduce(RoadVertexDesc desc);
	void moveVertex(RoadVertexDesc v, const QVector2D& pt);
	void movePolyline(std::vector<QVector2D>& polyline, const QVector2D& tgt_pos);
	bool hasEdge(RoadVertexDesc desc1, RoadVertexDesc desc2);
	RoadEdgeDesc getEdge(RoadVertexDesc src, RoadVertexDesc tgt);
	bool getEdge(const QVector2D &pt, float threshold, RoadEdgeDesc& e);
	void deleteEdge(RoadEdgeDesc desc);
	bool snapVertex(RoadVertexDesc v1, RoadVertexDesc v2);
	Polyline2D orderPolyLine(RoadEdgeDesc e, RoadVertexDesc src);
	RoadVertexDesc splitEdge(RoadEdgeDesc edge_desc, const QVector2D& pt);
	void planarify();
	bool planarifyOne();

	static float pointSegmentDistance(const QVector2D &a, const QVector2D &b, const QVector2D &c);
	static float pointSegmentDistance(const QVector2D &a, const QVector2D &b, const QVector2D &c, QVector2D& closest_pt);
	static bool segmentSegmentIntersect(const QVector2D& a, const QVector2D& b, const QVector2D& c, const QVector2D& d, float *tab, float *tcd, QVector2D& intPoint);
	static QVector2D projLatLonToMeter(double longitude, double latitude, const QVector2D& centerLonLat);
	static std::pair<double, double> projMeterToLatLon(const QVector2D& pos, const QVector2D& centerLonLat);
};

typedef boost::shared_ptr<RoadGraph> RoadGraphPtr;
