#pragma once

#include <QVector3d>
#include "qmatrix4x4.h"
#include <vector>

#include "nvModel\nvModel.h"
#include "VBOUtil.h"


	struct ModelSpec {
		QMatrix4x4 transMatrix;
		std::vector<QVector3D> colors;
		int type;
	};

	class VBOModel{
	public:

		VBOModel();

		~VBOModel();

		std::vector<QString> fileNames;

		void initModel(std::vector<QString>& fileNames);
		void initScale(float scale);
		void initScale(float scaleX,float scaleY,float scaleZ);
		void setPositions(std::vector<QVector3D>& positions);

		void loadModel();
		void clearModel();
		void renderModel(int programId,ModelSpec& modelSpec);

		std::vector<GLuint> vertexVBO;
		std::vector<GLuint> indexVBO;
		std::vector<GLuint> vaoVBO;
		int modelInitialized;

		std::vector<int> stride;
		std::vector<int> normalOffset;
		std::vector<int> numIndexCount;
		QVector3D scale;

		std::vector<QVector3D> positions;

	};