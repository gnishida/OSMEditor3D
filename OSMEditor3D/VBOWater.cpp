#include "VBOWater.h"
#include "VBORenderManager.h"
#include "global.h"

#define		MOD	0xff

static int		permut[256];
static const char	gradient[32][4] =
{
	{ 1,  1,  1,  0}, { 1,  1,  0,  1}, { 1,  0,  1,  1}, { 0,  1,  1,  1},
	{ 1,  1, -1,  0}, { 1,  1,  0, -1}, { 1,  0,  1, -1}, { 0,  1,  1, -1},
	{ 1, -1,  1,  0}, { 1, -1,  0,  1}, { 1,  0, -1,  1}, { 0,  1, -1,  1},
	{ 1, -1, -1,  0}, { 1, -1,  0, -1}, { 1,  0, -1, -1}, { 0,  1, -1, -1},
	{-1,  1,  1,  0}, {-1,  1,  0,  1}, {-1,  0,  1,  1}, { 0, -1,  1,  1},
	{-1,  1, -1,  0}, {-1,  1,  0, -1}, {-1,  0,  1, -1}, { 0, -1,  1, -1},
	{-1, -1,  1,  0}, {-1, -1,  0,  1}, {-1,  0, -1,  1}, { 0, -1, -1,  1},
	{-1, -1, -1,  0}, {-1, -1,  0, -1}, {-1,  0, -1, -1}, { 0, -1, -1, -1},
};

void InitNoise() {
	unsigned int i = 0;
	while (i < 256)
		permut[i++] = rand () & MOD;
}

/*
** Function finding out the gradient corresponding to the coordinates
*/
static int Indice(const int i, const int j, const int k, const int l) {
	return (permut[(l + permut[(k + permut[(j + permut[i & MOD])
		& MOD])
		& MOD])
		& MOD]
	& 0x1f);
}

/*
** Functions computing the dot product of the vector and the gradient
*/
static float Prod(const float a, const char b) {
	if (b > 0)
		return a;
	if (b < 0)
		return -a;
	return 0;
}

static float Dot_prod(const float x1, const char x2, const float y1, const char y2, const float z1, const char z2, const float t1, const char t2) {
	return (Prod (x1, x2) + Prod (y1, y2) + Prod (z1, z2) + Prod (t1, t2));
}

/*
** Functions computing interpolations
*/
static float Spline5(const float state) {
	/*
	** Enhanced spline :
	** (3x^2 + 2x^3) is not as good as (6x^5 - 15x^4 + 10x^3)
	*/
	const float sqr = state * state;
	return state * sqr * (6 * sqr - 15 * state + 10);
}

static float Linear (const float start, const float end, const float state) {
	return start + (end - start) * state;
}

/*
** Noise function, returning the Perlin Noise at a given point
*/
float Noise (const float x, const float y, const float z, const float t) {
	/* The unit hypercube containing the point */
	const int x1 = (int) (x > 0 ? x : x - 1);
	const int y1 = (int) (y > 0 ? y : y - 1);
	const int z1 = (int) (z > 0 ? z : z - 1);
	const int t1 = (int) (t > 0 ? t : t - 1);
	const int x2 = x1 + 1;
	const int y2 = y1 + 1;
	const int z2 = z1 + 1;
	const int t2 = t1 + 1;

	/* The 16 corresponding gradients */
	const char * g0000 = gradient[Indice (x1, y1, z1, t1)];
	const char * g0001 = gradient[Indice (x1, y1, z1, t2)];
	const char * g0010 = gradient[Indice (x1, y1, z2, t1)];
	const char * g0011 = gradient[Indice (x1, y1, z2, t2)];
	const char * g0100 = gradient[Indice (x1, y2, z1, t1)];
	const char * g0101 = gradient[Indice (x1, y2, z1, t2)];
	const char * g0110 = gradient[Indice (x1, y2, z2, t1)];
	const char * g0111 = gradient[Indice (x1, y2, z2, t2)];
	const char * g1000 = gradient[Indice (x2, y1, z1, t1)];
	const char * g1001 = gradient[Indice (x2, y1, z1, t2)];
	const char * g1010 = gradient[Indice (x2, y1, z2, t1)];
	const char * g1011 = gradient[Indice (x2, y1, z2, t2)];
	const char * g1100 = gradient[Indice (x2, y2, z1, t1)];
	const char * g1101 = gradient[Indice (x2, y2, z1, t2)];
	const char * g1110 = gradient[Indice (x2, y2, z2, t1)];
	const char * g1111 = gradient[Indice (x2, y2, z2, t2)];

	/* The 16 vectors */
	const float dx1 = x - x1;
	const float dx2 = x - x2;
	const float dy1 = y - y1;
	const float dy2 = y - y2;
	const float dz1 = z - z1;
	const float dz2 = z - z2;
	const float dt1 = t - t1;
	const float dt2 = t - t2;

	/* The 16 dot products */
	const float b0000 = Dot_prod(dx1, g0000[0], dy1, g0000[1],
		dz1, g0000[2], dt1, g0000[3]);
	const float b0001 = Dot_prod(dx1, g0001[0], dy1, g0001[1],
		dz1, g0001[2], dt2, g0001[3]);
	const float b0010 = Dot_prod(dx1, g0010[0], dy1, g0010[1],
		dz2, g0010[2], dt1, g0010[3]);
	const float b0011 = Dot_prod(dx1, g0011[0], dy1, g0011[1],
		dz2, g0011[2], dt2, g0011[3]);
	const float b0100 = Dot_prod(dx1, g0100[0], dy2, g0100[1],
		dz1, g0100[2], dt1, g0100[3]);
	const float b0101 = Dot_prod(dx1, g0101[0], dy2, g0101[1],
		dz1, g0101[2], dt2, g0101[3]);
	const float b0110 = Dot_prod(dx1, g0110[0], dy2, g0110[1],
		dz2, g0110[2], dt1, g0110[3]);
	const float b0111 = Dot_prod(dx1, g0111[0], dy2, g0111[1],
		dz2, g0111[2], dt2, g0111[3]);
	const float b1000 = Dot_prod(dx2, g1000[0], dy1, g1000[1],
		dz1, g1000[2], dt1, g1000[3]);
	const float b1001 = Dot_prod(dx2, g1001[0], dy1, g1001[1],
		dz1, g1001[2], dt2, g1001[3]);
	const float b1010 = Dot_prod(dx2, g1010[0], dy1, g1010[1],
		dz2, g1010[2], dt1, g1010[3]);
	const float b1011 = Dot_prod(dx2, g1011[0], dy1, g1011[1],
		dz2, g1011[2], dt2, g1011[3]);
	const float b1100 = Dot_prod(dx2, g1100[0], dy2, g1100[1],
		dz1, g1100[2], dt1, g1100[3]);
	const float b1101 = Dot_prod(dx2, g1101[0], dy2, g1101[1],
		dz1, g1101[2], dt2, g1101[3]);
	const float b1110 = Dot_prod(dx2, g1110[0], dy2, g1110[1],
		dz2, g1110[2], dt1, g1110[3]);
	const float b1111 = Dot_prod(dx2, g1111[0], dy2, g1111[1],
		dz2, g1111[2], dt2, g1111[3]);

	/* Then the interpolations, down to the result */
	const float idx1 = Spline5 (dx1);
	const float idy1 = Spline5 (dy1);
	const float idz1 = Spline5 (dz1);
	const float idt1 = Spline5 (dt1);

	const float b111 = Linear (b1110, b1111, idt1);
	const float b110 = Linear (b1100, b1101, idt1);
	const float b101 = Linear (b1010, b1011, idt1);
	const float b100 = Linear (b1000, b1001, idt1);
	const float b011 = Linear (b0110, b0111, idt1);
	const float b010 = Linear (b0100, b0101, idt1);
	const float b001 = Linear (b0010, b0011, idt1);
	const float b000 = Linear (b0000, b0001, idt1);

	const float b11 = Linear (b110, b111, idz1);
	const float b10 = Linear (b100, b101, idz1);
	const float b01 = Linear (b010, b011, idz1);
	const float b00 = Linear (b000, b001, idz1);

	const float b1 = Linear (b10, b11, idy1);
	const float b0 = Linear (b00, b01, idy1);

	return Linear(b0, b1, idx1);
}

VBOWater::VBOWater() {
}

float VBOWater::z(const float x, const float y, const float t) {
	const float x2 = x - 3;
	const float y2 = y + 1;
	const float xx = x2 * x2;
	const float yy = y2 * y2;
	float tmpZ = ((0.15 * sinf(2000 * sqrtf (xx + yy) - 4 * t) + Noise(1000 * x, 1000 * y, t, 0)) * 0.1875);

	return tmpZ + G::getFloat("sea_level");// 60.5;
}

/*
** Function called to update rendering
*/
void VBOWater::render(VBORenderManager& rendManager) {
	//printf("RenderWater\n");
	GLuint texture;
	texture=rendManager.loadTexture("data/textures/water.jpg");

	if(water_initialized==false){
		InitNoise ();
		water_initialized=true;
	}

	QTime curTime = QTime::currentTime();
	const float t = ((float)curTime.second()) + (float(curTime.msec())/1000.0f);

	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Vertices */
	int indice = 0;
	for (int j = 0; j < RESOLUTION - 1; ++j) {
		float y1 = (rendManager.maxPos.y() - rendManager.minPos.y()) / (RESOLUTION - 1) * j;
		float y2 = (rendManager.maxPos.y() - rendManager.minPos.y()) / (RESOLUTION - 1) * (j + 1);

		for (int i = 0; i < RESOLUTION; ++i) {
			float x = (rendManager.maxPos.x() - rendManager.minPos.x()) / (RESOLUTION - 1) * i;
			surface[indice + 0] = x - (rendManager.maxPos.x() - rendManager.minPos.x()) * 0.5;
			surface[indice + 1] = y1 - (rendManager.maxPos.y() - rendManager.minPos.y()) * 0.5;
			surface[indice + 2] = z(x, y1, t);
			surface[indice + 3] = x - (rendManager.maxPos.x() - rendManager.minPos.x()) * 0.5;
			surface[indice + 4] = y2 - (rendManager.maxPos.y() - rendManager.minPos.y()) * 0.5;
			surface[indice + 5] = z(x, y2, t);
			indice += 6;
		}
	}

	/* Normals */
	indice = 0;
	for (int j = 0; j < RESOLUTION - 1; ++j) {
		for (int i = 0; i < RESOLUTION; ++i) {
			QVector3D va(surface[indice + 6] - surface[indice], surface[indice + 7] - surface[indice + 1], surface[indice + 8] - surface[indice + 2]);
			QVector3D vb(surface[indice + 3] - surface[indice], surface[indice + 4] - surface[indice + 1], surface[indice + 5] - surface[indice + 2]);
			QVector3D n1 = QVector3D::crossProduct(va, vb);
			normal[indice + 0] = n1.x();
			normal[indice + 1] = n1.y();
			normal[indice + 2] = n1.z();

			if (j == RESOLUTION - 2) {
				va = QVector3D(surface[indice + 0] - surface[indice + 3], surface[indice + 1] - surface[indice + 4], surface[indice + 2] - surface[indice + 5]);
				vb = QVector3D(surface[indice + 9] - surface[indice + 3], surface[indice + 10] - surface[indice + 4], surface[indice + 11] - surface[indice + 5]);
			}
			else {
				va = QVector3D(surface[indice + 9] - surface[indice + 3], surface[indice + 10] - surface[indice + 4], surface[indice + 11] - surface[indice + 5]);
				vb = QVector3D(surface[indice + 3 + RESOLUTION * 6] - surface[indice + 3], surface[indice + 4 + RESOLUTION * 6] - surface[indice + 4], surface[indice + 5 + RESOLUTION * 6] - surface[indice + 5]);
			}
			QVector3D n2 = QVector3D::crossProduct(va, vb);
			normal[indice + 3] = n2.x();
			normal[indice + 4] = n2.y();
			normal[indice + 5] = n2.z();

			indice += 6;
		}
	}

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	/////////////////
	// POS
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float)*RESOLUTION*(RESOLUTION-1), &surface[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
		
	// NORMAL
	GLuint nomVBO;
	glGenBuffers(1, &nomVBO);
	glBindBuffer(GL_ARRAY_BUFFER, nomVBO);
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float)*RESOLUTION*(RESOLUTION-1), &normal[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);

	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i (glGetUniformLocation (rendManager.program, "mode"), 4);//MODE: water
	glUniform1i (glGetUniformLocation (rendManager.program, "tex0"), 0);//tex0: 0
		
	for (int i = 0; i < RESOLUTION - 1; ++i) {//magic 2 to avoid wrong strip
		glDrawArrays(GL_TRIANGLE_STRIP, i * RESOLUTION * 2, RESOLUTION * 2);
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &nomVBO);
	//glEnable(GL_CULL_FACE);
}//
