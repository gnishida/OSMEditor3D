#include "RoadMeshGenerator.h"
#include "Util.h"
#include "global.h"

bool compare2ndPartTuple2 (const std::pair<float, RoadEdgeDesc> &i, const std::pair<float, RoadEdgeDesc> &j) {
	return (i.first > j.first);
}

void RoadMeshGenerator::generateRoadMesh(VBORenderManager& rendManager, RoadGraph& roads) {
	float deltaZ = 2.0f;

	std::vector<Vertex> vertSide;

	//////////////////////////////////////////
	// POLYLINES
	{
		float const maxSegmentLeng = 5.0f;

		RoadEdgeIter ei, eiEnd;
		int numEdges = 0;

		std::vector<Vertex> vertROAD[2];
		std::vector<Vertex> intersectCirclesV;
		for (boost::tie(ei, eiEnd) = boost::edges(roads.graph); ei != eiEnd; ++ei) {
			if (!roads.graph[*ei]->valid) continue;
			numEdges++;

			RoadEdgePtr edge = roads.graph[*ei];
			float hWidth;
			if (roads.graph[*ei]->type == RoadEdge::TYPE_AVENUE) {
				hWidth = roads.graph[*ei]->lanes * G::getFloat("major_road_width");
			}
			else {
				hWidth = roads.graph[*ei]->lanes * G::getFloat("minor_road_width");
			}
			
			int type;
			switch (roads.graph[*ei]->type) {
			case RoadEdge::TYPE_AVENUE:
				type = 1;
				break;
			case RoadEdge::TYPE_STREET:
				type = 0;
				break;
			default:
				type = 0;
				break;
			}
			
			float lengthMovedL = 0; // road texture dX
			float lengthMovedR = 0; // road texture dX

			QVector2D a0,a1,a2,a3;

			for (int pL = 0; pL < edge->polyline.size() - 1; pL++) {
				bool bigAngle = false;
				QVector2D p0 = edge->polyline[pL];
				QVector2D p1 = edge->polyline[pL+1];
				if ((p0 - p1).lengthSquared() <= 0.00001f) continue;

				QVector2D dir = p1 -p0;
				float length = dir.length();
				dir /= length;
				
				QVector2D per(-dir.y(), dir.x());
				if (pL == 0) {
					a0 = p0 - per * hWidth;
					a3 = p0 + per * hWidth;
				}
				a1 = p1 - per * hWidth;
				a2 = p1 + per * hWidth;

				QVector2D p2;
				if (pL < edge->polyline.size() - 2) {
					p2 = edge->polyline[pL + 2];
					
					// Use getIrregularBisector only if the two segments are not parallel.
					if (fabs(QVector2D::dotProduct((p2 - p1).normalized(), (p1 - p0).normalized())) < 0.99f) {
						Util::getIrregularBisector(p0, p1, p2, hWidth, hWidth, a2);
						Util::getIrregularBisector(p0, p1, p2, -hWidth, -hWidth, a1);
					}
				}
				
				float middLenghtR = length;
				float middLenghtL = length;
				float segmentLengR, segmentLengL;
				int numSegments = ceil(length/5.0f);

				float dW = 7.5f;//tex size in m

				QVector3D b0, b3;
				QVector3D b1 = a0;
				QVector3D b2 = a3;
				QVector3D vecR = a1 - a0;
				QVector3D vecL = a2 - a3;

				for(int nS=0;nS<numSegments;nS++){
					segmentLengR = std::min(maxSegmentLeng, middLenghtR);
					segmentLengL = std::min(maxSegmentLeng, middLenghtL);

					b0 = b1;
					b3 = b2;
					if (nS < numSegments - 1) {
						b1 += dir * segmentLengR;
						b2 += dir * segmentLengL;
					} else {
						b1 = a1;
						b2 = a2;
					}
									

					QVector3D b03 = (b0 + b3) * 0.5f;
					QVector3D b12 = (b1 + b2) * 0.5f;
					float z1 = rendManager.getTerrainHeight(b03.x(), b03.y());
					if (z1 < G::getFloat("road_min_level")) z1 = G::getFloat("road_min_level");
					float z2 = rendManager.getTerrainHeight(b12.x(), b12.y());
					if (z2 < G::getFloat("road_min_level")) z2 = G::getFloat("road_min_level");

					b0.setZ(z1 + deltaZ);
					b3.setZ(z1 + deltaZ);
					b1.setZ(z2 + deltaZ);
					b2.setZ(z2 + deltaZ);

					vertROAD[type].push_back(Vertex(b0, QColor(), QVector3D(0, 0, 1.0f), QVector3D(1, lengthMovedR / dW, 0)));
					vertROAD[type].push_back(Vertex(b1, QColor(), QVector3D(0, 0, 1.0f), QVector3D(1, (lengthMovedR + segmentLengR) / dW, 0)));
					vertROAD[type].push_back(Vertex(b2, QColor(), QVector3D(0, 0, 1.0f), QVector3D(0, (lengthMovedL + segmentLengL) / dW, 0)));
					vertROAD[type].push_back(Vertex(b3, QColor(), QVector3D(0, 0, 1.0f), QVector3D(0, lengthMovedL / dW, 0)));

					// side face geometry
					vertSide.push_back(Vertex(b0 + QVector3D(0, 0, -deltaZ * 2), QColor(64, 64, 64), -per, QVector3D()));
					vertSide.push_back(Vertex(b1 + QVector3D(0, 0, -deltaZ * 2), QColor(64, 64, 64), -per, QVector3D()));
					vertSide.push_back(Vertex(b1, QColor(64, 64, 64), -per, QVector3D()));
					vertSide.push_back(Vertex(b0, QColor(64, 64, 64), -per, QVector3D()));

					vertSide.push_back(Vertex(b2 + QVector3D(0, 0, -deltaZ * 2), QColor(64, 64, 64), per, QVector3D()));
					vertSide.push_back(Vertex(b3 + QVector3D(0, 0, -deltaZ * 2), QColor(64, 64, 64), per, QVector3D()));
					vertSide.push_back(Vertex(b3, QColor(64, 64, 64), per, QVector3D()));
					vertSide.push_back(Vertex(b2, QColor(64, 64, 64), per, QVector3D()));

					lengthMovedR += segmentLengR;
					lengthMovedL += segmentLengL;
					middLenghtR -= segmentLengR;
					middLenghtL -= segmentLengL;
				}				

				a3 = a2;
				a0 = a1;
			}
		}
		
		// add all geometry
		rendManager.addStaticGeometry("3d_roads", vertROAD[0], "data/textures/roads/road_2lines.jpg", GL_QUADS, 2);
		rendManager.addStaticGeometry("3d_roads", vertROAD[1], "data/textures/roads/road_4lines.jpg", GL_QUADS, 2);
	}

	//////////////////////////////////////////
	// Circle+Complex
	{
		// 2. INTERSECTIONS
		std::vector<Vertex> intersectCirclesV;
		std::vector<Vertex> interPedX;
		std::vector<Vertex> interPedXLineR;

		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			if (!roads.graph[*vi]->valid) continue;

			int outDegree = roads.getDegree(*vi);

			if (outDegree == 0) {
				continue;
			} else if (outDegree == 1) { // dead end
				// get the largest width of the outing edges
				float rad = 0.0f;
				float angle_offset = 0.0f;
				RoadOutEdgeIter oei, oeend;
				for (boost::tie(oei, oeend) = boost::out_edges(*vi, roads.graph); oei != oeend; ++oei) {
					if (!roads.graph[*oei]->valid) continue;

					if (roads.graph[*oei]->type == RoadEdge::TYPE_AVENUE) {
						rad = roads.graph[*oei]->lanes * G::getFloat("major_road_width");
					}
					else {
						rad = roads.graph[*oei]->lanes * G::getFloat("minor_road_width");
					}
					Polyline2D polyline = roads.orderPolyLine(*oei, *vi);
					QVector2D dir = polyline[1] - polyline[0];
					angle_offset = atan2f(dir.x(), -dir.y());
					break;
				}

				float z = rendManager.getTerrainHeight(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y());
				if (z < G::getFloat("road_min_level")) z = G::getFloat("road_min_level");
				QVector3D center(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y(), z + deltaZ);

				const float numSides = 10;
				QVector3D cc1 = QVector3D(rad * cosf(angle_offset), rad * sinf(angle_offset), 0.0f);
				for (int i = 1; i <= numSides; ++i) {
					float angle = angle_offset + M_PI * i / numSides;
					QVector3D cc2 = QVector3D(rad * cosf(angle), rad * sinf(angle), 0.0f);

					intersectCirclesV.push_back(Vertex(center, center/7.5f));
					intersectCirclesV.push_back(Vertex(center + cc1, (center + cc1) / 7.5f));
					intersectCirclesV.push_back(Vertex(center + cc2, (center + cc2) / 7.5f));

					// side face geometry
					QVector3D side0_u = center + cc1;
					QVector3D side0_b = center + cc1 + QVector3D(0, 0, -deltaZ * 2.0f);
					QVector3D side1_u = center + cc2;
					QVector3D side1_b = center + cc2 + QVector3D(0, 0, -deltaZ * 2.0f);

					vertSide.push_back(Vertex(side0_b, QColor(64, 64, 64), cc1, QVector3D()));
					vertSide.push_back(Vertex(side1_b, QColor(64, 64, 64), cc2, QVector3D()));
					vertSide.push_back(Vertex(side1_u, QColor(64, 64, 64), cc2, QVector3D()));
					vertSide.push_back(Vertex(side0_u, QColor(64, 64, 64), cc1, QVector3D()));

					cc1 = cc2;
				}
			} else {
				////////////////////////
				// 2.2 FOUR OR MORE--> COMPLEX INTERSECTION
				float z = rendManager.getTerrainHeight(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y());
				if (z < G::getFloat("road_min_level")) z = G::getFloat("road_min_level");
				z += deltaZ + 0.1f;

				////////////
				// 2.2.1 For each vertex find edges and sort them in clockwise order
				std::vector<std::pair<float, RoadEdgeDesc> > edgeAngleOut;
				RoadOutEdgeIter Oei, Oei_end;
				QMap<RoadEdgeDesc, bool> visited;
				//printf("a1\n");
				for (boost::tie(Oei, Oei_end) = boost::out_edges(*vi, roads.graph); Oei != Oei_end; ++Oei) {
					if (!roads.graph[*Oei]->valid) continue;
					if (visited[*Oei]) continue;

					// GEN 1/12/2015
					// to avoid some garbage in the boost graph
					RoadVertexDesc tgt = boost::target(*Oei, roads.graph);
					if (*vi == 0 && *vi == tgt) continue;

					Polyline2D polyline = roads.orderPolyLine(*Oei, *vi);
					QVector2D p0 = polyline[0];
					QVector2D p1 = polyline[1];

					QVector2D edgeDir=(p1-p0).normalized();// NOTE p1-p0

					float angle = atan2(edgeDir.y(),edgeDir.x());
					edgeAngleOut.push_back(std::make_pair(angle, *Oei));//z as width

					// For self-loop edge
					if (tgt == *vi) {
						p0 = polyline.back();
						p1 = polyline[polyline.size() - 2];
						edgeDir = (p1 - p0).normalized();
						float angle = atan2(edgeDir.y(),edgeDir.x());
						edgeAngleOut.push_back(std::make_pair(angle, *Oei));//z as width
					}

					visited[*Oei] = true;
				}
				std::sort(edgeAngleOut.begin(), edgeAngleOut.end(), compare2ndPartTuple2);

				// 2.2.2 Create intersection geometry of the given edges
				std::vector<QVector3D> interPoints;
				std::vector<QVector3D> stopPoints;
				float prev_angle = std::numeric_limits<float>::max();
				for (int eN = 0; eN < edgeAngleOut.size(); eN++) {
					// GEN
					// Some OSM files have redundant edges. To handle this situation,
					// skip the edge if its angle is the same as the previous one.
					// This is just a HACK, so it is recommended to clean up the roads.
					if (edgeAngleOut[eN].first == prev_angle) continue;
					prev_angle = edgeAngleOut[eN].first;

					//printf("** eN %d\n",eN);
					// a) ED1: this edge
					float ed1W;
					if (roads.graph[edgeAngleOut[eN].second]->type == RoadEdge::TYPE_AVENUE) {
						ed1W = roads.graph[edgeAngleOut[eN].second]->getWidth(G::getFloat("major_road_width") * 0.5f);
					}
					else {
						ed1W = roads.graph[edgeAngleOut[eN].second]->getWidth(G::getFloat("minor_road_width") * 0.5f);
					}
					Polyline2D ed1poly = roads.orderPolyLine(edgeAngleOut[eN].second, *vi);// , edgeAngleOut[eN].first);
					QVector2D ed1p1 = ed1poly[1];
					// compute right side
					QVector2D ed1Dir = (roads.graph[*vi]->pt - ed1p1).normalized();//ends in 0
					QVector2D ed1Per(ed1Dir.y(), -ed1Dir.x());
					QVector2D ed1p0R = roads.graph[*vi]->pt + ed1Per*ed1W/2.0f;
					QVector2D ed1p1R = ed1p1 + ed1Per*ed1W/2.0f;
					// compute left side
					QVector2D ed1p0L = roads.graph[*vi]->pt - ed1Per*ed1W/2.0f;
					QVector2D ed1p1L = ed1p1 - ed1Per*ed1W/2.0f;

					// b) ED2: next edge
					int lastEdge = eN - 1;
					if (lastEdge < 0) lastEdge = edgeAngleOut.size() - 1;
					float ed2WL;
					if (roads.graph[edgeAngleOut[lastEdge].second]->type == RoadEdge::TYPE_AVENUE) {
						ed2WL = roads.graph[edgeAngleOut[lastEdge].second]->getWidth(G::getFloat("major_road_width") * 0.5f);
					}
					else {
						ed2WL = roads.graph[edgeAngleOut[lastEdge].second]->getWidth(G::getFloat("minor_road_width") * 0.5f);
					}
					QVector2D ed2p0L = roads.graph[*vi]->pt;
					Polyline2D ed2polyL = roads.orderPolyLine(edgeAngleOut[lastEdge].second, *vi);// , edgeAngleOut[lastEdge].first);
					QVector2D ed2p1L = ed2polyL[1];
					// compute left side
					QVector2D ed2DirL = (ed2p0L - ed2p1L).normalized();//ends in 0
					QVector2D ed2PerL(ed2DirL.y(), -ed2DirL.x());
					ed2p0L -= ed2PerL*ed2WL/2.0f;
					ed2p1L -= ed2PerL*ed2WL/2.0f;

					// c) ED2: last edge
					int nextEdge = (eN + 1) % edgeAngleOut.size();
					float ed2WR;
					if (roads.graph[edgeAngleOut[nextEdge].second]->type == RoadEdge::TYPE_AVENUE) {
						ed2WR = roads.graph[edgeAngleOut[nextEdge].second]->getWidth(G::getFloat("major_road_width") * 0.5f);
					}
					else {
						ed2WR = roads.graph[edgeAngleOut[nextEdge].second]->getWidth(G::getFloat("minor_road_width") * 0.5f);
					}
					QVector2D ed2p0R = roads.graph[*vi]->pt;
					Polyline2D ed2polyR = roads.orderPolyLine(edgeAngleOut[nextEdge].second, *vi);// , edgeAngleOut[nextEdge].first);
					QVector2D ed2p1R = ed2polyR[1];
					// compute left side
					QVector2D ed2DirR = (ed2p0R - ed2p1R).normalized();//ends in 0
					QVector2D ed2PerR(ed2DirR.y(), -ed2DirR.x());
					ed2p0R += ed2PerR*ed2WR/2.0f;
					ed2p1R += ed2PerR*ed2WR/2.0f;

					//////////////////////////////////////////
					// d) Computer interior coordinates
					// d.1 computer intersection left
					QVector2D intPt1(FLT_MAX,0);
					if (fabs(QVector2D::dotProduct(ed1Dir, ed2DirL)) < 0.95f) { // adjacent road segments are not parallel
						double tab,tcd;
						Util::segmentSegmentIntersectXY(ed1p0R, ed1p1R, ed2p0L, ed2p1L, &tab, &tcd, false, intPt1);
					} else { // adjacent road segments are parallel
						intPt1 = (ed1p0R + ed2p0L) * 0.5f;
					}
					// d.2 computer intersecion right
					QVector2D intPt2(FLT_MAX,0);
					if (fabs(QVector2D::dotProduct(ed1Dir, ed2DirR)) < 0.95f) {
						double tab, tcd;
						Util::segmentSegmentIntersectXY(ed1p0L, ed1p1L, ed2p0R, ed2p1R, &tab, &tcd, false, intPt2);
					} else {
						intPt2 = (ed1p0L + ed2p0R) * 0.5f;
					}

					QVector3D intPoint1(intPt1.x(), intPt1.y(), z);
					QVector3D intPoint2(intPt2.x(), intPt2.y(), z);

					interPoints.push_back(intPoint1);
					// align intPoint1 and intPoint2 such that they are perpendicular to the road orientation
					if (QVector3D::dotProduct(intPoint1 - intPoint2, ed1Dir) >= 0) {
						intPoint1 -= ed1Dir * QVector3D::dotProduct(intPoint1 - intPoint2, ed1Dir);
						interPoints.push_back(intPoint1);
					} else {
						intPoint2 += ed1Dir * QVector3D::dotProduct(intPoint1 - intPoint2, ed1Dir);
						interPoints.push_back(intPoint2);
					}
					
					stopPoints.push_back(intPoint1);
					stopPoints.push_back(intPoint2);

					if (outDegree >= 3 && roads.graph[edgeAngleOut[eN].second]->type == RoadEdge::TYPE_AVENUE && (ed1poly[0] - ed1poly.back()).length() > 10.0f && ed1poly.length() > 50.0f) {
						// crosswalk
						interPedX.push_back(Vertex(intPoint1, QVector3D(0 - 0.07f, 0, 0)));
						interPedX.push_back(Vertex(intPoint2, QVector3D(ed1W / 7.5f + 0.07f, 0, 0)));
						interPedX.push_back(Vertex(intPoint2 - ed1Dir * 3.5f, QVector3D(ed1W / 7.5f + 0.07f, 1.0f, 0)));
						interPedX.push_back(Vertex(intPoint1 - ed1Dir * 3.5f, QVector3D(0.0f - 0.07f, 1.0f, 0)));

						// stop line
						QVector3D midPoint=(intPoint2+intPoint1)/2.0f+0.2f*ed1Per;
					
						interPedXLineR.push_back(Vertex(intPoint1 - ed1Dir * 3.5f, QVector3D(0, 0.0f, 0)));
						interPedXLineR.push_back(Vertex(midPoint - ed1Dir * 3.5f, QVector3D(1.0f, 0.0f, 0)));
						interPedXLineR.push_back(Vertex(midPoint - ed1Dir * 4.25f, QVector3D(1.0f, 1.0f, 0)));
						interPedXLineR.push_back(Vertex(intPoint1 - ed1Dir * 4.25f, QVector3D(0.0f, 1.0f, 0)));
					}
				}
								
				if (interPoints.size() > 2) {
					rendManager.addStaticGeometry2("3d_roads", interPoints,0.0f, "data/textures/roads/road_0lines.jpg", 2, QVector3D(1.0f / 7.5f, 1.0f / 7.5f, 1), QColor());
				}
			}
		}

		rendManager.addStaticGeometry("3d_roads", intersectCirclesV, "data/textures/roads/road_0lines.jpg", GL_TRIANGLES, 2);
		rendManager.addStaticGeometry("3d_roads", interPedX, "data/textures/roads/road_pedX.jpg", GL_QUADS, 2);
		rendManager.addStaticGeometry("3d_roads", interPedXLineR, "data/textures/roads/road_pedXLineR.jpg", GL_QUADS, 2);
	}

	rendManager.addStaticGeometry("3d_roads", vertSide, "", GL_QUADS, 1|mode_Lighting);
}

std::vector<QVector3D> RoadMeshGenerator::generateCurvePoints(const QVector3D& intPoint, const QVector3D& p1, const QVector3D& p2) {
	std::vector<QVector3D> points;

	// if they are co-linear, return p1-intPoint-p2
	if (fabs(QVector3D::dotProduct((p1 - intPoint).normalized(), (p2 - intPoint).normalized())) > 0.95f) {
		points.push_back(p1);
		points.push_back(intPoint);
		points.push_back(p2);
		return points;
	}

	QVector3D d1 = intPoint - p1;
	QVector3D per1 = QVector3D(-d1.y(), d1.x(), 0);
	QVector3D d2 = p2 - intPoint;
	QVector3D per2 = QVector3D(-d2.y(), d2.x(), 0);

	double tab, tcd;
	QVector3D center;
	if (!Util::segmentSegmentIntersectXY3D(p1, p1 + per1 * 100, p2, p2 + per2 * 100, &tab, &tcd, false, center)) return points;

	QVector2D v1(p1 - center);
	QVector2D v2(p2 - center);
	float r = v1.length();
	float theta1 = atan2f(v1.y(), v1.x());
	float theta2 = atan2f(v2.y(), v2.x());
	if (theta2 < theta1) theta2 += 2 * M_PI;

	points.push_back(p1);
	for (float theta = theta1 + 0.2f; theta < theta2; theta += 0.2f) {
		points.push_back(center + QVector3D(cosf(theta) * r, sinf(theta) * r, 0));
	}
	points.push_back(p2);

	return points;
}

void RoadMeshGenerator::generate2DRoadMesh(VBORenderManager& renderManager, RoadGraph& roads) {
	float deltaZ = 2.0f;

	//////////////////////////////////////
	// EDGES
	{
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			int num = roads.graph[*ei]->polyline.size();
			if (num <= 1) continue;

			float halfWidth;
			if (roads.graph[*ei]->type == RoadEdge::TYPE_AVENUE) {
				halfWidth = roads.graph[*ei]->lanes * G::getFloat("major_road_width");
			}
			else {
				halfWidth = roads.graph[*ei]->lanes * G::getFloat("minor_road_width");
			}
			
			std::vector<Vertex> vert(4 * (num - 1));
			std::vector<Vertex> vertBg(4 * (num - 1));
			
			// Type
			QColor color;
			QColor colorBg;
			float heightOffset = 0.0f;
			float heightOffsetBg = 0.0f;

			switch (roads.graph[*ei]->type) {
			case RoadEdge::TYPE_AVENUE:
				heightOffset = 0.6f;
				heightOffsetBg = 0.1f;
				color = QColor(0xff,0xe1,0x68);
				colorBg = QColor(0x00, 0x00, 0x00);//QColor(0xe5,0xbd,0x4d);
				break;
			default:
				heightOffset = 0.4f;
				heightOffsetBg = 0.1f;
				color = QColor(0xff,0xff,0xff);
				colorBg = QColor(0x00, 0x00, 0x00);//QColor(0xd7,0xd1,0xc7);
				break;
			}

			heightOffset += 0.45f;//to have park below
			heightOffsetBg += 0.45f;//to have park below

			float halfWidthBg = halfWidth + G::global().getFloat("2DroadsStroke") * 0.5;
			halfWidth -= G::global().getFloat("2DroadsStroke") * 0.5;

			QVector2D p0, p1, p2, p3;
			QVector2D p0Bg, p1Bg, p2Bg, p3Bg;
			for (int i = 0; i < num - 1; ++i) {
				QVector2D pt1 = roads.graph[*ei]->polyline[i];
				QVector2D pt2 = roads.graph[*ei]->polyline[i + 1];

				QVector2D perp = pt2 - pt1;
				perp = QVector2D(-perp.y(), perp.x());
				perp.normalize();

				if (i == 0) {
					p0 = pt1 + perp * halfWidth;
					p1 = pt1 - perp * halfWidth;
					p0Bg = pt1 + perp * halfWidthBg;
					p1Bg = pt1 - perp * halfWidthBg;
				}
				p2 = pt2 - perp * halfWidth;
				p3 = pt2 + perp * halfWidth;
				p2Bg = pt2 - perp * halfWidthBg;
				p3Bg = pt2 + perp * halfWidthBg;
				QVector3D normal(0, 0, 1);

				if (i < num - 2) {
					QVector2D pt3 = roads.graph[*ei]->polyline[i + 2];

					Util::getIrregularBisector(pt1, pt2, pt3, halfWidth, halfWidth, p3);
					Util::getIrregularBisector(pt1, pt2, pt3, -halfWidth, -halfWidth, p2);
					Util::getIrregularBisector(pt1, pt2, pt3, halfWidthBg, halfWidthBg, p3Bg);
					Util::getIrregularBisector(pt1, pt2, pt3, -halfWidthBg, -halfWidthBg, p2Bg);
				}

				vert[i * 4 + 0] = Vertex(p0.x(), p0.y(), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vert[i * 4 + 1] = Vertex(p1.x(), p1.y(), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vert[i * 4 + 2] = Vertex(p2.x(), p2.y(), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vert[i * 4 + 3] = Vertex(p3.x(), p3.y(), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
					
				vertBg[i * 4 + 0] = Vertex(p0Bg.x(), p0Bg.y(), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vertBg[i * 4 + 1] = Vertex(p1Bg.x(), p1Bg.y(), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vertBg[i * 4 + 2] = Vertex(p2Bg.x(), p2Bg.y(), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				vertBg[i * 4 + 3] = Vertex(p3Bg.x(), p3Bg.y(), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);// pos color normal texture
				
				p0 = p3;
				p1 = p2;
				p0Bg = p3Bg;
				p1Bg = p2Bg;
			}

			renderManager.addStaticGeometry("3d_roads", vert, "", GL_QUADS, 1);//MODE=1 color
			renderManager.addStaticGeometry("3d_roads", vertBg, "", GL_QUADS, 1);//MODE=1 color
		}
	}

	/////////////////////////////////////////////////////
	// INTERSECTIONS
	{
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			if (!roads.graph[*vi]->valid) continue;
			if (roads.getDegree(*vi) == 0) continue;

			// get the largest width of the outing edges
			QColor color;
			QColor colorBg;
			float heightOffset = 0.0f;
			float heightOffsetBg = 0.0f;
			int maxType = -1;
			float halfWidth;

			RoadOutEdgeIter oei, oeend;
			for (boost::tie(oei, oeend) = boost::out_edges(*vi, roads.graph); oei != oeend; ++oei) {
				if (!roads.graph[*oei]->valid) continue;
				
				maxType = roads.graph[*oei]->type;
				if (roads.graph[*oei]->type == RoadEdge::TYPE_AVENUE) {
					halfWidth = roads.graph[*oei]->lanes * G::getFloat("major_road_width");
				}
				else {
					halfWidth = roads.graph[*oei]->lanes * G::getFloat("minor_road_width");
				}

				switch (roads.graph[*oei]->type) {
				case RoadEdge::TYPE_AVENUE:
					heightOffset = 0.5f;
					heightOffsetBg = 0.2f;
					color = QColor(0xff,0xe1,0x68);
					colorBg = QColor(0x00, 0x00, 0x00);//QColor(0xe5,0xbd,0x4d);
					continue;
				default:
					heightOffset = 0.4f;
					heightOffsetBg = 0.2f;
					color = QColor(0xff,0xff,0xff);
					colorBg = QColor(0x00, 0x00, 0x00);//QColor(0xd7,0xd1,0xc7);
					continue;
				}
			}

			heightOffset += 0.45f;//to have park below
			heightOffsetBg += 0.45f;//to have park below

			float rad = halfWidth - G::global().getFloat("2DroadsStroke") * 0.5f;
			float rad_bg = halfWidth + G::global().getFloat("2DroadsStroke") * 0.5f;

			std::vector<Vertex> vert(3 * 20);
			std::vector<Vertex> vertBg(3 * 20);

			for (int i = 0; i < 20; ++i) {
				float angle1 = 2.0 * M_PI * i / 20.0f;
				float angle2 = 2.0 * M_PI * (i + 1) / 20.0f;

				vert[i * 3 + 0] = Vertex(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y(), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);
				vert[i * 3 + 1] = Vertex(roads.graph[*vi]->pt.x() + rad * cosf(angle1), roads.graph[*vi]->pt.y() + rad * sinf(angle1), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);
				vert[i * 3 + 2] = Vertex(roads.graph[*vi]->pt.x() + rad * cosf(angle2), roads.graph[*vi]->pt.y() + rad * sinf(angle2), deltaZ + heightOffset, color, 0, 0, 1.0f, 0, 0, 0);

				vertBg[i * 3 + 0] = Vertex(roads.graph[*vi]->pt.x(), roads.graph[*vi]->pt.y(), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);
				vertBg[i * 3 + 1] = Vertex(roads.graph[*vi]->pt.x() + rad_bg * cosf(angle1), roads.graph[*vi]->pt.y() + rad_bg * sinf(angle1), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);
				vertBg[i * 3 + 2] = Vertex(roads.graph[*vi]->pt.x() + rad_bg * cosf(angle2), roads.graph[*vi]->pt.y() + rad_bg * sinf(angle2), deltaZ + heightOffsetBg, colorBg, 0, 0, 1.0f, 0, 0, 0);
			}
						
			renderManager.addStaticGeometry("3d_roads", vert, "", GL_TRIANGLES, 1);
			renderManager.addStaticGeometry("3d_roads", vertBg, "", GL_TRIANGLES, 1);
		}
	}
}