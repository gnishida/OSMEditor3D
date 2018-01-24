#pragma once

#include "glew.h"
#include "VBORenderManager.h"
#include "GLWidget3D_Shadows.h"

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include "Camera.h"
#include "RoadGraph.h"
#include "RoadGraphEditor.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	MainWindow* mainWin;
	Camera camera;

	bool shiftPressed;
	bool ctrlPressed;
	bool altPressed;
	QPoint prev_mouse_pt;
	bool first_paint;
	RoadGraphEditor editor;
	VBORenderManager vboRenderManager;
	GLWidgetSimpleShadow shadow;

public:
	GLWidget3D(MainWindow* parent);

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void drawScene(int drawMode);
	void render();
	void updateCamera();

protected:
	void initializeGL();

	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseDoubleClickEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
};

