#pragma once

#include <QPainter>
#include "RoadGraph.h"
#include "History.h"

class RoadGraphEditor {
public:
	RoadGraph roads;

	History history;

	QPointF origin;
	double scale;

	bool vertex_selected;
	RoadVertexDesc selected_vertex_desc;
	bool edge_selected;
	RoadEdgeDesc selected_edge_desc;
	bool edge_point_selected;
	int selected_edge_point;
	bool vertex_moved;
	bool adding_new_edge;
	std::vector<QVector2D> new_edge;
	
public:
	RoadGraphEditor();

	void moveCamera(const QPointF& d);
	void zoom(double d, int screen_width, int screen_height);
	void resize(const QSize& old_size, const QSize& new_size);
	void load(const QString& filename);
	void save(const QString& filename);
	void undo();
	void redo();
	void deleteEdge();
	void draw(QPainter& paitner, const QPointF& current_mouse_pos);

	// operation by mouse
	void addVertex(const QVector2D& pt);
	void completeAddingVertex();
	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);
	void moveVertex(const QVector2D& pt);
	void completeMovingVertex(const QVector2D& pt);

	// helper functions
	bool findClosestVertex(const QVector2D& pt, float threshold, RoadVertexDesc& closest_vertex_desc);
	bool findClosestVertexExcept(const QVector2D& pt, float threshold, RoadVertexDesc except_vertex, RoadVertexDesc& closest_vertex_desc);
	bool findClosestEdgePoint(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc, int& closest_edge_point);
	bool findClosestEdge(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc);
	bool findClosestEdge(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc, QVector2D& closest_pt);
	bool findClosestEdgeExcept(const QVector2D& pt, float threshold, RoadVertexDesc except_vertex, RoadEdgeDesc& closest_edge_desc, QVector2D& closest_pt);
	QVector2D screenToWorldCoordinates(const QVector2D& p);
	QVector2D screenToWorldCoordinates(double x, double y);
	QVector2D worldToScreenCoordinates(const QVector2D& p);
};

