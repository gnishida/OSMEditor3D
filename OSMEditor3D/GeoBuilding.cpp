#include "VBORenderManager.h"
#include "GeoBuilding.h"
#include "Util.h"
#include "qdir.h"
const float storyHeight=3.0f;

using namespace boost::polygon::operators;
bool GeoBuilding::bldgInitialized = false;

static std::vector<QString> facadeTex;
static std::vector<QVector3D> facadeScale;
static std::vector<QString> windowTex;
static std::vector<QString> roofTex;


void addExtrudedGeom(VBORenderManager& rendManager, const QString& name, const Loop3D& polygon, const QColor& color, float z, float height) {
	std::vector<Vertex> vert(polygon.size() * 4);

	for (int i = 0; i < polygon.size(); ++i) {
		int next = (i + 1) % polygon.size();
		QVector3D a(polygon[next].x() - polygon[i].x(), polygon[next].y() - polygon[i].y(), 0);
		QVector3D b(0, 0, 1);
		QVector3D n = QVector3D::crossProduct(a, b).normalized();

		vert[i * 4 + 0] = Vertex(polygon[i].x(), polygon[i].y(), z, color, n.x(), n.y(), n.z(), 0, 0, 0);
		vert[i * 4 + 1] = Vertex(polygon[next].x(), polygon[next].y(), z, color, n.x(), n.y(), n.z(), 0, 0, 0);
		vert[i * 4 + 2] = Vertex(polygon[next].x(), polygon[next].y(), z + height, color, n.x(), n.y(), n.z(), 0, 0, 0);
		vert[i * 4 + 3] = Vertex(polygon[i].x(), polygon[i].y(), z + height, color, n.x(), n.y(), n.z(), 0, 0, 0);
	}
	rendManager.addStaticGeometry(name, vert, "", GL_QUADS, 1 | mode_Lighting);
}

void addTexExtrudedGeom(VBORenderManager& rendManager, const QString& name, const Loop3D& polygon, const QColor& color, float z, float height) {
	int randomFacade = qrand() % facadeTex.size();
	float uS = facadeScale[randomFacade].x();
	float vS = facadeScale[randomFacade].y();

	std::vector<Vertex> vert(polygon.size() * 4);

	for (int i = 0; i < polygon.size(); ++i) {
		int next = (i + 1) % polygon.size();
		QVector3D a(polygon[next].x() - polygon[i].x(), polygon[next].y() - polygon[i].y(), 0);
		QVector3D b(0, 0, 1);
		QVector3D n = QVector3D::crossProduct(a, b).normalized();

		QVector3D t1(0, 0, 0);
		QVector3D t2(a.length() * uS, 0, 0);
		QVector3D t3(t2.x(), height * vS, 0);
		QVector3D t4(0, t3.y(), 0);

		vert[i * 4 + 0] = Vertex(polygon[i].x(), polygon[i].y(), z, color, n.x(), n.y(), n.z(), t1.x(), t1.y(), t1.z());
		vert[i * 4 + 1] = Vertex(polygon[next].x(), polygon[next].y(), z, color, n.x(), n.y(), n.z(), t2.x(), t2.y(), t2.z());
		vert[i * 4 + 2] = Vertex(polygon[next].x(), polygon[next].y(), z + height, color, n.x(), n.y(), n.z(), t3.x(), t3.y(), t3.z());
		vert[i * 4 + 3] = Vertex(polygon[i].x(), polygon[i].y(), z + height, color, n.x(), n.y(), n.z(), t4.x(), t4.y(), t4.z());
	}
	rendManager.addStaticGeometry(name, vert, facadeTex[randomFacade], GL_QUADS, 2 | mode_Lighting);
}

void addConvexPoly(VBORenderManager& rendManager, QString geoName, const Loop3D& polygon, const QColor& color, float z, bool flipped = false) {
	if (polygon.size() < 3) return;

	std::vector<VBORenderManager::polygonP> polySet;
	VBORenderManager::polygonP tempPolyP;

	std::vector<VBORenderManager::pointP> vP(polygon.size());
	for (int i = 0; i < polygon.size(); ++i) {
		vP[i] = boost::polygon::construct<VBORenderManager::pointP>(polygon[i].x(), polygon[i].y());
	}

	// close the polygon
	if (polygon.back().x() != polygon.front().x() && polygon.back().y() != polygon.front().y()) {
		vP.push_back(vP[0]);
	}

	boost::polygon::set_points(tempPolyP, vP.begin(), vP.end());
	polySet += tempPolyP;
	std::vector<VBORenderManager::polygonP> allP;
	boost::polygon::get_trapezoids(allP, polySet);
	std::vector<Vertex> vert;

	QVector3D norm(0, 0, 1);
	if (flipped) {
		norm = QVector3D(0, 0, -1);
	}

	for (int i = 0; i < allP.size(); ++i) {
		Loop3D points;
		std::vector<QVector3D> texP;
		for (auto it = allP[i].begin(); it != allP[i].end(); ++it) {
			VBORenderManager::pointP cP = *it;
			points.push_back(QVector2D(cP.x(), cP.y()));
		}

		if (points.size() >= 3) {
			if ((!flipped && points.isClockwise()) || (flipped && !points.isClockwise())) {
				std::reverse(points.begin(), points.end());
			}
			for (int k = 1; k < points.size() - 1; ++k) {
				vert.push_back(Vertex(QVector3D(points[0].x(), points[0].y(), z), color, norm, QVector3D(0, 0, 0)));
				vert.push_back(Vertex(QVector3D(points[k].x(), points[k].y(), z), color, norm, QVector3D(0, 0, 0)));
				vert.push_back(Vertex(QVector3D(points[k + 1].x(), points[k + 1].y(), z), color, norm, QVector3D(0, 0, 0)));
			}
		}
	}

	rendManager.addStaticGeometry(geoName, vert, "", GL_TRIANGLES, 1 | mode_Lighting);
}

void addTexConvexPoly(VBORenderManager& rendManager, const QString& geoName, const QString& textureName, int shaderMode, const Loop3D& pos, const QColor& col, float zShift, const QVector3D& texScale, bool flipped = false) {
	if (pos.size() < 3) return;

	std::vector<VBORenderManager::polygonP> polySet;
	VBORenderManager::polygonP tempPolyP;

	// determin the orientation of the texture
	QVector3D size;
	QMatrix4x4 xformMat;
	Polygon3D::getLoopOBB2(pos, size, xformMat);
	Loop3D xformPos;
	Polygon3D::transformLoop(pos, xformPos, xformMat);

	float minX=FLT_MAX,minY=FLT_MAX;
	float maxX=-FLT_MAX,maxY=-FLT_MAX;

	std::vector<VBORenderManager::pointP> vP(pos.size());
	for (int i = 0; i < xformPos.size(); i++) {
		vP[i] = boost::polygon::construct<VBORenderManager::pointP>(pos[i].x(), pos[i].y());
		minX = std::min<float>(minX, xformPos[i].x());
		minY = std::min<float>(minY, xformPos[i].y());
		maxX = std::max<float>(maxX, xformPos[i].x());
		maxY = std::max<float>(maxY, xformPos[i].y());
	}

	QVector3D norm(0, 0, 1);
	if (flipped) {
		norm = QVector3D(0, 0, -1);
	}

	boost::polygon::set_points(tempPolyP, vP.begin(), vP.end());
	polySet += tempPolyP;
	std::vector<VBORenderManager::polygonP> allP;
	boost::polygon::get_trapezoids(allP,polySet);
	std::vector<Vertex> vert;
	for (int i = 0; i < allP.size(); ++i) {
		Loop3D points;
		std::vector<QVector3D> texP;
		for (auto it = allP[i].begin(); it != allP[i].end(); ++it) {
			VBORenderManager::pointP cP=*it;
			points.push_back(QVector3D(cP.x(), cP.y(), pos[0].z() + zShift));

			if (texScale.x() == 0 && texScale.y() == 0) {
				QVector3D cP2 = xformMat * QVector3D(cP.x(), cP.y(), 0);
				texP.push_back(QVector3D((cP2.x() - minX) / size.x(), (cP2.y() - minY) / size.y(), 0.0f));
			} else {
				QVector3D cP2 = xformMat * QVector3D(cP.x(), cP.y(), 0);
				texP.push_back(QVector3D((cP2.x() - minX) * texScale.x(), (cP2.y() - minY) * texScale.y(), 0.0f));
			}
		}

		if ((!flipped && points.isClockwise()) || (flipped && !points.isClockwise())) {
			std::reverse(points.begin(), points.end());
			std::reverse(texP.begin(), texP.end());
		}

		for (int k = 1; k < points.size() - 1; ++k) {
			vert.push_back(Vertex(points[0], col, norm, texP[0]));
			vert.push_back(Vertex(points[k], col, norm, texP[k]));
			vert.push_back(Vertex(points[k + 1], col, norm, texP[k + 1]));
		}
	}

	rendManager.addStaticGeometry(geoName, vert, textureName, GL_TRIANGLES, shaderMode);
}

void addBox(VBORenderManager& rendManager, const Loop3D& roofOffCont, const QColor& color, float z, float height) {
	addConvexPoly(rendManager, "3d_building", roofOffCont, color, z + height);

	addConvexPoly(rendManager, "3d_building", roofOffCont, color, z, true);

	addExtrudedGeom(rendManager, "3d_building", roofOffCont, color, z, height);
}

void addRoof(VBORenderManager& rendManager, const Loop3D& roofOffCont, const QColor& roofColor, float initHeight, float roofHeight) {
	addTexConvexPoly(rendManager, "3d_building", roofTex[qrand() % roofTex.size()], 2 | mode_Lighting, roofOffCont, roofColor, initHeight + roofHeight, QVector3D(0, 0, 0));

	Loop3D reversed_contour = roofOffCont;
	std::reverse(reversed_contour.begin(), reversed_contour.end());
	addConvexPoly(rendManager, "3d_building", reversed_contour, roofColor, initHeight, true);

	addExtrudedGeom(rendManager, "3d_building", roofOffCont, roofColor, initHeight, roofHeight);
}

void calculateColumnContour(const std::vector<QVector3D>& currentContour, std::vector<QVector3D>& columnContour, std::vector<int>& windows) {
	columnContour.clear();
	windows.clear();

	for (int i = 0; i < currentContour.size(); ++i) {
		int next = (i + 1) % currentContour.size();
		QVector3D pos1 = currentContour[i];
		QVector3D pos2 = currentContour[next];
		QVector3D dirV = pos2 - pos1;
		float len = dirV.length();
		dirV /= len;
		if (len > 7.0f) {
			QVector3D perDir = QVector3D::crossProduct(dirV, QVector3D(0, 0, 1.0f));

			int numWindows = (len + 0.5f + 5.25f) / 10.5f;
			float windowWidth = ((len + 0.5f) / numWindows - 1.5f) / 3.0f;

			columnContour.push_back(pos1);
			windows.push_back(0);

			pos1 = pos1 + 0.5f * dirV;
			for (int j = 0; j < numWindows; ++j) {
				columnContour.push_back(pos1);
				windows.push_back(0);
				columnContour.push_back(pos1 - 0.2f * perDir);
				windows.push_back(2);
				columnContour.push_back(pos1 + windowWidth * dirV - 0.2f * perDir);
				windows.push_back(1);
				columnContour.push_back(pos1 + windowWidth * 2.0f * dirV - 0.2f * perDir);
				windows.push_back(1);
				columnContour.push_back(pos1 + windowWidth * 3.0f * dirV - 0.2f * perDir);
				windows.push_back(0);
				columnContour.push_back(pos1 + windowWidth * 3.0f * dirV);
				windows.push_back(0);

				pos1 = pos1 + (1.5f + windowWidth * 3.0f) * dirV;
			}
		}
		else if (len > 6.0f) {
			columnContour.push_back(pos1);
			windows.push_back(1);
			columnContour.push_back((pos1 + pos2) * 0.5);
			windows.push_back(1);
		}
		else if (len > 3.0f) {
			columnContour.push_back(pos1);
			windows.push_back(1);
		}
		else {
			columnContour.push_back(pos1);
			windows.push_back(0);
		}
	}
}

void addWindow(VBORenderManager& rendManager, const QVector3D& randN, const QVector3D& initPoint, const QVector3D& dirR, const QVector3D& dirUp, float width, float height, const QString& texture, float uS, float vS) {
	QColor color;
	int randCol = ((int)randN.z()) % 5;

	switch (randCol) {
	case 0:
		color=QColor(0.3f,0.3f,0.3f);
		break;
	case 1:
		color=QColor(0.345, 0.171, 0.075);//brown
		break;
	case 2:
		color=QColor(0.412, 0.412, 0.412);//grey
		break;
	case 3:
		color=QColor(0.02, 0.02, 0.13);//blue
		break;
	case 4:
		color=QColor(0.961, 0.961, 0.863);//beige
		break;
	}

	// WINDOW
	std::vector<Vertex> vertWind;

	float depth = 0.5f;// 2.0f;

	// IN: TOP
	QVector3D perI = QVector3D::crossProduct(dirUp,dirR); //note direction: to inside
	QVector3D vert[8];
	vert[0] = initPoint;
	vert[1] = initPoint + perI * depth;
	vert[2] = initPoint + perI * depth + dirUp * height;
	vert[3] = initPoint + dirUp * height;

	vert[4] = initPoint + perI * depth + dirR * width;
	vert[5] = initPoint + dirR * width;
	vert[6] = initPoint + dirUp * height + dirR * width;
	vert[7] = initPoint + perI * depth + dirUp * height + dirR * width;

	// LEFT
	QVector3D norm = QVector3D::crossProduct(vert[1] - vert[0], vert[3] - vert[0]);
	vertWind.push_back(Vertex(vert[0], color, norm, QVector3D(0, 0, 0)));
	vertWind.push_back(Vertex(vert[1], color, norm, QVector3D(depth * uS, 0, 0)));
	vertWind.push_back(Vertex(vert[2], color, norm, QVector3D(depth * uS, height * vS, 0)));
	vertWind.push_back(Vertex(vert[3], color, norm, QVector3D(0, height * vS, 0)));

	// RIGHT
	norm = QVector3D::crossProduct(vert[5] - vert[4], vert[7] - vert[4]);
	vertWind.push_back(Vertex(vert[4], color, norm, QVector3D(0, 0, 0)));
	vertWind.push_back(Vertex(vert[5], color, norm, QVector3D(depth * uS, 0, 0)));
	vertWind.push_back(Vertex(vert[6], color, norm, QVector3D(depth * uS, height * vS, 0)));
	vertWind.push_back(Vertex(vert[7], color, norm, QVector3D(0, height * vS, 0)));

	// TOP
	norm = QVector3D::crossProduct(vert[7] - vert[2], vert[3] - vert[2]);
	vertWind.push_back(Vertex(vert[2], color, norm, QVector3D(0, 0, 0)));
	vertWind.push_back(Vertex(vert[7], color, norm, QVector3D(width * uS, 0, 0)));
	vertWind.push_back(Vertex(vert[6], color, norm, QVector3D(width * uS, depth * vS, 0)));
	vertWind.push_back(Vertex(vert[3], color, norm, QVector3D(0, depth * vS, 0)));

	// BOT
	norm = QVector3D::crossProduct(vert[5] - vert[0], vert[1] - vert[0]);
	vertWind.push_back(Vertex(vert[0], color, norm, QVector3D(0, 0, 0)));
	vertWind.push_back(Vertex(vert[5], color, norm, QVector3D(width * uS, 0, 0)));
	vertWind.push_back(Vertex(vert[4], color, norm, QVector3D(width * uS, depth * vS, 0)));
	vertWind.push_back(Vertex(vert[1], color, norm, QVector3D(0, depth * vS, 0)));

	rendManager.addStaticGeometry("3d_building", vertWind, texture, GL_QUADS, 2 | mode_Lighting);

	//////////////////////////////////////////////////////
	// BACK
	vertWind.clear();
	norm = QVector3D::crossProduct(vert[4] - vert[1], vert[2] - vert[1]);
	vertWind.push_back(Vertex(vert[1], color, norm, QVector3D(0, 0, 0)));
	vertWind.push_back(Vertex(vert[4], color, norm, QVector3D(1, 0, 0)));
	vertWind.push_back(Vertex(vert[7], color, norm, QVector3D(1, 1, 0)));
	vertWind.push_back(Vertex(vert[2], color, norm, QVector3D(0, 1, 0)));

	rendManager.addStaticGeometry("3d_building", vertWind, windowTex[((int)randN.x()) % windowTex.size()], GL_QUADS, 2 | mode_Lighting);
}

void addFirstFloor(VBORenderManager& rendManager, const std::vector<QVector3D>& columnContour, const std::vector<int>& windows, int randomFacade, const QVector3D& randN, float uS, float vS, float height, float floorHeight, const QVector3D& windowRandomSize) {
	std::vector<Vertex> vert;

	float verticalHoleSize = windowRandomSize.x();
	float horHoleSize = windowRandomSize.y();
	float accPerimeter = 0;

	for (int sN = 0; sN < columnContour.size(); ++sN) {
		int ind1 = sN;
		int ind2 = (sN + 1) % columnContour.size();
		QVector3D dir = columnContour[ind2] - columnContour[ind1];
		float sideLength = dir.length();
		dir /= sideLength;
		QVector3D norm = QVector3D::crossProduct(dir, QVector3D(0, 0, 1.0f));

		if (windows[sN] == 0) {
			float heightB = height;
			float heightT = floorHeight + height;

			vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, heightB), QColor(), norm, QVector3D(accPerimeter * uS, heightB * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, heightB), QColor(), norm, QVector3D((accPerimeter + sideLength) * uS, heightB * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, heightT), QColor(), norm, QVector3D((accPerimeter + sideLength) * uS, heightT * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, heightT), QColor(), norm, QVector3D(accPerimeter * uS, heightT * vS, 0.0f)));
		}
		else {
			float h0 = height;
			float h3 = floorHeight + height;
			float h1 = h0 + verticalHoleSize;
			float h2 = h3 - verticalHoleSize;

			if (windows[sN] == 1) {
				// BOT
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h0), QColor(), norm, QVector3D(accPerimeter*uS, h0*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h0), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h0*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter)*uS, h1*vS, 0.0f)));

				// TOP
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D(accPerimeter*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h3*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter)*uS, h3*vS, 0.0f)));

				// LEFT
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1), QColor(), norm, QVector3D(accPerimeter*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter)*uS, h2*vS, 0.0f)));

				// RIGHT
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h2*vS, 0.0f)));

				////////// INSIDE
				addWindow(rendManager, randN, columnContour[ind1] + QVector3D(0, 0, h1) + dir*horHoleSize, dir, QVector3D(0, 0, 1.0f), sideLength - 2 * horHoleSize, h2 - h1, facadeTex[randomFacade], uS, vS);
			}
			else {
				// TOP
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D(accPerimeter*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h3*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter)*uS, h3*vS, 0.0f)));

				// LEFT
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h0), QColor(), norm, QVector3D(accPerimeter*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h0) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter)*uS, h2*vS, 0.0f)));

				// RIGHT
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h0) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h0), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h2*vS, 0.0f)));

				////////// INSIDE
				addWindow(rendManager, randN, columnContour[ind1] + QVector3D(0, 0, h0) + dir*horHoleSize, dir, QVector3D(0, 0, 1.0f), sideLength - 2 * horHoleSize, h2 - h0, facadeTex[randomFacade], uS, vS);
			}
		}
		accPerimeter += sideLength;
	}

	rendManager.addStaticGeometry("3d_building", vert, facadeTex[randomFacade], GL_QUADS, 2 | mode_Lighting);
}

void addColumnGeometry(VBORenderManager& rendManager, const std::vector<QVector3D>& columnContour, const std::vector<int>& windows, int randomFacade, const QVector3D& randN, float uS, float vS, float height, int numFloors, float floorHeight, const QVector3D& windowRandomSize) {
	std::vector<Vertex> vert;

	float verticalHoleSize = windowRandomSize.x();
	float horHoleSize = windowRandomSize.y();
	float accPerimeter=0;

	for (int sN = 0; sN < columnContour.size(); ++sN) {
		int ind1 = sN;
		int ind2 = (sN + 1) %columnContour.size();
		QVector3D dir = columnContour[ind2] - columnContour[ind1];
		float sideLength = dir.length();
		dir /= sideLength;
		QVector3D norm = QVector3D::crossProduct(columnContour[ind2] - columnContour[ind1], QVector3D(0, 0, 1.0f));

		if (windows[sN] == 0) {
			float heightB = height;
			float heightT = numFloors * floorHeight + height;

			vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, heightB), QColor(), norm, QVector3D(accPerimeter * uS, heightB * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, heightB), QColor(), norm, QVector3D((accPerimeter + sideLength) * uS, heightB * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, heightT), QColor(), norm, QVector3D((accPerimeter + sideLength) * uS, heightT * vS, 0.0f)));
			vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, heightT), QColor(), norm, QVector3D(accPerimeter * uS, heightT * vS, 0.0f)));
		} else {
			for (int numF = 0; numF < numFloors; ++numF) {
				float h0 = numF * floorHeight + height;
				float h3 = (numF + 1) * floorHeight + height;
				float h1 = h0 + verticalHoleSize;
				float h2 = h3 - verticalHoleSize;
					
				// BOT
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h0), QColor(), norm, QVector3D(accPerimeter*uS, h0*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h0), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h0*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter)*uS, h1*vS, 0.0f)));

				// TOP
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D(accPerimeter*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h3*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h3), QColor(), norm, QVector3D((accPerimeter)*uS, h3*vS, 0.0f)));

				// LEFT
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1), QColor(), norm, QVector3D(accPerimeter*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h1) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2) + dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + horHoleSize)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind1] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter)*uS, h2*vS, 0.0f)));

				// RIGHT
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h1), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h1*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2), QColor(), norm, QVector3D((accPerimeter + sideLength)*uS, h2*vS, 0.0f)));
				vert.push_back(Vertex(columnContour[ind2] + QVector3D(0, 0, h2) - dir*horHoleSize, QColor(), norm, QVector3D((accPerimeter + sideLength - horHoleSize)*uS, h2*vS, 0.0f)));

				////////// INSIDE
				addWindow(rendManager, randN, columnContour[ind1] + QVector3D(0, 0, h1) + dir*horHoleSize, dir, QVector3D(0, 0, 1.0f), sideLength - 2 * horHoleSize, h2 - h1, facadeTex[randomFacade], uS, vS);
			}
		}
		accPerimeter+=sideLength;
	}
		
	rendManager.addStaticGeometry("3d_building", vert, facadeTex[randomFacade], GL_QUADS, 2 | mode_Lighting);
}

void GeoBuilding::initBuildingsTex() {
	QString pathName = "data/textures/LC/";
	QStringList nameFilter;
	nameFilter << "*.png" << "*.jpg" << "*.gif";

	// 1. facade
	QDir directory(pathName + "facade/");
	QStringList list = directory.entryList(nameFilter, QDir::Files);
	for (int lE = 0; lE < list.size(); ++lE) {
		facadeTex.push_back(pathName + "/facade/" + list[lE]);
		QStringList scaleS = list[lE].split("_");
		if (scaleS.size() != 4)
			facadeScale.push_back(QVector3D(1.0f,1.0f,0));
		else {
			facadeScale.push_back(QVector3D(scaleS[1].toFloat(),scaleS[2].toFloat(),0));
		}
	}

	// 2. windows
	QDir directoryW(pathName + "wind/");
	list = directoryW.entryList(nameFilter, QDir::Files);
	for(int lE = 0; lE < list.size(); ++lE) {
		windowTex.push_back(pathName + "wind/" + list[lE]);
	}

	// 3. roof
	QDir directoryR(pathName + "roof/");
	list = directoryR.entryList(nameFilter, QDir::Files);
	for(int lE = 0; lE < list.size(); ++lE) {
		roofTex.push_back(pathName + "roof/" + list[lE]);
	}

	bldgInitialized = true;
}

void GeoBuilding::generateBuilding(VBORenderManager& rendManager, Building& building) {
	if (!bldgInitialized) {
		initBuildingsTex();
	}

	// order the polygon in counter clockwise
	if (building.buildingFootprint.isClockwise()) {
		std::reverse(building.buildingFootprint.contour.begin(), building.buildingFootprint.contour.end());
	}

	// obtain the minimum elevation of the footprint
	float min_elev = std::numeric_limits<float>::max();
	float max_elev = -std::numeric_limits<float>::max();

	for (int k = 0; k < building.buildingFootprint.size(); ++k) {
		float z = rendManager.getTerrainHeight(building.buildingFootprint[k].x(), building.buildingFootprint[k].y());
		min_elev = std::min(min_elev, z);
		max_elev = std::max(max_elev, z);
	}

	///////////////////////////
	// Simple box
	if (building.bldType == 0) {
		//addExtrudedGeom(rendManager, "3d_building", building.buildingFootprint.contour, building.color, z, building.numStories * storyHeight);
		addTexExtrudedGeom(rendManager, "3d_building", building.buildingFootprint.contour, building.color, min_elev, building.numStories * storyHeight);
		//addConvexPoly(rendManager, "3d_building", building.buildingFootprint.contour, building.color, z + building.numStories * storyHeight);
		addTexConvexPoly(rendManager, "3d_building", roofTex[qrand() % roofTex.size()], 2 | mode_Lighting, building.buildingFootprint.contour, building.color, min_elev + building.numStories * storyHeight, QVector3D(0, 0, 0));
	}

	if (building.bldType == 1) {
		float baseHeight = max_elev - min_elev + 2.8f;
		float firstFloorHeight = 4.0f;
		float buildingHeight = baseHeight + firstFloorHeight + (building.numStories - 1) * storyHeight;

		Loop3D roofOffCont;
		building.buildingFootprint.offsetOutside(0.1, roofOffCont);

		////////////////////////////
		// FLOORS
		int randC = Util::genRand(0, 255 * 0.8);
		QColor bldgColor(randC, randC, randC);
		
		///////////////////////////
		// Add Base
		addExtrudedGeom(rendManager, "3d_building", building.buildingFootprint.contour, bldgColor, min_elev, baseHeight);
		addConvexPoly(rendManager, "3d_building", building.buildingFootprint.contour, bldgColor, min_elev + baseHeight);

		/// Add columns
		std::vector<QVector3D> columnContour;
		std::vector<int> windows;
		calculateColumnContour(building.buildingFootprint.contour, columnContour, windows);

		// add geometry
		int randomFacade = qrand() % facadeTex.size();
		float uS = facadeScale[randomFacade].x();
		float vS = facadeScale[randomFacade].y();

		QVector3D windowRandSize((float)qrand()/RAND_MAX * 0.5 + 0.3, (float)qrand()/RAND_MAX * 0.5 + 0.3, (float)qrand()/RAND_MAX);
		QVector3D randN(qrand(), qrand(), qrand());

		// First floor
		addFirstFloor(rendManager, columnContour, windows, randomFacade, randN, uS, vS, min_elev + baseHeight, firstFloorHeight, windowRandSize);

		// Upper floors
		addColumnGeometry(rendManager, columnContour, windows, randomFacade, randN, uS, vS, min_elev + baseHeight + firstFloorHeight, building.numStories - 1, storyHeight, windowRandSize);

		////////////////////////////
		// ROOF	
		addRoof(rendManager, roofOffCont, bldgColor, min_elev + buildingHeight, 1.0f);
	}
}

/**
 * Generate a house.
 * INCOMPLETE!
 */
void GeoBuilding::generateHouse(VBORenderManager& rendManager, Building& building) {
	Polygon3D& footprint = building.buildingFootprint;
	int numStories = building.numStories;
	//printf("numSt %d numSides %d\n",numStories,footprint.contour.size());

	///////////////////////////
	// SIDES
	float height = storyHeight;
	std::vector<Vertex> sideVert;
	int nextN;
	QVector3D normal;
	float rndColod = ((0.7f * (float)qrand()) / RAND_MAX) + 0.3f;
	QColor color(rndColod, rndColod, rndColod);

	std::vector<QVector3D> pts(4);
	pts[0] = (footprint[1] - footprint[0]).normalized() * 2 + footprint[0];
	pts[1] = footprint[1];
	pts[2] = footprint[2];
	pts[3] = (footprint[2] - footprint[3]).normalized() * 2 + footprint[3];

	for(int curN = 0; curN < pts.size(); ++curN) {
		nextN = (curN+1) % pts.size();
		normal = QVector3D::crossProduct(pts[nextN]-pts[curN], QVector3D(0,0,1)).normalized();
		sideVert.push_back(Vertex(QVector3D(pts[curN].x(), pts[curN].y(), pts[curN].z()), color, normal, QVector3D()));
		sideVert.push_back(Vertex(QVector3D(pts[nextN].x(), pts[nextN].y(), pts[nextN].z()), color, normal, QVector3D()));
		sideVert.push_back(Vertex(QVector3D(pts[nextN].x(), pts[nextN].y(), pts[nextN].z()+height), color, normal, QVector3D()));
		sideVert.push_back(Vertex(QVector3D(pts[curN].x(), pts[curN].y(), pts[curN].z()+height), color, normal, QVector3D()));			
	}
	rendManager.addStaticGeometry("3d_building", sideVert, "", GL_QUADS, 1 | mode_Lighting);

	// ROOF
	/*if(footprint.contour.size()==3||footprint.contour.size()==4){
		normal=QVector3D(0,0,1.0f);
		std::vector<Vertex> topVert;
		topVert.push_back(Vertex(QVector3D(footprint[0].x(),footprint[0].y(),footprint[0].z()+height),color,normal,QVector3D()));
		topVert.push_back(Vertex(QVector3D(footprint[1].x(),footprint[1].y(),footprint[1].z()+height),color,normal,QVector3D()));
		topVert.push_back(Vertex(QVector3D(footprint[2].x(),footprint[2].y(),footprint[2].z()+height),color,normal,QVector3D()));
		if(footprint.contour.size()==4){

			topVert.push_back(Vertex(QVector3D(footprint[3].x(),footprint[3].y(),footprint[3].z()+height),color,normal,QVector3D()));
		}else{
			topVert.push_back(Vertex(QVector3D(footprint[2].x(),footprint[2].y(),footprint[2].z()+height),color,normal,QVector3D()));
		}
		rendManager.addStaticGeometry("3d_building",topVert,"",GL_QUADS,1|mode_Lighting);//|LC::mode_AdaptTerrain|LC::mode_Lighting);
	}*/
}
