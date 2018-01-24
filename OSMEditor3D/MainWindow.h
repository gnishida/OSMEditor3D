#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glew.h"
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "PropertyWidget.h"
#include "GLWidget3D.h"
#include "UrbanGeometry.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	Ui::MainWindow ui;
	GLWidget3D* glWidget;
	UrbanGeometry* urbanGeometry;
	PropertyWidget* propertyWidget;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void setParameters();

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onNew();
	void onOpen();
	void onSave();
	void onClear();
	void onSaveImage();
	void onSaveImageHD();
	void onLoadCamera();
	void onSaveCamera();
	void onResetCamera();
	void onUndo();
	void onRedo();
	void onDeleteEdge();
	void onViewChanged();
	void onPropertyWindow();
};

#endif // MAINWINDOW_H
