#include "MainWindow.h"
#include <QFileDialog>
#include <QShortcut>
#include "Util.h"
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QActionGroup* group = new QActionGroup(this);
	group->addAction(ui.actionView2D);
	group->addAction(ui.actionView3D);
	ui.actionView2D->setChecked(true);
	ui.actionView3D->setChecked(false);

	// register the menu's action handlers
	connect(ui.actionLoadRoads, SIGNAL(triggered()), this, SLOT(onLoadRoads()));
	connect(ui.actionSaveRoads, SIGNAL(triggered()), this, SLOT(onSaveRoads()));
	connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(onClear()));

	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionSaveImageHD, SIGNAL(triggered()), this, SLOT(onSaveImageHD()));
	connect(ui.actionLoadCamera, SIGNAL(triggered()), this, SLOT(onLoadCamera()));
	connect(ui.actionSaveCamera, SIGNAL(triggered()), this, SLOT(onSaveCamera()));
	connect(ui.actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui.actionGenerateBlocks, SIGNAL(triggered()), this, SLOT(onGenerateBlocks()));
	connect(ui.actionGenerateParcels, SIGNAL(triggered()), this, SLOT(onGenerateParcels()));
	connect(ui.actionGenerateBuildings, SIGNAL(triggered()), this, SLOT(onGenerateBuildings()));
	connect(ui.actionGenerateVegetation, SIGNAL(triggered()), this, SLOT(onGenerateVegetation()));
	connect(ui.actionGenerateAll, SIGNAL(triggered()), this, SLOT(onGenerateAll()));

	connect(ui.actionView2D, SIGNAL(triggered()), this, SLOT(onViewChanged()));
	connect(ui.actionView3D, SIGNAL(triggered()), this, SLOT(onViewChanged()));
	connect(ui.actionShowWater, SIGNAL(triggered()), this, SLOT(onShowWater()));
	connect(ui.actionSeaLevel, SIGNAL(triggered()), this, SLOT(onSeaLevel()));

	// setup the GL widget
	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);

	//urbanGeometry = new UrbanGeometry(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::setParameters() {
	/*
	G::global()["parksRatio"] = controlWidget->ui.lineEditParkRatio->text().toFloat();
	G::global()["parcel_area_mean"] = controlWidget->ui.lineEditParcelArea->text().toFloat();
	G::global()["parcel_area_deviation"] = controlWidget->ui.lineEditParcelAreaDev->text().toFloat();
	G::global()["parcel_setback_front"] = controlWidget->ui.lineEditSetbackFront->text().toFloat();
	G::global()["parcel_setback_rear"] = controlWidget->ui.lineEditSetbackRear->text().toFloat();
	G::global()["parcel_setback_sides"] = controlWidget->ui.lineEditSetbackSide->text().toFloat();
	G::global()["building_stories_min"] = controlWidget->ui.lineEditNumStoriesMin->text().toFloat();
	G::global()["building_stories_max"] = controlWidget->ui.lineEditNumStoriesMax->text().toFloat();
	G::global()["building_min_dimension"] = controlWidget->ui.lineEditMinBuildingDim->text().toFloat();
	*/
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onLoadRoads() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load roads..."), "", tr("OSM files (*.osm)"));
	if (filename.isEmpty()) return;

	urbanGeometry->clear();
	urbanGeometry->loadRoads(filename);
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onSaveRoads() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save roads..."), "", tr("Shapefiles Files (*.osm)"));
	if (filename.isEmpty()) return;

	QApplication::setOverrideCursor(Qt::WaitCursor);

	urbanGeometry->saveRoads(filename);

	QApplication::restoreOverrideCursor();
}

void MainWindow::onClear() {
	urbanGeometry->clear();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onSaveImage() {
	if (!QDir("screenshots").exists()) QDir().mkdir("screenshots");
	QString fileName = "screenshots/" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("HHmmss") + ".png";
	glWidget->grabFrameBuffer().save(fileName);
}

void MainWindow::onSaveImageHD() {
	if (!QDir("screenshots").exists()) QDir().mkdir("screenshots");
	QString fileName = "screenshots/" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("HHmmss") + "_HD.png";
	int cH = glWidget->height();
	int cW = glWidget->width();
	glWidget->resize(cW * 3, cH * 3);
	glWidget->updateGL();
	glWidget->grabFrameBuffer().save(fileName);
	glWidget->resize(cW,cH);
	glWidget->updateGL();
}

void MainWindow::onLoadCamera() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Camera file..."), "", tr("Camera files (*.cam)"));
	if (filename.isEmpty()) return;

	glWidget->camera.loadCameraPose(filename.toUtf8().constData());
	glWidget->updateCamera();
	glWidget->updateGL();
}

void MainWindow::onSaveCamera() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Camera file..."), "", tr("Camera files (*.cam)"));
	if (filename.isEmpty()) return;
	
	glWidget->camera.saveCameraPose(filename.toUtf8().constData());
}

void MainWindow::onResetCamera() {
	glWidget->camera.resetCamera();
	glWidget->updateCamera();
	glWidget->updateGL();
}

void MainWindow::onGenerateBlocks() {
	setParameters();
	urbanGeometry->generateBlocks();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateParcels() {
	setParameters();
	urbanGeometry->generateParcels();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateBuildings() {
	setParameters();
	urbanGeometry->generateBuildings();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateVegetation() {
	setParameters();
	urbanGeometry->generateVegetation();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateAll() {
	setParameters();
	urbanGeometry->generateAll(true);
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateCity() {
	std::cout << "Generating a city...";

	urbanGeometry->clear();

	// set the parameter values
	setParameters();

	urbanGeometry->generateAll(true);
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();

	std::cout << " Done." << std::endl;
}

void MainWindow::onViewChanged() {
	G::global()["shader2D"] = ui.actionView2D->isChecked();

	int terrainMode;
	if (ui.actionView2D->isChecked()) {
		terrainMode = 0;

		// change to top view
		glWidget->camera.resetCamera();
	}
	else  {
		terrainMode = 1;
	}

	glWidget->vboRenderManager.changeTerrainShader(terrainMode);
	urbanGeometry->update(glWidget->vboRenderManager);
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onShowWater() {
	G::global()["show_water"] = ui.actionShowWater->isChecked();
	glWidget->updateGL();
}

void MainWindow::onSeaLevel() {
	bool ok;
	QInputDialog dlg;
	dlg.setOptions(QInputDialog::NoButtons);		
	QString text = dlg.getText(NULL, "Sea Level", "Enter sea level", QLineEdit::Normal,	QString::number(G::getFloat("sea_level")), &ok);	
	if (ok && !text.isEmpty()) {
		G::global()["sea_level"] = text.toFloat();
		glWidget->updateGL();
	}
}

