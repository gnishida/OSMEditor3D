#include "PmVegetation.h"

#include "VBORenderManager.h"
#include "BBox.h"
#include "Util.h"
#include "global.h"

PmVegetation::PmVegetation() {
}

PmVegetation::~PmVegetation() {
}

ModelSpec PmVegetation::addTree(const QVector3D& pos) {
	ModelSpec tree;

	tree.transMatrix.setToIdentity();
	tree.transMatrix.translate(pos);
	tree.transMatrix.rotate(pos.x()*pos.y(),0.0f,0.0f,1.0f);//random
	tree.transMatrix.scale(0.8f + (((0.5f*qrand())/RAND_MAX))-0.5f);
	tree.colors.resize(2);
	//trunk
	tree.colors[1]=QVector3D(0.27f,0.22f,0.22f);
	tree.colors[1]=tree.colors[1]+QVector3D(1.0,1.0,1.0f)*((0.2f*qrand())/RAND_MAX-0.1f);
	//leaves
	int treeColor=qrand()%100;
	int desvLeaf=qrand()%40;
	if(treeColor>=0&&treeColor<=46)
		tree.colors[0]=QVector3D(115-desvLeaf,169-desvLeaf,102-desvLeaf);//green to dark 
	if(treeColor>=47&&treeColor<=92)
		tree.colors[0]=QVector3D(69-desvLeaf/2.0f,109-desvLeaf/2.0f,72-desvLeaf/2.0f);//green to dark 
	if(treeColor>=93&&treeColor<=94)
		tree.colors[0]=QVector3D(155-desvLeaf/3.0f,124-desvLeaf/3.0f,24-desvLeaf/3.0f);//yellow to dark 

	if(treeColor>=95&&treeColor<=96)
		tree.colors[0]=QVector3D(96-desvLeaf/4.0f,25-desvLeaf/4.0f,33-desvLeaf/4.0f);//red to dark 

	if(treeColor>=97&&treeColor<=100)
		tree.colors[0]=QVector3D(97-desvLeaf/2.0f,69-desvLeaf/2.0f,58-desvLeaf/2.0f);//grey to dark 
	tree.colors[0]/=255.0f;
	tree.type=0;

	return tree;
}

ModelSpec PmVegetation::addStreetLap(const QVector3D& pos, const QVector3D& contourDir) {
	ModelSpec stEl;

	stEl.transMatrix.setToIdentity();
	stEl.transMatrix.translate(pos);
	QVector3D perP=QVector3D::crossProduct(contourDir,QVector3D(0,0,1.0f));

	float rotAngle=atan2(perP.y(),perP.x())*57.2957795f;//rad to degrees (angle to rotate will be the tan since we compare respect 1,0,0)
	stEl.transMatrix.rotate(rotAngle,0.0f,0.0f,1.0f);
	stEl.transMatrix.scale(0.6f);

	stEl.colors.resize(1);
	//body color
	stEl.colors[0]=QVector3D(0.35f,0.35f,0.35f);
	stEl.type=1;

	return stEl;
}

bool PmVegetation::generateVegetation(VBORenderManager& rendManager, const std::vector<Block>& blocks){
	const float deltaZ = 2.0f;

	rendManager.removeAllStreetElementName("streetLamp");
	rendManager.removeAllStreetElementName("tree");

	float treesPerSqMeter = 0.04f;// 0.002f; //used for trees in parcels

	// generate trees in blocks (park)
	for (int bN = 0; bN < blocks.size(); bN++) {
		if (blocks[bN].isPark) {
			for (int cN = 0; cN < blocks[bN].blockContours.size(); ++cN) {
				BBox bbox = blocks[bN].blockContours[cN].envelope();
				int numTrees = blocks[bN].blockContours[cN].area() * treesPerSqMeter;

				for (int i = 0; i < numTrees; ++i) {
					QVector3D pos;
					while (true) {
						pos.setX(Util::genRand(bbox.minPt.x(), bbox.maxPt.x()));
						pos.setY(Util::genRand(bbox.minPt.y(), bbox.maxPt.y()));
						pos.setZ(deltaZ);
						if (blocks[bN].blockContours[cN].contains(pos)) {
							break;
						}
					}

					rendManager.addStreetElementModel("tree", addTree(pos));
				}
			}
		} else {			
			for (int pN = 0; pN < blocks[bN].parcels.size(); ++pN) {
				if (blocks[bN].parcels[pN].isPark) {
					BBox bbox = blocks[bN].parcels[pN].parcelContour.envelope();
					int numTrees = blocks[bN].parcels[pN].parcelContour.area() * treesPerSqMeter;

					for (int i = 0; i < numTrees; ++i) {
						QVector3D pos;
						while (true) {
							pos.setX(Util::genRand(bbox.minPt.x(), bbox.maxPt.x()));
							pos.setY(Util::genRand(bbox.minPt.y(), bbox.maxPt.y()));
							pos.setZ(deltaZ);
							if (blocks[bN].parcels[pN].parcelContour.contains(pos)) {
								break;
							}
						}

						rendManager.addStreetElementModel("tree", addTree(pos));
					}
				}
				else {
					// for the parcel with building, put trees in the remaining area outside the footprint
					BBox bbox = blocks[bN].parcels[pN].parcelContour.envelope();
					int numTrees = (blocks[bN].parcels[pN].parcelContour.area() - blocks[bN].parcels[pN].building.buildingFootprint.area()) * treesPerSqMeter;

					for (int i = 0; i < numTrees; ++i) {
						QVector3D pos;
						while (true) {
							pos.setX(Util::genRand(bbox.minPt.x(), bbox.maxPt.x()));
							pos.setY(Util::genRand(bbox.minPt.y(), bbox.maxPt.y()));
							pos.setZ(deltaZ);
							if (blocks[bN].parcels[pN].parcelContour.contains(pos) && !blocks[bN].parcels[pN].building.buildingFootprint.contains(pos)) {
								break;
							}
						}

						rendManager.addStreetElementModel("tree", addTree(pos));
					}
				}
			}
		}
	}

	float distanceBetweenLamps = 50.0f;//23 N 15.0f; //used for trees along streets

	// generate trees along streets
	float tree_setback = G::getFloat("tree_setback");
	for (int i = 0; i < blocks.size(); ++i) {
		const Loop3D contour = blocks[i].sidewalkContour.contour;

		float distLeftOver = tree_setback;

		for (int j = 0; j < contour.size(); ++j) {
			QVector3D ptThis = contour[j];
			QVector3D ptNext = contour[(j + 1) % contour.size()];
			QVector3D segmentVector = ptNext - ptThis;
			float segmentLength = segmentVector.length();
			segmentVector /= segmentLength;

			QVector3D perpV = QVector3D(segmentVector.y(), -segmentVector.x(), 0);
			ptThis = ptThis - perpV * tree_setback;

			float distFromSegmentStart = distLeftOver;
			while (true) {
				if (distFromSegmentStart > segmentLength - tree_setback) {
					distLeftOver = distFromSegmentStart - segmentLength;
					break;
				}

				if (j == contour.size() - 1) {
					// For the last segment, don't place a tree close to the last end point
					if (segmentLength - distFromSegmentStart < distanceBetweenLamps * 0.5f) break;
				}			

				QVector3D pos = ptThis + segmentVector * distFromSegmentStart;
				pos.setZ(deltaZ);
				rendManager.addStreetElementModel("streetLamp", addStreetLap(pos, segmentVector));

				distFromSegmentStart += distanceBetweenLamps * Util::genRand(0.4, 0.6);
			}
		}
	}

	return true;
}
