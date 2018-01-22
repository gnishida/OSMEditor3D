#include "OSMRoadsExporter.h"
#include <QFile>
#include <QTextStream>

void OSMRoadsExporter::save(const QString& filename, const RoadGraph& roads) {
	QFile file(filename);
	if (!file.open(QFile::WriteOnly)) throw "File cannot open.";

	QDomDocument doc;

	// set root node
	QDomElement root = doc.createElement("osm");
	root.setAttribute("generator", "OSM Editor");
	root.setAttribute("version", "0.6");
	doc.appendChild(root);

	// calculate the bounding box
	double minlon, maxlon, minlat, maxlat;
	calculateBounds(roads, minlon, maxlon, minlat, maxlat);

	// write boundary
	QDomElement bounds = doc.createElement("bounds");
	bounds.setAttribute("minlon", minlon);
	bounds.setAttribute("maxlon", maxlon);
	bounds.setAttribute("minlat", minlat);
	bounds.setAttribute("maxlat", maxlat);
	root.appendChild(bounds);

	int node_id = 0;

	// write nodes
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; vi++) {
		if (!roads.graph[*vi]->valid) continue;

		QDomElement node = doc.createElement("node");
		node.setAttribute("id", *vi);
		std::pair<double, double> lonlat = RoadGraph::projMeterToLatLon(roads.graph[*vi]->pt, roads.centerLonLat);
		node.setAttribute("lon", lonlat.first);
		node.setAttribute("lat", lonlat.second);
		root.appendChild(node);

		node_id = std::max(node_id, (int)*vi);
	}

	node_id++;

	int way_id = 0;

	// write ways
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!roads.graph[src]->valid || !roads.graph[tgt]->valid) continue;

		QDomElement way = doc.createElement("way");
		way.setAttribute("id", way_id++);

		if ((roads.graph[*ei]->polyline[0] - roads.graph[src]->pt).lengthSquared() < (roads.graph[*ei]->polyline[0] - roads.graph[tgt]->pt).lengthSquared()) {
			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref", src);
			way.appendChild(nd);
		}
		else {
			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref", tgt);
			way.appendChild(nd);
		}

		for (int i = 1; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QDomElement node = doc.createElement("node");
			node.setAttribute("id", node_id);
			std::pair<double, double> lonlat = RoadGraph::projMeterToLatLon(roads.graph[*ei]->polyline[i], roads.centerLonLat);
			node.setAttribute("lon", lonlat.first);
			node.setAttribute("lat", lonlat.second);
			root.appendChild(node);

			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref", node_id);
			way.appendChild(nd);

			node_id++;
		}

		if ((roads.graph[*ei]->polyline[0] - roads.graph[src]->pt).lengthSquared() < (roads.graph[*ei]->polyline[0] - roads.graph[tgt]->pt).lengthSquared()) {
			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref", tgt);
			way.appendChild(nd);
		}
		else {
			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref", src);
			way.appendChild(nd);
		}

		QDomElement tag_highway = doc.createElement("tag");
		tag_highway.setAttribute("k", "highway");
		if (roads.graph[*ei]->type == RoadEdge::TYPE_HIGHWAY) {
			tag_highway.setAttribute("v", "trunk");
		}
		else if (roads.graph[*ei]->type == RoadEdge::TYPE_BOULEVARD) {
			tag_highway.setAttribute("v", "primary");
		}
		else if (roads.graph[*ei]->type == RoadEdge::TYPE_AVENUE) {
			tag_highway.setAttribute("v", "secondary");
		}
		else if (roads.graph[*ei]->type == RoadEdge::TYPE_STREET) {
			tag_highway.setAttribute("v", "residential");
		}
		way.appendChild(tag_highway);

		QDomElement tag_lanes = doc.createElement("tag");
		tag_lanes.setAttribute("k", "lanes");
		tag_lanes.setAttribute("v", roads.graph[*ei]->lanes);
		way.appendChild(tag_lanes);

		QDomElement tag_oneway = doc.createElement("tag");
		tag_oneway.setAttribute("k", "oneway");
		tag_oneway.setAttribute("v", roads.graph[*ei]->oneWay ? "yes" : "no");
		way.appendChild(tag_oneway);

		root.appendChild(way);
	}

	QTextStream out(&file);
	doc.save(out, 4);
}

void OSMRoadsExporter::calculateBounds(const RoadGraph& roads, double& minlon, double& maxlon, double& minlat, double& maxlat) {
	minlon = std::numeric_limits<double>::max();
	maxlon = -std::numeric_limits<double>::max();
	minlat = std::numeric_limits<double>::max();
	maxlat = -std::numeric_limits<double>::max();

	// check the vertices
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; vi++) {
		if (!roads.graph[*vi]->valid) continue;

		std::pair<double, double> lonlat = RoadGraph::projMeterToLatLon(roads.graph[*vi]->pt, roads.centerLonLat);

		minlon = std::min(minlon, lonlat.first);
		maxlon = std::max(maxlon, lonlat.first);
		minlat = std::min(minlat, lonlat.second);
		maxlat = std::max(maxlat, lonlat.second);
	}

	// check the edges
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!roads.graph[src]->valid || !roads.graph[tgt]->valid) continue;

		for (int i = 1; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			std::pair<double, double> lonlat = RoadGraph::projMeterToLatLon(roads.graph[*ei]->polyline[i], roads.centerLonLat);

			minlon = std::min(minlon, lonlat.first);
			maxlon = std::max(maxlon, lonlat.first);
			minlat = std::min(minlat, lonlat.second);
			maxlat = std::max(maxlat, lonlat.second);
		}
	}
}