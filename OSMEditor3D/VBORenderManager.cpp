#include "VBORenderManager.h"
#include "global.h"
#include "Polygon3D.h"

VBORenderManager::VBORenderManager() {
}

VBORenderManager::~VBORenderManager() {
	Shader::cleanShaders();
}

void VBORenderManager::init(const QVector2D& size) {
	this->size = size;
	minPos = QVector3D(-size.x() / 2.0f, -size.y() / 2.0f, 0);
	maxPos = QVector3D(size.x() / 2.0f, size.y() / 2.0f, 0);

	// init program shader
	program = Shader::initShader("data/shaders/lc_vertex_sk.glsl", "data/shaders/lc_fragment_sk.glsl");
	glUseProgram(program);

	vboTerrain.init(this, size);
	vboSkyBox.init(*this);

	nameToTexId[""] = 0;
}

GLuint VBORenderManager::loadTexture(const QString fileName, bool mirrored) {
	GLuint texId;
	if (nameToTexId.contains(fileName)) {
		texId = nameToTexId[fileName];
	} else {
		texId = VBOUtil::loadImage(fileName,mirrored);
		nameToTexId[fileName] = texId;
	}
	return texId;
}

GLuint VBORenderManager::loadArrayTexture(QString texName, const std::vector<QString>& fileNames) {
	GLuint texId;
	if (nameToTexId.contains(texName)) {
		texId = nameToTexId[texName];
	} else {
		texId = VBOUtil::loadImageArray(fileNames);
		nameToTexId[texName] = texId;
	}
	return texId;
}

// ATRIBUTES
// 0 Vertex
// 1 Color
// 2 Normal
// 3 UV

// UNIFORMS
// 0 mode
// 1 tex0

bool VBORenderManager::createVAO(const std::vector<Vertex>& vert, GLuint& vbo, GLuint& vao, int& numVertex) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Crete VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vert.size(), vert.data(), GL_STATIC_DRAW);
	
	// Configure the attributes in the VAO.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12 * sizeof(float)));
	
	// Bind back to the default state.
	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER,0);
	
	// clean space
	numVertex = vert.size();
	
	return true;
}

void VBORenderManager::renderVAO(RenderSt& renderSt) {
	//printf("renderVAO numVert %d texNum %d vao %d numVertVert %d\n",renderSt.numVertex,renderSt.texNum,renderSt.vao,renderSt.vertices.size());
	// 1. Create if necessary
	if (renderSt.numVertex != renderSt.vertices.size() && renderSt.vertices.size() > 0) {
		if (renderSt.numVertex != -1) {
			std::cout << "ERROR!!!!!!!!!" << std::endl;
			cleanVAO(renderSt.vbo, renderSt.vao);
		}
		// generate vao/vbo
		createVAO(renderSt.vertices, renderSt.vbo, renderSt.vao, renderSt.numVertex);
	}

	// 2. Render
	// 2.1 TEX
	int mode = renderSt.shaderMode;
	if ((mode & mode_TexArray) == mode_TexArray) {
		// MULTI TEX
		mode = mode & (~mode_TexArray);//remove tex array bit
		glActiveTexture(GL_TEXTURE8);

		glBindTexture(GL_TEXTURE_2D, 0); 
		glBindTexture(GL_TEXTURE_2D_ARRAY, renderSt.texNum);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i (glGetUniformLocation (program, "tex_3D"), 8);
	} else {
		glBindTexture(GL_TEXTURE_2D, renderSt.texNum);
	}

	glUniform1i(glGetUniformLocation (program, "mode"), mode);
	glUniform1i(glGetUniformLocation (program, "tex0"), 0);

	glBindVertexArray(renderSt.vao);
	glDrawArrays(renderSt.geometryType,0,renderSt.numVertex);
	glBindVertexArray(0);
}

void VBORenderManager::renderAll() {
	for (auto it = geoName2StaticRender.begin(); it != geoName2StaticRender.end(); ++it) {
		for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
			for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
				renderVAO(*it3);
			}
		}
	}
}

void VBORenderManager::cleanVAO(GLuint vbo,GLuint vao) {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

/**
 * Return the terrain elevation.
 *
 * @param x			x coordinate [-side/2, side/2]
 * @param y			y coordinate [-side/2, side/2]
 */
float VBORenderManager::getTerrainHeight(float x, float y) {
	float u = x / size.x() + 0.5f;
	float v = y / size.y() + 0.5f;
	if (u < 0) u = 0.0f;
	if (u > 1.0f) u = 1.0f;
	if (v < 0) v = 0.0f;
	if (v > 1.0f) v = 1.0f;

	return vboTerrain.getTerrainHeight(u, v);
}

/**
 * Return the minimum terrain elevation around the given poing.
 *
 * @param x			x coordinate [-side/2, side/2]
 * @param y			y coordinate [-side/2, side/2]
 * @param radius	radius
 */
float VBORenderManager::getMinTerrainHeight(float x, float y, float radius) {
	float u = x / size.x() + 0.5f;
	float v = y / size.y() + 0.5f;
	float r = radius / size.x();

	float min_z = std::numeric_limits<float>::max();
	for (int i = -1; i <= 1; ++i) {
		float uu = u + i * r;
		if (uu < 0) uu = 0.0f;
		if (uu > 1.0f) uu = 1.0f;

		for (int j = -1; j <= 1; ++j) {
			float vv = v + j * r;
			if (vv < 0) vv = 0.0f;
			if (vv > 1.0f) vv = 1.0f;

			float z = vboTerrain.getTerrainHeight(uu, vv);
			if (z < min_z) min_z = z;
		}
	}

	return min_z;
}

void VBORenderManager::changeTerrainDimensions(const QVector2D& terrainSize) {
	size = terrainSize;
	minPos = QVector3D(-terrainSize.x() / 2.0f, -terrainSize.y() / 2.0f, 0);
	maxPos = QVector3D(terrainSize.x() / 2.0f, terrainSize.y() / 2.0f, 0);
	vboTerrain.init(this, terrainSize);
	vboSkyBox.init(*this);
}

void VBORenderManager::changeTerrainShader(int newMode){
	glUniform1i(glGetUniformLocation(program, "terrainMode"), newMode);
}

///////////////////////////////////////////////////////////////////
// STATIC
bool VBORenderManager::addStaticGeometry(const QString& geoName, const std::vector<Vertex>& vert, const QString& texName, GLenum geometryType, int shaderMode) {
	if (vert.size() <= 0) return false;

	GLuint texId;
	if (nameToTexId.contains(texName)) {
		texId = nameToTexId[texName];
	} else {
		std::cout << "texture is loaded : " << texName.toUtf8().constData() << std::endl;
		texId = VBOUtil::loadImage(texName);
		nameToTexId[texName] = texId;
	}
	
	if (!geoName2StaticRender.contains(geoName) || !geoName2StaticRender[geoName].contains(texId) || !geoName2StaticRender[geoName][texId].contains(geometryType)) {
		geoName2StaticRender[geoName][texId][geometryType] = RenderSt(texId, vert, geometryType, shaderMode);
	}

	if (geoName2StaticRender[geoName][texId][geometryType].numVertex > 0) {
		std::cout << "Old vao is removed." << std::endl;
		geoName2StaticRender[geoName][texId][geometryType].numVertex = -1;
		cleanVAO(geoName2StaticRender[geoName][texId][geometryType].vbo, geoName2StaticRender[geoName][texId][geometryType].vao);
	}

	geoName2StaticRender[geoName][texId][geometryType].vertices.insert(geoName2StaticRender[geoName][texId][geometryType].vertices.end(), vert.begin(), vert.end());

	return true;
}

/**
	* 指定されたポリゴンに基づいて、ジオメトリを生成する。
	* 凹型のポリゴンにも対応するよう、ポリゴンは台形にtessellateする。
	*/
bool VBORenderManager::addStaticGeometry2(const QString& geoName, const std::vector<QVector3D>& pos, float zShift, const QString& textureName, int shaderMode, const QVector3D& texScale, const QColor& color){
	if (pos.size() < 3) return false;

	return addStaticGeometry2WithHole(geoName, pos, std::vector<std::vector<QVector3D>>(), zShift, textureName, shaderMode, texScale, color);
}

bool VBORenderManager::addStaticGeometry2WithHole(const QString& geoName, const std::vector<QVector3D>& pos, const std::vector<std::vector<QVector3D>>& holes, float zShift, const QString& textureName, int shaderMode, const QVector3D& texScale, const QColor& color){
	if (pos.size() < 3) return false;

	std::vector<pointP> vP;
	vP.resize(pos.size());
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = -std::numeric_limits<float>::max();
	float maxY = -std::numeric_limits<float>::max();

	for (int i = 0; i < pos.size(); ++i) {
		vP[i] = boost::polygon::construct<pointP>(pos[i].x(), pos[i].y());
		minX = std::min<float>(minX, pos[i].x());
		minY = std::min<float>(minY, pos[i].y());
		maxX = std::max<float>(maxX, pos[i].x());
		maxY = std::max<float>(maxY, pos[i].y());
	}

	// close the polygon
	if (pos.back().x() != pos.front().x() && pos.back().y() != pos.front().y()) {
		vP.push_back(vP[0]);
	}

	// create boost polygon
	polygonP polygon;
	boost::polygon::set_points(polygon, vP.begin(), vP.end());

	// add holes
	std::vector<boost::polygon::polygon_with_holes_traits<polygonP>::hole_type> hole_polys(holes.size());
	for (int i = 0; i < holes.size(); ++i) {
		std::vector<pointP> hole_poly(holes[i].size());
		for (int k = holes[i].size() - 1; k >= 0; --k) {
			hole_poly[k] = boost::polygon::construct<pointP>(holes[i][k].x(), holes[i][k].y());
		}
		hole_poly.push_back(hole_poly[0]);
		boost::polygon::set_points(hole_polys[i], hole_poly.begin(), hole_poly.end());
	}
	boost::polygon::set_holes(polygon, hole_polys.begin(), hole_polys.end());

	std::vector<polygonP> polySet;
	polySet.push_back(polygon);

	std::vector<polygonP> polygons;
	boost::polygon::get_trapezoids(polygons, polySet);

	std::vector<Vertex> vert;

	// triangulate each trapezoid
	for (int i = 0; i < polygons.size(); i++) {
		Polygon3D points;
		std::vector<QVector3D> texP;
		for (auto it = polygons[i].begin(); it != polygons[i].end(); ++it) {
			pointP cP = *it;
			points.push_back(QVector3D(it->x(), it->y(), pos[0].z() + zShift));
			texP.push_back(QVector3D((it->x() - minX) * texScale.x(), (it->y() - minY) * texScale.y(), 0.0f));
		}

		if (points.isClockwise()) {
			std::reverse(points.contour.begin(), points.contour.end());
			std::reverse(texP.begin(), texP.end());
		}

		for (int k = 1; k < points.contour.size() - 1; ++k) {
			vert.push_back(Vertex(points[0], color, QVector3D(0, 0, 1), texP[0]));
			vert.push_back(Vertex(points[k], color, QVector3D(0, 0, 1), texP[k]));
			vert.push_back(Vertex(points[k + 1], color, QVector3D(0, 0, 1), texP[k + 1]));
		}
	}

	return addStaticGeometry(geoName, vert, textureName, GL_TRIANGLES, shaderMode);
}

bool VBORenderManager::removeStaticGeometry(const QString& geoName) {
	if (geoName2StaticRender.contains(geoName)) {
		for (auto it = geoName2StaticRender[geoName].begin(); it != geoName2StaticRender[geoName].end(); ++it) {
			for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
				cleanVAO(it2->vbo, it2->vao);
			}
		}
		geoName2StaticRender.remove(geoName);
	} else {
		//std::cerr << "ERROR: Remove geometry " << geoName.toUtf8().constData() << ", but it did not exist." << std::endl;
		return false;
	}

	return true;
}

void VBORenderManager::renderStaticGeometry(const QString& geoName) {
	if (geoName2StaticRender.contains(geoName)) {
		for (auto it = geoName2StaticRender[geoName].begin(); it != geoName2StaticRender[geoName].end(); ++it) {
			for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
				renderVAO(*it2);
			}
		}
	} else {
		//std::cerr << "ERROR: Render geometry " << geoName.toUtf8().constData() << ", but it did not exist." << std::endl;
		return;
	}
}

////////////////////////////////////////////////////////////////////
// MODEL
void VBORenderManager::addStreetElementModel(const QString& name, const ModelSpec& mSpec) {
	nameToVectorModels[name].push_back(mSpec);
}

void VBORenderManager::renderAllStreetElementName(const QString& name) {
	for (int i = 0; i < nameToVectorModels[name].size(); ++i) {
		VBOModel_StreetElements::renderOneStreetElement(program,nameToVectorModels[name][i]);
	}
}

void VBORenderManager::removeAllStreetElementName(const QString& name) {
	nameToVectorModels[name].clear();
}
	
