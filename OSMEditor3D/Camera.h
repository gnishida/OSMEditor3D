#pragma once

#include "glew.h"
#include "qvector4d.h"
#include "qmatrix4x4.h"
#include "global.h"
#include <fstream>
#include <iostream>

class Camera {
public:
	QVector3D rot;
	QVector3D pos;
	QVector3D lookAt;
	float fovy;

	QMatrix4x4 mvMatrix;
	QMatrix4x4 mvpMatrix;
	QMatrix4x4 pMatrix;
	QMatrix3x3 normalMatrix;

	Camera() {
		fovy = 60.0f;
	}

	QVector4D getCamPos() {
		QVector4D eye(0.0f, 0.0f, 0.0f, 1.0f);
		return ((mvMatrix.transposed()).inverted()) * eye;
	}

	void updatePerspective(int width,int height) {
		float aspect = (float)width / (float)height;
		float zfar = 30000.0f;
		float znear = 0.1f;
		float f = 1.0f / tan (fovy * (0.00872664625f));//PI/360

		float m[16] = {
			f/aspect,	0,								0,									0,
					0,	f,								0,						 			0,
			        0,	0,		(zfar+znear)/(znear-zfar),		(2.0f*zfar*znear)/(znear-zfar),
			        0,	0,		    				   -1,									0

		};
		pMatrix=QMatrix4x4(m);
		updateCamMatrix();
	}

	float getCamElevation() {	
		return getCamPos().z();
	}

	void setRotation(float x, float y, float z) {
		setXRotation(x);
		setYRotation(y);
		setZRotation(z);
		updateCamMatrix();
	}

	void setXRotation(float angle) {
		qNormalizeAngle(angle);
		rot.setX(angle);
		updateCamMatrix();
	}

	void setYRotation(float angle) {
		qNormalizeAngle(angle);
		rot.setY(angle);
		updateCamMatrix();
	}

	void setZRotation(float angle) {
		qNormalizeAngle(angle);
		rot.setZ(angle);
		updateCamMatrix();
	}

	void changeXRotation(float angle) {
		setXRotation(rot.x() + angle);
	}

	void changeYRotation(float angle) {
		setYRotation(rot.y() + angle);
	}

	void changeZRotation(float angle) {
		setZRotation(rot.z() + angle);
	}

	void setTranslation(float x, float y, float z) {
		pos = QVector3D(x, y, z);
		updateCamMatrix();
	}

	void changeXYZTranslation(float dx, float dy, float dz) {
		pos += QVector3D(dx, dy, dz);
		updateCamMatrix();
	}

	void setLookAt(float x, float y, float z) {
		lookAt = QVector3D(x, y, z);
		updateCamMatrix();
	}

	void changeLookAt(float dx, float dy, float dz) {
		lookAt += QVector3D(dx, dy, dz);
		updateCamMatrix();
	}

	void resetCamera() {
		setLookAt(0.0f, 0.0f, 0.0f);
		rot = QVector3D(0, 0, 0);
		setTranslation(0.0f, 0.0f, G::global().getFloat("MAX_Z"));
		updateCamMatrix();
	}

	void saveCameraPose(const std::string& filename) {
		std::ofstream out(filename);
		out << rot.x() << " " << rot.y() << " " << rot.z() << " ";
		out << pos.x() << " " << pos.y() << " " << pos.z() << " ";
		out << lookAt.x() << " " << lookAt.y() << " " << lookAt.z() << std::endl;
		out.close();
	}

	void loadCameraPose(const std::string& filename) {
		std::ifstream in(filename);

		float x, y, z;
		in >> x >> y >> z;
		rot = QVector3D(x, y, z);
		in >> x >> y >> z;
		pos = QVector3D(x, y, z);
		in >> x >> y >> z;
		lookAt = QVector3D(x, y, z);
		in.close();
	}

private:
	void qNormalizeAngle(float &angle) {
		while (angle < 0)
			angle += 360.0;
		while (angle >= 360.0)
			angle -= 360.0;
	}

	void updateCamMatrix() {
		mvMatrix.setToIdentity();
		mvMatrix.translate(-pos.x(), -pos.y(), -pos.z());
		mvMatrix.rotate(rot.x(), 1.0, 0.0, 0.0);
		mvMatrix.rotate(rot.y(), 0.0, 1.0, 0.0);
		mvMatrix.rotate(rot.z(), 0.0, 0.0, 1.0);
		
		//mvMatrix.translate(-lookAt.x(), -lookAt.y(), -lookAt.z());
		mvMatrix.translate(-lookAt.x() * cos(rot.z() / 180 * M_PI) - lookAt.y() * sin(rot.z() / 180 * M_PI), lookAt.x() * sin(rot.z() / 180 * M_PI) - lookAt.y() * cos(rot.z() / 180 * M_PI), -lookAt.z());

		// normal matrix
		normalMatrix = mvMatrix.normalMatrix();

		// mvp
		mvpMatrix = pMatrix*mvMatrix;
	}
};

