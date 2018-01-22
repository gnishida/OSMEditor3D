#pragma once

#include <QString>
#include <QtXml/qxml.h>
#include <QVector3D>
#include "RoadGraph.h"

class RoadNode;
class RoadEdge;

typedef struct {
	QString parentNodeName;
	unsigned long long way_id;
	bool isStreet;
	bool oneWay;
	bool link;
	bool roundabout;
	bool bridge;
	uint lanes;
	uint type;
	std::vector<unsigned long long> nds;
} Way;

class OSMRoadsParser : public QXmlDefaultHandler {
private:
	//static double M_PI;

private:
	// temporary data for parsing ways
	Way way;

	//QVector3D centerLatLon;
	QVector2D centerLonLat;

	RoadGraph* roads;

	/** temporary node list */
	QMap<unsigned long long, RoadVertexDesc> idToDesc;
	QMap<unsigned long long, unsigned long long> idToActualId;
	QMap<unsigned long long, RoadVertex> vertices;

public:
	/** node list to be output to XML file */
	QMap<unsigned long long, RoadNode*> nodes;

	/** edge list to be output to XML file */
	std::vector<RoadEdge*> edges;

public:
	OSMRoadsParser(RoadGraph* roads);

	bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
	bool characters(const QString &ch_in);
	bool endElement(const QString&, const QString& localName, const QString& qName);

private:
	void handleBounds(const QXmlAttributes &atts);
	void handleNode(const QXmlAttributes &atts);
	void handleWay(const QXmlAttributes &atts);
	void handleNd(const QXmlAttributes &atts);
	void handleTag(const QXmlAttributes &atts);
	void createRoadEdge();
};

