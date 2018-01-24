#include "PmBlocks.h"
#include "Polygon3D.h"

#include <qdir.h>
#include <QStringList>
#include "Util.h"
#include "PmParcels.h"
#include "global.h"

///////////////////////////////////////////////////////////////
// BLOCKS
///////////////////////////////////////////////////////////////
RoadGraph * roadGraphPtr;
std::vector< Block > * blocksPtr;

Polygon3D sidewalkContourTmp;

std::vector< float > sidewalkContourWidths;
std::vector<RoadVertexDesc> visitedVs;

int face_index = 0;
bool vertex_output_visitor_invalid = false;

struct output_visitor : public boost::planar_face_traversal_visitor {
	void begin_face() {
		face_index++;
		sidewalkContourTmp.clear();
		sidewalkContourWidths.clear();

		vertex_output_visitor_invalid = false;
		visitedVs.clear();
	}

	void end_face() {
		if (vertex_output_visitor_invalid) {
			//printf("INVALID end face\n");
			return;
		}

		// Discard the face if it is clock wise order
		// because it is the contour of the graph.
		if (sidewalkContourTmp.isClockwise()) return;

		if (vertex_output_visitor_invalid){ 
			//printf("INVALID end face\n");

			char filename[255];
			sprintf(filename, "block_images/block_%d.jpg", face_index);
			//PmBlocks::saveBlockImage(*roadGraphPtr, sidewalkContourTmp, filename);

			return;
		}

		//if (blockContourTmp.area() > 100.0f) {
		if (sidewalkContourTmp.size() >= 3 && sidewalkContourWidths.size() >= 3) {
			Block newBlock;
			newBlock.sidewalkContour = sidewalkContourTmp;
			newBlock.sidewalkContourRoadsWidths = sidewalkContourWidths;
			while (newBlock.sidewalkContour.size() > newBlock.sidewalkContourRoadsWidths.size()) {
				newBlock.sidewalkContourRoadsWidths.push_back(newBlock.sidewalkContourRoadsWidths.back());
			}
	
			blocksPtr->push_back(newBlock);
			//printf("CREATE block %d: %d\n",blocksPtr->size(),blocksPtr->back().blockContour.contour.size());
		}else{
			//printf("Contour %d widths %d\n",sidewalkContourTmp.size(),sidewalkContourWidths.size());
		}
	}
};

//Vertex visitor
struct vertex_output_visitor : public output_visitor {
	template <typename Vertex> 
	void next_vertex(Vertex v) {
		if (v >= boost::num_vertices(roadGraphPtr->graph)) {
			vertex_output_visitor_invalid = true;
			return;
		}

		if (!vertex_output_visitor_invalid) {
			visitedVs.push_back(v);
			if (v >= boost::num_vertices(roadGraphPtr->graph)) {
				vertex_output_visitor_invalid = true;
				printf("INVALID vertex\n");
				return;
			}
		}
	}

	template <typename Edge> 
	void next_edge(Edge e)  {
		if (!vertex_output_visitor_invalid) {
			RoadVertexDesc src = boost::source(e, roadGraphPtr->graph);
			RoadVertexDesc tgt = boost::target(e, roadGraphPtr->graph);

			if (src == tgt) {
				vertex_output_visitor_invalid = true;
				return;
			}
			if (visitedVs.size() > 0 && src != visitedVs.back() && tgt != visitedVs.back()) {
				vertex_output_visitor_invalid = true;
				return;
			}
			if (src >= boost::num_vertices(roadGraphPtr->graph) || tgt >= boost::num_vertices(roadGraphPtr->graph)) {
				vertex_output_visitor_invalid = true;
				printf("INVALID edge\n");
				return;
			}

			Polyline2D polyline = roadGraphPtr->graph[e]->polyline;
			if ((roadGraphPtr->graph[visitedVs.back()]->pt - polyline.front()).lengthSquared() > (roadGraphPtr->graph[visitedVs.back()]->pt - polyline.back()).lengthSquared()) {
				std::reverse(polyline.begin(), polyline.end());
			}
			for (int i = 0; i < polyline.size() - 1; ++i) {
				sidewalkContourTmp.push_back(polyline[i]);
			}

			for (int i = 0; i < roadGraphPtr->graph[e]->polyline.size() - 1; ++i) {
				if (roadGraphPtr->graph[e]->type != RoadEdge::TYPE_STREET) {
					sidewalkContourWidths.push_back(G::getFloat("major_road_width"));
				}
				else {
					sidewalkContourWidths.push_back(G::getFloat("minor_road_width"));
				}
			}
		}
	}
};

/**
* Remove intersecting edges.
*/
bool PmBlocks::removeIntersectingEdges(RoadGraph& roadGraph) {
	/*
	std::vector<RoadEdgeIter> edgesToRemove;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roadGraph.graph); ei != eend; ++ei) {
		RoadVertexDesc src = boost::source(*ei, roadGraph.graph);
		RoadVertexDesc tgt = boost::target(*ei, roadGraph.graph);

		RoadEdgeIter ei2;
		for (ei2 = ei; ei2 != eend; ++ei2) {
			if (ei2 != ei) {
				RoadVertexDesc src2 = boost::source(*ei2, roadGraph.graph);
				RoadVertexDesc tgt2 = boost::target(*ei2, roadGraph.graph);

				if (src == src2 || src == tgt2 || tgt == src2 || tgt == tgt2) continue;

				if (GraphUtil::isIntersect(roadGraph, roadGraph.graph[*ei]->polyline, roadGraph.graph[*ei2]->polyline)) {
					if (std::find(edgesToRemove.begin(), edgesToRemove.end(), ei2) == edgesToRemove.end()) {
						edgesToRemove.push_back(ei2);

						RoadVertexDesc src = boost::source(*ei2, roadGraph.graph);
						RoadVertexDesc tgt = boost::target(*ei2, roadGraph.graph);
						//printf("remove edge: (%lf, %lf) - (%lf, %lf)\n", roadGraph.graph[src]->pt.x(), roadGraph.graph[src]->pt.y(), roadGraph.graph[tgt]->pt.x(), roadGraph.graph[tgt]->pt.y());
					}
				}
			}
		}		
	}

	for (int i = 0; i < edgesToRemove.size(); ++i) {
		boost::remove_edge(*(edgesToRemove[i]), roadGraph.graph);
	}

	if (edgesToRemove.size() > 0) {
		//printf("Edge removed %d\n", edgesToRemove.size());
		return true;
	} else {
		return false;
	}
	*/
	return true;
}


/**
 * Generate blocks from the road network
 */
bool PmBlocks::generateBlocks(VBORenderManager* renderManager, RoadGraph& roadGraph, BlockSet& blocks) {
	roadGraphPtr = &roadGraph;
	std::vector<Block> tmpBlocks;
	blocksPtr = &tmpBlocks;
	blocksPtr->clear();

	bool isPlanar = false;
	bool converges = true;
	
	// Make sure graph is planar
	typedef std::vector< RoadEdgeDesc > tEdgeDescriptorVector;
	std::vector<tEdgeDescriptorVector> embedding(boost::num_vertices(roadGraph.graph));

	// There should be no cross over, but I keep this just in case
	removeIntersectingEdges(roadGraph);
	
	// build embedding manually
	buildEmbedding(roadGraph, embedding);

	// Create edge index property map?	
	typedef std::map<RoadEdgeDesc, size_t> EdgeIndexMap;
	EdgeIndexMap mapEdgeIdx;
	boost::associative_property_map<EdgeIndexMap> pmEdgeIndex(mapEdgeIdx);		
	RoadEdgeIter ei, ei_end;	
	int edge_count = 0;
	for (boost::tie(ei, ei_end) = boost::edges(roadGraph.graph); ei != ei_end; ++ei) {
		mapEdgeIdx.insert(std::make_pair(*ei, edge_count++));	
	}

	// Extract blocks from road graph using boost graph planar_face_traversal
	vertex_output_visitor v_vis;	
	boost::planar_face_traversal(roadGraph.graph, &embedding[0], v_vis, pmEdgeIndex);

	// Remove invalid data
	for (int i = 0; i < tmpBlocks.size();) {
		if (tmpBlocks[i].sidewalkContour.size() != tmpBlocks[i].sidewalkContourRoadsWidths.size()) {
			//std::cout << "!!!!! Error: contour:" << tmpBlocks[i].sidewalkContour.size() << ", width: " << tmpBlocks[i].sidewalkContourRoadsWidths.size() << "\n";
			tmpBlocks.erase(tmpBlocks.begin() + i);
		}
		else if (tmpBlocks[i].sidewalkContour.size() < 3) {
			//std::cout << "!!!!! Error: Contour <3 " << "\n";
			tmpBlocks.erase(tmpBlocks.begin() + i);
		}
		else {
			i++;
		}
	}

	// Remove redundant point along the polygon
	for (int i = 0; i < tmpBlocks.size(); ++i) {
		for (int j = 1; j < tmpBlocks[i].sidewalkContour.size(); ) {
			if (tmpBlocks[i].sidewalkContour[j] == tmpBlocks[i].sidewalkContour[j - 1]) {
				tmpBlocks[i].sidewalkContour.contour.erase(tmpBlocks[i].sidewalkContour.contour.begin() + j);
				tmpBlocks[i].sidewalkContourRoadsWidths.erase(tmpBlocks[i].sidewalkContourRoadsWidths.begin() + j);
			}
			else {
				j++;
			}
		}
	}


	// order the blocks in counter clockwise order
	// This is not necessary any more, but let's keep this just in case.
	for (int i = 0; i < tmpBlocks.size(); ++i) {
		// order the face in counter clockwise
		if (tmpBlocks[i].sidewalkContour.isClockwise()) {
			std::reverse(tmpBlocks[i].sidewalkContour.contour.begin(), tmpBlocks[i].sidewalkContour.contour.end());
			std::reverse(tmpBlocks[i].sidewalkContourRoadsWidths.begin(), tmpBlocks[i].sidewalkContourRoadsWidths.end());
		}
	}

	blocks.clear();
	for (int i = 0; i < tmpBlocks.size(); ++i) {
		// Compute block offset
		std::vector<Loop3D> sidewalkContours;
		tmpBlocks[i].sidewalkContour.offsetInside(tmpBlocks[i].sidewalkContourRoadsWidths, sidewalkContours);

		for (int j = 0; j < sidewalkContours.size(); ++j) {
			blocks.blocks.push_back(Block());
			blocks.blocks.back().sidewalkContour.contour = sidewalkContours[j];
		}
	}
	
	// GEN: remove the blocks whose edges are less than 3
	// This problem is caused by the computeInset() function.
	// ToDo: fix the computeInset function.
	for (int i = 0; i < blocks.size(); ) {
		if (blocks[i].sidewalkContour.size() < 3) {
			blocks.blocks.erase(blocks.blocks.begin() + i);
		}
		else {
			i++;
		}
	}

	checkValidness(renderManager, blocks);
	generateSideWalk(renderManager, blocks);

	// make some blocks open space
	int num_parks = blocks.size() * G::getFloat("parksRatio");
	std::vector<int> block_ids;
	for (int i = 0; i < blocks.size(); ++i) block_ids.push_back(i);
	std::random_shuffle(block_ids.begin(), block_ids.end());
	for (int i = 0; i < num_parks; ++i) {
		blocks[block_ids[i]].isPark = true;
	}

	return true;
}

void PmBlocks::buildEmbedding(RoadGraph& roads, std::vector<std::vector<RoadEdgeDesc>>& embedding) {
	embedding.clear();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		if (!roads.graph[*vi]->valid) continue;

		QMap<float, RoadEdgeDesc> edges;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			Polyline2D polyline = roads.orderPolyLine(*ei, *vi);
			QVector2D vec = polyline[1] - polyline[0];
			edges[-atan2f(vec.y(), vec.x())] = *ei;
		}

		std::vector<RoadEdgeDesc> edge_descs;
		for (QMap<float, RoadEdgeDesc>::iterator it = edges.begin(); it != edges.end(); ++it) {
			edge_descs.push_back(it.value());

			RoadEdgePtr e = roads.graph[it.value()];
			Polyline2D pl = e->polyline;
		}

		embedding.push_back(edge_descs);
	}
}


void PmBlocks::checkValidness(VBORenderManager* renderManager, BlockSet& blocks) {
	for (int i = 0; i < blocks.size(); ) {
		BBox3D bbox;
		blocks[i].sidewalkContour.getBBox3D(bbox.minPt, bbox.maxPt);

		// If the block is too narrow, make it park.
		if (blocks[i].sidewalkContour.isTooNarrow(8.0f, 18.0f) || blocks[i].sidewalkContour.isTooNarrow(1.0f, 3.0f)) {
			blocks[i].isPark = true;
			i++;
			continue;
		}

		i++;

		// comment out the following because we assume the terrain is flat
		/*
		// If the block is below sea level, or on a steep terain, make it invalid or park.
		float min_z = std::numeric_limits<float>::max();
		float max_z = 0.0f;
		for (int pi = 0; pi < blocks[i].sidewalkContour.size(); ++pi) {
			int next_pi = (pi + 1) % blocks[i].sidewalkContour.size();
			for (int k = 0; k <= 20; ++k) {
				QVector3D pt = blocks[i].sidewalkContour[pi] * (float)(20 - k) * 0.05 + blocks[i].sidewalkContour[next_pi] * (float)k * 0.05;
				float z = renderManager->getTerrainHeight(pt.x(), pt.y());
				min_z = std::min(min_z, z);
				max_z = std::max(max_z, z);
			}
		}

		if (min_z < G::getFloat("sea_level")) {
			blocks.blocks.erase(blocks.blocks.begin() + i);
		}
		else {
			if (max_z - min_z > 20.0f) {
				blocks[i].isPark = true;
			}
			i++;
		}
		*/
	}
}

/**
 * Generate side walks
 */
void PmBlocks::generateSideWalk(VBORenderManager* renderManager, BlockSet& blocks) {
	// Compute the block contour (the outer part becomes sidewalks)
	for (int i = 0; i < blocks.size(); ++i) {
		Loop3D blockContourInset;
		float sidewalk_width = G::getFloat("sidewalk_width");
		std::vector<Loop3D> contours;
		blocks[i].sidewalkContour.offsetInside(sidewalk_width, contours);

		blocks[i].blockContours.resize(contours.size());
		for (int cN = 0; cN < contours.size(); ++cN) {
			for (int k = 0; k < contours[cN].size(); ++k) {
				blocks[i].blockContours[cN].push_back(contours[cN][k]);
			}
		}
	}
}

/*void PmBlocks::saveBlockImage(const RoadGraph& roads, const Polygon3D& contour, const char* filename) {
	BBox bbox = GraphUtil::getAABoundingBox(roads, true);
	cv::Mat img(bbox.dy() + 1, bbox.dx() + 1, CV_8UC3, cv::Scalar(255, 255, 255));

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		for (int pl = 0; pl < roads.graph[*ei]->polyline.size() - 1; ++pl) {
			int x1 = roads.graph[*ei]->polyline[pl].x() - bbox.minPt.x();
			int y1 = roads.graph[*ei]->polyline[pl].y() - bbox.minPt.y();
			int x2 = roads.graph[*ei]->polyline[pl + 1].x() - bbox.minPt.x();
			int y2 = roads.graph[*ei]->polyline[pl + 1].y() - bbox.minPt.y();
			cv::line(img, cv::Point(x1, img.rows - y1), cv::Point(x2, img.rows - y2), cv::Scalar(224, 224, 224), 3);
		}
	}

	for (int i = 0; i < contour.size(); ++i) {
		int next = (i + 1) % contour.size();
		int x1 = contour.contour[i].x() - bbox.minPt.x();
		int y1 = contour.contour[i].y() - bbox.minPt.y();
		int x2 = contour.contour[next].x() - bbox.minPt.x();
		int y2 = contour.contour[next].y() - bbox.minPt.y();
		cv::line(img, cv::Point(x1, img.rows - y1), cv::Point(x2, img.rows - y2), cv::Scalar(0, 0, 0), 3);
	}

	cv::imwrite(filename, img);
}*/
