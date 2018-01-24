#include "RoadGraphEditor.h"
#include "OSMRoadsParser.h"
#include "OSMRoadsExporter.h"
#include <QTextStream>
#include <QDomDocument>

RoadGraphEditor::RoadGraphEditor() {
	origin = QPoint(0, 0);// width() * 0.5, height() * 0.5);
	scale = 0.1;

	vertex_selected = false;
	edge_selected = false;
	edge_point_selected = false;
	vertex_moved = false;
	adding_new_edge = false;
}

void RoadGraphEditor::moveCamera(const QPointF& d) {
	origin += d;
}

void RoadGraphEditor::zoom(double d, int screen_width, int screen_height) {
	double new_scale = scale + d;
	new_scale = std::min(std::max(0.1, new_scale), 1000.0);

	// adjust the origin in order to keep the screen center
	QVector2D p = screenToWorldCoordinates(screen_width * 0.5, screen_height* 0.5);
	origin.setX(screen_width * 0.5 - p.x() * new_scale);
	origin.setY(screen_height * 0.5 + p.y() * new_scale);

	scale = new_scale;
}

// adjust the origin in order to keep the screen center
void RoadGraphEditor::resize(const QSize& old_size, const QSize& new_size) {
	QVector2D p = screenToWorldCoordinates(old_size.width() * 0.5, old_size.height() * 0.5);
	origin.setX(new_size.width() * 0.5 - p.x() * scale);
	origin.setY(new_size.height() * 0.5 + p.y() * scale);

}

void RoadGraphEditor::load(const QString& filename) {
	roads.clear();
	vertex_selected = false;
	edge_selected = false;
	edge_point_selected = false;

	OSMRoadsParser parser(&roads);
	QXmlSimpleReader reader;
	reader.setContentHandler(&parser);
	QFile file(filename);
	QXmlInputSource source(&file);
	reader.parse(source);

	//roads.reduce();
	//roads.planarify();
}

void RoadGraphEditor::save(const QString& filename) {
	OSMRoadsExporter::save(filename, roads);
}

void RoadGraphEditor::undo() {
	try {
		roads = history.undo();
	}
	catch (char* ex) {
	}

	// clear the selection
	vertex_selected = false;
	edge_selected = false;
	edge_point_selected = false;
}

void RoadGraphEditor::redo() {
	try {
		roads = history.redo();
	}
	catch (char* ex) {
	}

	// clear the selection
	vertex_selected = false;
	edge_selected = false;
	edge_point_selected = false;
}

void RoadGraphEditor::deleteEdge() {
	if (edge_selected) {
		history.push(roads);
		roads.deleteEdge(selected_edge_desc);
		edge_selected = false;
	}
}

void RoadGraphEditor::draw(QPainter& painter, const QPointF& current_mouse_pos) {
	// draw road edges
	painter.setPen(QPen(QColor(128, 128, 255), 1));
	painter.setBrush(QColor(128, 128, 255));
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		QPolygonF polygon;
		for (int i = 0; i < roads.graph[*ei]->polyline.size(); i++) {
			QVector2D pt = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			polygon.push_back(QPointF(pt.x(), pt.y()));
		}
		painter.drawPolyline(polygon);

		for (int i = 1; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QVector2D pt = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			painter.drawEllipse(pt.x() - 1, pt.y() - 1, 3, 3);
		}
	}

	// draw selected edge
	if (edge_selected) {
		painter.setPen(QPen(QColor(0, 0, 128), 2));
		painter.setBrush(QColor(0, 0, 128));

		QPolygonF polygon;
		for (int i = 0; i < roads.graph[selected_edge_desc]->polyline.size(); i++) {
			QVector2D pt = worldToScreenCoordinates(roads.graph[selected_edge_desc]->polyline[i]);
			polygon.push_back(QPointF(pt.x(), pt.y()));
		}
		painter.drawPolyline(polygon);

		for (int i = 1; i < roads.graph[selected_edge_desc]->polyline.size() - 1; i++) {
			QVector2D pt = worldToScreenCoordinates(roads.graph[selected_edge_desc]->polyline[i]);
			painter.drawEllipse(pt.x() - 2, pt.y() - 2, 5, 5);
		}
	}

	if (edge_point_selected) {
		painter.setPen(QPen(QColor(0, 0, 0), 1));
		painter.setBrush(QBrush(QColor(0, 0, 0)));

		QVector2D pt = worldToScreenCoordinates(roads.graph[selected_edge_desc]->polyline[selected_edge_point]);
		painter.drawEllipse(pt.x() - 2, pt.y() - 2, 5, 5);
	}

	// draw road vertices
	painter.setPen(QPen(QColor(192, 192, 192), 1));
	painter.setBrush(QBrush(QColor(255, 255, 255)));
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; vi++) {
		if (!roads.graph[*vi]->valid) continue;

		QVector2D pt = worldToScreenCoordinates(roads.graph[*vi]->pt);
		painter.drawEllipse(pt.x() - 2, pt.y() - 2, 5, 5);
	}

	// draw selected vertex
	if (vertex_selected) {
		painter.setPen(QPen(QColor(0, 0, 0), 3));
		painter.setBrush(QBrush(QColor(255, 255, 255)));

		QVector2D pt = worldToScreenCoordinates(roads.graph[selected_vertex_desc]->pt);
		painter.drawEllipse(pt.x() - 2, pt.y() - 2, 5, 5);
	}

	// draw the adding edge
	if (adding_new_edge) {
		painter.setPen(QPen(QColor(0, 0, 128), 2));
		painter.setBrush(QColor(0, 0, 128));
		QPolygonF polygon;
		for (int i = 0; i < new_edge.size(); i++) {
			QVector2D pt = worldToScreenCoordinates(new_edge[i]);
			polygon.push_back(QPointF(pt.x(), pt.y()));
		}
		polygon.push_back(current_mouse_pos);
		painter.drawPolyline(polygon);

		for (int i = 0; i < new_edge.size(); i++) {
			QVector2D pt = worldToScreenCoordinates(new_edge[i]);
			painter.drawEllipse(pt.x() - 2, pt.y() - 2, 5, 5);
		}
	}
}

void RoadGraphEditor::addVertex(const QVector2D& pt) {
	// add a vertex
	if (!adding_new_edge) {
		adding_new_edge = true;
		new_edge.clear();
	}
	new_edge.push_back(pt);

	// resest other flags
	vertex_moved = false;
	vertex_selected = false;
	edge_selected = false;
	edge_point_selected = false;
}

void RoadGraphEditor::completeAddingVertex() {
	if (new_edge.size() == 1) {
		RoadEdgeDesc closest_edge_desc;
		QVector2D closest_pt;

		if (findClosestEdge(new_edge[0], 10, closest_edge_desc, closest_pt)) {
			history.push(roads);

			// add a vertex on the edge
			selected_vertex_desc = roads.splitEdge(closest_edge_desc, closest_pt);
			vertex_selected = true;
		}
	}
	else if (new_edge.size() >= 2) {
		history.push(roads);

		// add the new edge
		for (int i = 0; i < new_edge.size() - 1; i++) {
			RoadVertexDesc src;
			RoadEdgeDesc closest_edge_desc;
			QVector2D closest_pt;
			if (findClosestVertex(new_edge[i], 10, src)) {
			}
			else if (findClosestEdge(new_edge[i], 10, closest_edge_desc, closest_pt)) {
				src = roads.splitEdge(closest_edge_desc, closest_pt);
			}
			else {
				RoadVertexPtr v = RoadVertexPtr(new RoadVertex(new_edge[i]));
				src = boost::add_vertex(roads.graph);
				roads.graph[src] = v;
			}

			RoadVertexDesc tgt;
			if (findClosestVertex(new_edge[i + 1], 10, tgt)) {
			}
			else if (findClosestEdge(new_edge[i + 1], 10, closest_edge_desc, closest_pt)) {
				tgt = roads.splitEdge(closest_edge_desc, closest_pt);
			}
			else {
				RoadVertexPtr v = RoadVertexPtr(new RoadVertex(new_edge[i + 1]));
				tgt = boost::add_vertex(roads.graph);
				roads.graph[tgt] = v;
			}

			std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(src, tgt, roads.graph);
			roads.graph[edge_pair.first] = RoadEdgePtr(new RoadEdge(RoadEdge::TYPE_STREET, 1));
			roads.graph[edge_pair.first]->polyline.push_back(roads.graph[src]->pt);
			roads.graph[edge_pair.first]->polyline.push_back(roads.graph[tgt]->pt);
		}
	}

	adding_new_edge = false;

}

bool RoadGraphEditor::selectVertex(const QVector2D& pt) {
	// reset all flags
	vertex_selected = false;
	vertex_moved = false;
	edge_selected = false;
	edge_point_selected = false;
	adding_new_edge = false;

	if (findClosestVertex(pt, 10, selected_vertex_desc)) {
		vertex_selected = true;

		// even though the selected vertex may not be moved, we back up the current roads just in case.
		// later, when the mouse is released, we check whether the vertex is actually moved, and
		// if it is not moved, we cancel the back up.
		history.push(roads);

		return true;
	}
	else {
		return false;
	}
}

bool RoadGraphEditor::selectEdge(const QVector2D& pt) {
	// reset all flags
	vertex_selected = false;
	vertex_moved = false;
	edge_selected = false;
	edge_point_selected = false;
	adding_new_edge = false;

	if (findClosestEdge(pt, 9, selected_edge_desc)) {
		edge_selected = true;

		return true;
	}
	else {
		return false;
	}
}

/**
 * Move the current vertex to the specified position.
 * If there is any vertex close to that, the current vertex is moved to the position of the closest one,
 * but they are not merged at this moment.
 */
void RoadGraphEditor::moveVertex(const QVector2D& pt) {
	RoadVertexDesc target_vertex_desc;
	RoadEdgeDesc target_edge_desc;
	QVector2D closest_pt;
	if (findClosestVertexExcept(pt, 10, selected_vertex_desc, target_vertex_desc)) {
		roads.moveVertex(selected_vertex_desc, roads.graph[target_vertex_desc]->pt);
	}
	else if (findClosestEdgeExcept(pt, 10, selected_vertex_desc, target_edge_desc, closest_pt)) {
		roads.moveVertex(selected_vertex_desc, closest_pt);
	}
	else {
		// move the currently selected vertex
		roads.moveVertex(selected_vertex_desc, pt);
	}

	// we set this flag on so that when the mouse is released, we know that the vertex is moved, 
	// so we keep the backup. Otherwise, the back will be discarded.
	vertex_moved = true;
}

/**
 * Complete moving the current vertex.
 * If there is any vertex close, the current vertex will be snaped to that and they will be merged.
 */
void RoadGraphEditor::completeMovingVertex(const QVector2D& pt) {
	if (!vertex_moved) {
		// if the currently selected vertex was not moved at all, cancel backuping the current state of roads
		history.undo();
	}
	else {
		// merge the snapped vertex to the closest one if that exists.
		RoadVertexDesc target_vertex_desc;
		RoadEdgeDesc target_edge_desc;
		QVector2D closest_pt;
		if (findClosestVertexExcept(pt, 10, selected_vertex_desc, target_vertex_desc)) {
			if (roads.snapVertex(selected_vertex_desc, target_vertex_desc)) {
				selected_vertex_desc = target_vertex_desc;
			}
			/*else {
				vertex_selected = false;
			}*/
		}
		else if (findClosestEdgeExcept(pt, 10, selected_vertex_desc, target_edge_desc, closest_pt)) {
			target_vertex_desc = roads.splitEdge(target_edge_desc, closest_pt);
			if (roads.snapVertex(selected_vertex_desc, target_vertex_desc)) {
				selected_vertex_desc = target_vertex_desc;
			}
			/*else {
				vertex_selected = false;
			}*/
		}
	}
}

/**
* Find the closest vertex.
*
* @param pt			coordinates in the world coordinate system
* @param threshold	distance threshold in the screen space
*/
bool RoadGraphEditor::findClosestVertex(const QVector2D& pt, float threshold, RoadVertexDesc& closest_vertex_desc) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; vi++) {
		if (!roads.graph[*vi]->valid) continue;

		QVector2D vert_pt = worldToScreenCoordinates(roads.graph[*vi]->pt);
		float dist = std::hypot(vert_pt.x() - screen_pt.x(), vert_pt.y() - screen_pt.y());
		if (dist < min_dist) {
			min_dist = dist;
			closest_vertex_desc = *vi;
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}
}

/**
* Find the closest vertex.
*
* @param x		x coordinate in the world coordinate system
* @param y		y coordinate in the world coordinate system
*/
bool RoadGraphEditor::findClosestVertexExcept(const QVector2D& pt, float threshold, RoadVertexDesc except_vertex, RoadVertexDesc& closest_vertex_desc) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; vi++) {
		if (!roads.graph[*vi]->valid || *vi == except_vertex) continue;

		QVector2D vert_pt = worldToScreenCoordinates(roads.graph[*vi]->pt);
		float dist = std::hypot(vert_pt.x() - screen_pt.x(), vert_pt.y() - screen_pt.y());
		if (dist < min_dist) {
			min_dist = dist;
			closest_vertex_desc = *vi;
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}
}

bool RoadGraphEditor::findClosestEdgePoint(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc, int& closest_edge_point) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		for (int i = 0; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QVector2D p = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			double dist = (p - screen_pt).length();
			if (dist < min_dist) {
				min_dist = dist;
				closest_edge_desc = *ei;
				closest_edge_point = i;
			}
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}

}

/**
* Find the closest edge.
*
* @param pt		coordinates in the world coordinate system
*/
bool RoadGraphEditor::findClosestEdge(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		for (int i = 0; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QVector2D p0 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			QVector2D p1 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i + 1]);
			double dist = RoadGraph::pointSegmentDistance(p0, p1, screen_pt);
			if (dist < min_dist) {
				min_dist = dist;
				closest_edge_desc = *ei;
			}
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}
}

/**
* Find the closest edge.
*
* @param pt				coordinates of the given point in the world coordinate system
* @param closest_pt		coordinates of the closest point in the world coordinate system
*/
bool RoadGraphEditor::findClosestEdge(const QVector2D& pt, float threshold, RoadEdgeDesc& closest_edge_desc, QVector2D& closest_pt) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		for (int i = 0; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QVector2D p0 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			QVector2D p1 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i + 1]);
			QVector2D p2;
			double dist = RoadGraph::pointSegmentDistance(p0, p1, screen_pt, p2);
			if (dist < min_dist) {
				min_dist = dist;
				closest_edge_desc = *ei;
				closest_pt = screenToWorldCoordinates(p2);
			}
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}
}

bool RoadGraphEditor::findClosestEdgeExcept(const QVector2D& pt, float threshold, RoadVertexDesc except_vertex, RoadEdgeDesc& closest_edge_desc, QVector2D& closest_pt) {
	float min_dist = std::numeric_limits<float>::max();

	QVector2D screen_pt = worldToScreenCoordinates(pt);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ei++) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (except_vertex == src || except_vertex == tgt) continue;

		for (int i = 0; i < roads.graph[*ei]->polyline.size() - 1; i++) {
			QVector2D p0 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i]);
			QVector2D p1 = worldToScreenCoordinates(roads.graph[*ei]->polyline[i + 1]);
			QVector2D p2;
			double dist = RoadGraph::pointSegmentDistance(p0, p1, screen_pt, p2);
			if (dist < min_dist) {
				min_dist = dist;
				closest_edge_desc = *ei;
				closest_pt = screenToWorldCoordinates(p2);
			}
		}
	}

	if (min_dist < threshold) {
		return true;
	}
	else {
		return false;
	}
}

QVector2D RoadGraphEditor::screenToWorldCoordinates(const QVector2D& p) {
	return screenToWorldCoordinates(p.x(), p.y());
}

QVector2D RoadGraphEditor::screenToWorldCoordinates(double x, double y) {
	return QVector2D((x - origin.x()) / scale, -(y - origin.y()) / scale);
}

QVector2D RoadGraphEditor::worldToScreenCoordinates(const QVector2D& p) {
	return QVector2D(origin.x() + p.x() * scale, origin.y() - p.y() * scale);
}
