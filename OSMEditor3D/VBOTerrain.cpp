#include "VBOTerrain.h"
#include "VBOUtil.h"
#include <QImage>
#include <QGLWidget>
#include "global.h"
#include "VBORenderManager.h"

VBOTerrain::VBOTerrain() {
	initialized = false;
	texId = 0;
}

void VBOTerrain::init(VBORenderManager* rendManager, const QVector2D& size) {
	this->rendManager = rendManager;
	//this->_resolution = resolution;
	this->size = size;

	if (!initialized) {
		int numLevels=4;
		std::vector<QString> fileNames(numLevels);
		for (int i = 0; i < numLevels; ++i) {
			fileNames[i] = "data/textures/0" + QString::number(i+1) + "_terrain.jpg";
		}
		grassText = VBOUtil::loadImageArray(fileNames);
	}

	// TERRAIN LAYER
	layerData = cv::Mat(size.y(), size.x(), CV_32FC1, cv::Scalar(0.0f));
	updateTexture();

	//////////////////
	// VERTICES
	if (initialized) {
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &elementbuffer);
		vbo = 0;
		elementbuffer = 0;
	}

	std::vector<Vertex> vert;
	float step = 1;
		
	// VERTEX
	vert.resize(size.y() * size.x());
	int count = 0;
	for (int r = 0; r < size.y(); ++r) {
		for (int c = 0; c < size.x(); ++c) {
			QVector3D pos = QVector3D(c * step, r * step, 0) + rendManager->minPos;
			vert[count] = Vertex(pos, pos/100.0f);
			count++;
		}
	}
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vert.size(), vert.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// INDEX
	std::vector<uint> indices;
	indices.resize(size.x() * size.y() * 4);
	count = 0;
	for (int row = 0; row < size.y() - 1; ++row) {
		for (int col = 0; col < size.x() - 1; ++col) {
			indices[count] = col + row * size.x();
			indices[count + 1] = col + 1 + row * size.x();
			indices[count + 2] = col + 1 + (row + 1) * size.x();
			indices[count + 3] = col + (row + 1) * size.x();
			count += 4;
		}
	}

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	indicesCount=indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	initialized = true;
}

void VBOTerrain::render() {
	// glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	GLuint vao;
	glGenVertexArrays(1,&vao); 
	glBindVertexArray(vao); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D,0); 
	glBindTexture(GL_TEXTURE_2D_ARRAY, grassText);
		
	glActiveTexture(GL_TEXTURE0);

	glUniform1i(glGetUniformLocation(rendManager->program, "mode"), 3);//MODE: terrain
	glUniform1i(glGetUniformLocation(rendManager->program, "tex_3D"), 8);//tex0: 0

	glUniform1i(glGetUniformLocation(rendManager->program, "terrain_tex"), 7);//tex0: 0
	glUniform4f(glGetUniformLocation(rendManager->program, "terrain_size"), 
		rendManager->minPos.x(),
		rendManager->minPos.y(),
		(rendManager->maxPos.x() - rendManager->minPos.x()), 
		(rendManager->maxPos.y() - rendManager->minPos.y())
		);//tex0: 0

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(4*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(8*sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(12*sizeof(float)));

	// Draw the triangles 
	glDrawElements(GL_QUADS, indicesCount, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBindVertexArray(0);
	glDeleteVertexArrays(1,&vao);
}

/**
 * ガウシアン分布に基づき、高さを上げる。
 *
 * @param u				中心のX座標 [0, 1]
 * @param v				中心のY座標 [0, 1]
 * @param height		ガウス分布の最大高さ
 * @param rad_ratio		半径のサイズ（グリッドサイズに対する比）
 */
void VBOTerrain::updateGaussian(float u, float v, float height, float rad_ratio) {
	float x0 = u * size.x();// * _resolution;
	float y0 = v * size.y();// *_resolution;
	float sigma = rad_ratio * size.x();// *_resolution;

	for (int c = 0; c < layerData.cols; c++) {
		for (int r = 0; r < layerData.rows; r++) {
			float x = c;
			float y = r;

			float z = layerData.at<float>(r, c) + height * expf(-(SQR(x - x0) + SQR(y - y0)) / (2 * sigma * sigma));
			if (z < 0) z = 0.0f;
			layerData.at<float>(r, c) = z;
		}
	}

	// update texture
	updateTexture();
}

/**
 * 指定した位置を中心とする円の範囲の高さを、指定した高さheightにする。
 *
 * @param u				中心のX座標 [0, 1]
 * @param v				中心のY座標 [0, 1]
 * @param height		この高さに設定する
 * @param rad_ratio		半径のサイズ（グリッドサイズに対する比）
 */
void VBOTerrain::excavate(float u, float v, float height, float rad_ratio) {
	float x0 = u * size.x();// _resolution;
	float y0 = v * size.y();// _resolution;
	float rad = rad_ratio * size.x();// _resolution;

	for (int c = x0 - rad; c <= x0 + rad + 1; ++c) {
		if (c < 0 || c >= layerData.cols) continue;
		for (int r = y0 - rad; r <= y0 + rad + 1; ++r) {
			if (r < 0 || r >= layerData.rows) continue;

			float x = c;
			float y = r;

			if (SQR(x - x0) + SQR(y - y0) > SQR(rad)) continue;
			
			layerData.at<float>(r, c) = height;
		}
	}

	// update texture
	updateTexture();
}

void VBOTerrain::smoothTerrain() {
	cv::blur(layerData,layerData,cv::Size(3, 3));

	// update texture
	updateTexture();
}

/**
 * Return the terrain elevation.
 *
 * @param u			X coordinate [0, 1]
 * @param v			Y coordinate [0, 1]
 */
float VBOTerrain::getTerrainHeight(float u, float v) {
	if (u < 0) u = 0.0f;
	if (u >= 1.0f) u = 1.0f;
	if (v < 0) v = 0.0f;
	if (v >= 1.0f) v = 1.0f;

	int c1 = u * size.x();
	int c2 = u * size.x() + 1;
	int r1 = v * size.y();
	int r2 = v * size.y() + 1;


	if (c1 >= layerData.cols) c1 = layerData.cols - 1;
	if (c2 >= layerData.cols) c2 = layerData.cols - 1;
	if (r1 >= layerData.rows) r1 = layerData.rows - 1;
	if (r2 >= layerData.rows) r2 = layerData.rows - 1;

	float v1 = layerData.at<float>(r1, c1);
	float v2 = layerData.at<float>(r2, c1);
	float v3 = layerData.at<float>(r1, c2);
	float v4 = layerData.at<float>(r2, c2);

	float v12,v34;
	if (r2 == r1) {
		v12 = v1;
		v34 = v3;
	} else {
		float t = v * size.y() - r1;
		v12 = v1 * (1-t) + v2 * t;
		v34 = v3 * (1-t) + v4 * t;
	}

	if (c2 == c1) {
		return v12;
	} else {
		float s = u * size.x() - c1;
		return v12 * (1-s) + v34 * s;
	}
}

void VBOTerrain::setTerrain(const cv::Mat& mat) {
	layerData = mat;
	updateTexture();
}
	
void VBOTerrain::saveTerrain(const QString& fileName) {
	cv::Mat saveImage	= cv::Mat(layerData.rows, layerData.cols, CV_8UC4, layerData.data);
	cv::imwrite(fileName.toUtf8().constData(), saveImage);
}

/**
 * Update the texture according to the layer data.
 */
void VBOTerrain::updateTexture() {
	if (texId != 0) {
		glDeleteTextures(1, &texId);
		texId = 0;
	}

	glActiveTexture(GL_TEXTURE7);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, layerData.cols, layerData.rows);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layerData.cols, layerData.rows, GL_RED, GL_FLOAT, layerData.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE0);
}