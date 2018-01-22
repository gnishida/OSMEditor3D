#pragma once

#include "glew.h"
#include "VBOShader.h"
#include "VBOUtil.h"
#include "qmap.h"
#include <glm/glm.hpp>

#include "VBOWater.h"
#include "VBOSkyBox.h"
#include "VBOTerrain.h"

#include "VBOModel_StreetElements.h"
#include "Polyline3D.h"
#include "VBOModel.h"

#ifndef Q_MOC_RUN
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/polygon/polygon.hpp>
#endif

const int mode_AdaptTerrain=0x0100;
const int mode_Lighting=0x0200;
const int mode_TexArray=0x0400;
const int mode_Tex3D=0x0800;

//0x0100 --> adapt vboRenderManager
//0x0200 --> lighting

struct RenderSt {
	uint texNum; //0 means use color
	GLuint vbo;
	GLuint vao;
	int numVertex; //defines if the vbo has been created
	std::vector<Vertex> vertices;

	GLenum geometryType;
	int shaderMode;

	RenderSt(uint _texNum, const std::vector<Vertex>& _vertices, GLenum geoT, int shModer) {
		texNum = _texNum;
		vertices = _vertices;
		geometryType = geoT;
		shaderMode = shModer;
		numVertex = -1;
	}
	RenderSt() {
		numVertex = -1;
	}
};

/////////////////////////////////////
// VBORenderManager

class VBORenderManager {
public:
	GLuint program;
	
	VBOTerrain vboTerrain;
	QVector3D minPos;
	QVector3D maxPos;
	QVector2D size;
	VBOSkyBox vboSkyBox;	// sky
	VBOWater vboWater;		// water

	//QHash<QString, QHash<uint, RenderSt>> geoName2StaticRender;
	QHash<QString, QHash<uint, QHash<uint, RenderSt>>> geoName2StaticRender;

	QHash<QString, std::vector<ModelSpec>> nameToVectorModels;	// models

public:
	// POLYGON
	typedef boost::polygon::polygon_with_holes_data<double> polygonP;
	typedef boost::polygon::polygon_traits<polygonP>::point_type pointP;

	VBORenderManager();
	~VBORenderManager();

	void init(const QVector2D& size);

	void changeTerrainDimensions(const QVector2D& terrainSize);
	float getTerrainHeight(float x, float y);
	float getMinTerrainHeight(float x, float y, float radius = 20.0f);
	void changeTerrainShader(int newMode);

	void renderWater();

	// textures
	QHash<QString,GLuint> nameToTexId;
	GLuint loadTexture(const QString fileName, bool mirrored = false);
	GLuint loadArrayTexture(QString texName, const std::vector<QString>& fileNames);

	//static
	bool addStaticGeometry(const QString& geoName, const std::vector<Vertex>& vert, const QString& textureName, GLenum geometryType, int shaderMode);
	bool addStaticGeometry2(const QString& geoName, const std::vector<QVector3D>& pos, float zShift, const QString& textureName, int shaderMode, const QVector3D& texScale, const QColor& color);
	bool addStaticGeometry2WithHole(const QString& geoName, const std::vector<QVector3D>& pos, const std::vector<std::vector<QVector3D>>& holes, float zShift, const QString& textureName, int shaderMode, const QVector3D& texScale, const QColor& color);
	bool removeStaticGeometry(const QString& geoName);
	void renderStaticGeometry(const QString& geoName);

	void addStreetElementModel(const QString& name, const ModelSpec& mSpec);
	void renderAllStreetElementName(const QString& name);
	void removeAllStreetElementName(const QString& name);
	
	void renderAll();

private:
	void renderVAO(RenderSt& renderSt);
	bool createVAO(const std::vector<Vertex>& vert, GLuint& vbo, GLuint& vao, int& numVertex);
	void cleanVAO(GLuint vbo, GLuint vao);
};
