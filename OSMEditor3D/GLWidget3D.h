#pragma once

#include "glew.h"
#include "VBORenderManager.h"
#include "GLWidget3D_Shadows.h"

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include "Camera.h"
#include "RoadGraph.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	MainWindow* mainWin;
	Camera camera;

	bool shiftPressed;
	bool ctrlPressed;
	bool altPressed;
	QPoint lastPos;

	VBORenderManager vboRenderManager;
	GLWidgetSimpleShadow shadow;

public:
	GLWidget3D(MainWindow* parent);

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void drawScene(int drawMode);
	void updateCamera();

protected:
	void initializeGL();

	void resizeGL(int width, int height);
	void paintGL();    

	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
};

