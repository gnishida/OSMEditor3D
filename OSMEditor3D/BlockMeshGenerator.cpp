#include "BlockMeshGenerator.h"
#include <QDir>
#include "global.h"
#include <boost/throw_exception.hpp>

bool BlockMeshGenerator::initialized = false;
std::vector<QString> BlockMeshGenerator::sideWalkFileNames;
std::vector<QVector3D> BlockMeshGenerator::sideWalkScale;
std::vector<QString> BlockMeshGenerator::grassFileNames;

/**
 * Load texture
 */
void BlockMeshGenerator::init() {
	QString pathName = "data/textures/LC";

	// 3. sidewalk
	QDir directorySW(pathName + "/sidewalk/");
	QStringList nameFilter;
	nameFilter << "*.png" << "*.jpg" << "*.gif";
	QStringList list = directorySW.entryList(nameFilter, QDir::Files);
	for (int lE = 0; lE < list.size(); ++lE) {
		if (QFile::exists(pathName + "/sidewalk/" + list[lE])) {
			sideWalkFileNames.push_back(pathName + "/sidewalk/" + list[lE]);
			QStringList scaleS = list[lE].split("_");
			if (scaleS.size() != 4)
				sideWalkScale.push_back(QVector3D(1.0f, 1.0f, 0));
			else{
				sideWalkScale.push_back(QVector3D(scaleS[1].toFloat(), scaleS[2].toFloat(), 0));
			}
		}
	}
	grassFileNames.push_back("data/textures/LC/grass/grass01.jpg");
	grassFileNames.push_back("data/textures/LC/grass/grass02.jpg");
	grassFileNames.push_back("data/textures/LC/grass/grass03.jpg");
	grassFileNames.push_back("data/textures/LC/grass/grass04.jpg");

	initialized = true;
}

void BlockMeshGenerator::generateBlockMesh(VBORenderManager& rendManager, const BlockSet& blocks) {
	if (!initialized) init();

	// generate 3d model
	rendManager.removeStaticGeometry("3d_blocks");
	rendManager.removeStaticGeometry("3d_parks");

	const float deltaZ = 2.3f;

	for (int i = 0; i < blocks.size(); ++i) {
		// side walks
		{
			std::vector<QVector3D> polygon;
			for (int pi = 0; pi < blocks[i].sidewalkContour.size(); ++pi) {
				polygon.push_back(QVector3D(blocks[i].sidewalkContour[pi].x(), blocks[i].sidewalkContour[pi].y(), deltaZ));
			}


			std::vector<std::vector<QVector3D>> holes;
			for (int cN = 0; cN < blocks[i].blockContours.size(); ++cN) {
				std::vector<QVector3D> hole;
				for (int pN = 0; pN < blocks[i].blockContours[cN].size(); ++pN) {
					hole.push_back(QVector3D(blocks[i].blockContours[cN][pN].x(), blocks[i].blockContours[cN][pN].y(), deltaZ));
				}
				holes.push_back(hole);
			}

			int randSidewalk = 2; //qrand()%grassFileNames.size();
			try {
				//rendManager.addStaticGeometry2("3d_blocks", polygon, 0.0f,  sideWalkFileNames[randSidewalk], 2 | mode_AdaptTerrain, sideWalkScale[randSidewalk], QColor());
				rendManager.addStaticGeometry2WithHole("3d_blocks", polygon, holes, 0.0f, sideWalkFileNames[randSidewalk], 2 | mode_AdaptTerrain, sideWalkScale[randSidewalk], QColor());
			}
			catch (boost::exception const& ex) {
			}

			// side of the side walks
			std::vector<Vertex> vert;
			for (int sN = 0; sN < blocks[i].sidewalkContour.size(); ++sN) {
				int ind1 = sN;
				int ind2 = (sN + 1) % blocks[i].sidewalkContour.size();
				QVector3D dir = blocks[i].sidewalkContour[ind2] - blocks[i].sidewalkContour[ind1];
				float length = dir.length();
				dir /= length;

				QVector3D p1 = QVector3D(blocks[i].sidewalkContour[ind1].x(), blocks[i].sidewalkContour[ind1].y(), 0);
				QVector3D p2 = QVector3D(blocks[i].sidewalkContour[ind2].x(), blocks[i].sidewalkContour[ind2].y(), 0);
				QVector3D p3 = QVector3D(blocks[i].sidewalkContour[ind2].x(), blocks[i].sidewalkContour[ind2].y(), deltaZ);
				QVector3D p4 = QVector3D(blocks[i].sidewalkContour[ind1].x(), blocks[i].sidewalkContour[ind1].y(), deltaZ);
				QVector3D normal = QVector3D::crossProduct(p2 - p1, p4 - p1).normalized();
				vert.push_back(Vertex(p1, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p2, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p3, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p4, QColor(128, 128, 128), normal, QVector3D()));
			}
			rendManager.addStaticGeometry("3d_blocks", vert, "", GL_QUADS, 1 | mode_Lighting | mode_AdaptTerrain);
		}

		// parks
		if (blocks[i].isPark) {
			// PARK
			int randPark = qrand() % grassFileNames.size();
			for (int cN = 0; cN < blocks[i].blockContours.size(); ++cN) {
				rendManager.addStaticGeometry2("3d_parks",  blocks[i].blockContours[cN].contour, deltaZ + 0.1f, grassFileNames[randPark], 2 | mode_AdaptTerrain, QVector3D(0.25f, 0.25f, 0.25f), QColor());
			}

			// side
			std::vector<Vertex> vert;
			for (int cN = 0; cN < blocks[i].blockContours.size(); ++cN) {
				for (int pN = 0; pN < blocks[i].blockContours[cN].size(); ++pN) {
					int ind1 = pN;
					int ind2 = (pN + 1) % blocks[i].blockContours[cN].size();
					QVector3D dir = blocks[i].blockContours[cN][ind2] - blocks[i].blockContours[cN][ind1];
					float length = dir.length();
					dir /= length;

					QVector3D p1 = QVector3D(blocks[i].blockContours[cN][ind1].x(), blocks[i].blockContours[cN][ind1].y(), 0);
					QVector3D p2 = QVector3D(blocks[i].blockContours[cN][ind2].x(), blocks[i].blockContours[cN][ind2].y(), 0);
					QVector3D p3 = QVector3D(blocks[i].blockContours[cN][ind2].x(), blocks[i].blockContours[cN][ind2].y(), deltaZ + 0.1f);
					QVector3D p4 = QVector3D(blocks[i].blockContours[cN][ind1].x(), blocks[i].blockContours[cN][ind1].y(), deltaZ + 0.1f);
					QVector3D normal = QVector3D::crossProduct(p2 - p1, p4 - p1).normalized();
					vert.push_back(Vertex(p1, QColor(128, 128, 128), normal, QVector3D()));
					vert.push_back(Vertex(p2, QColor(128, 128, 128), normal, QVector3D()));
					vert.push_back(Vertex(p3, QColor(128, 128, 128), normal, QVector3D()));
					vert.push_back(Vertex(p4, QColor(128, 128, 128), normal, QVector3D()));
				}
			}
			rendManager.addStaticGeometry("3d_parks", vert, "", GL_QUADS, 1 | mode_Lighting | mode_AdaptTerrain);
		}
	}
}

/**
 * generate parcel mesh
 */
void BlockMeshGenerator::generateParcelMesh(VBORenderManager& rendManager, const BlockSet& blocks) {
	rendManager.removeStaticGeometry("3d_parcels");

	const float deltaZ = 2.6f;

	for (int i = 0; i < blocks.size(); ++i) {
		if (blocks.blocks[i].isPark) continue;
							
		for (int pN = 0; pN < blocks[i].parcels.size(); ++pN) {
			const Polygon3D& contour = blocks[i].parcels[pN].parcelContour;

			if (contour.isSelfIntersecting()) {
				std::cout << "ERROR!!!!!!!!!!!!!   parcel contour is self intersected." << std::endl;
				continue;
			}

			if (contour.isClockwise()) {
				std::cout << "ERROR!!!!!!!!!!!!!   parcel contour is clockwised." << std::endl;
			}

			// top surface
			int randPark = 1; //qrand()%grassFileNames.size();
			rendManager.addStaticGeometry2("3d_parcels", contour.contour, deltaZ, grassFileNames[randPark], 2 | mode_AdaptTerrain, QVector3D(0.25f, 0.25f, 0.25f), QColor());
			
			// side
			std::vector<Vertex> vert;
			for (int sN = 0; sN < contour.size(); ++sN) {
				int ind1 = sN;
				int ind2 = (sN + 1) % contour.size();
				
				QVector3D p1(contour[ind1].x(), contour[ind1].y(), 0);
				QVector3D p2(contour[ind2].x(), contour[ind2].y(), 0);
				QVector3D p3(contour[ind2].x(), contour[ind2].y(), deltaZ);
				QVector3D p4(contour[ind1].x(), contour[ind1].y(), deltaZ);
				QVector3D normal = QVector3D::crossProduct(p2 - p1, p4 - p1).normalized();
				vert.push_back(Vertex(p1, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p2, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p3, QColor(128, 128, 128), normal, QVector3D()));
				vert.push_back(Vertex(p4, QColor(128, 128, 128), normal, QVector3D()));
			}

			rendManager.addStaticGeometry("3d_parcels", vert, "", GL_QUADS, 1 | mode_Lighting | mode_AdaptTerrain);
		}
	}
}

void BlockMeshGenerator::generate2DParcelMesh(VBORenderManager& rendManager, const BlockSet& blocks) {
	rendManager.removeStaticGeometry("2d_blocks");
	rendManager.removeStaticGeometry("2d_blocks_contour");
	rendManager.removeStaticGeometry("2d_parks");

	const float deltaZ = 3.0f;

	QColor parkColor(0xca, 0xdf, 0xaa);
	for (int bN = 0; bN < blocks.size(); ++bN) {
		if (blocks[bN].isPark) {
			for (int cN = 0; cN < blocks[bN].blockContours.size(); ++cN) {
				Loop3D parkC = blocks[bN].blockContours[cN].contour;
				if (parkC.size() > 2) {
					parkC.push_back(parkC.front());
					rendManager.addStaticGeometry2("2d_parks", parkC, deltaZ, "", 1, QVector3D(), parkColor);
				}
			}
		}
	}

	// Draw parcels as lines
	float parcelLine = 0.2;// G::global().getFloat("2d_parcelLine");
	if (parcelLine > 0) {
		glLineWidth(parcelLine);
		std::vector<Vertex> vert;
		for(int bN = 0; bN < blocks.size(); ++bN) {
			if (blocks[bN].isPark) continue;
						
			for (int pN = 0; pN < blocks[bN].parcels.size(); ++pN) {
				const Parcel& parcel = blocks[bN].parcels[pN];

				if (parcel.parcelContour.isSelfIntersecting()) continue;

				for (int i = 0; i < parcel.parcelContour.size(); ++i) {
					int next = (i + 1) % parcel.parcelContour.size();
					vert.push_back(Vertex(parcel.parcelContour[i] + QVector3D(0, 0, deltaZ), QColor(150, 150, 150), QVector3D(0, 0, 1), QVector3D()));
					vert.push_back(Vertex(parcel.parcelContour[next] + QVector3D(0, 0, deltaZ), QColor(150, 150, 150), QVector3D(0, 0, 1), QVector3D()));
				}
				
				if (parcel.isPark) {
					rendManager.addStaticGeometry2("2d_parks", parcel.parcelContour.contour, deltaZ, "", 1, QVector3D(), parkColor);
				}
				else {
					// generate footprint polygon mesh of the bulding
					const Loop3D& footprint = parcel.building.buildingFootprint.contour;
					for (int i = 0; i < footprint.size(); ++i) {
						int next = (i + 1) % footprint.size();
						vert.push_back(Vertex(footprint[i] + QVector3D(0, 0, deltaZ), QColor(156, 143, 186), QVector3D(0, 0, 1), QVector3D()));
						vert.push_back(Vertex(footprint[next] + QVector3D(0, 0, deltaZ), QColor(156, 143, 186), QVector3D(0, 0, 1), QVector3D()));
					}

					rendManager.addStaticGeometry2("2d_blocks", footprint, deltaZ, "", 1, QVector3D(), QColor(205, 191, 242));
				}
			}
		}
		rendManager.addStaticGeometry("2d_blocks_contour", vert, "", GL_LINES, 1);
	}
}
