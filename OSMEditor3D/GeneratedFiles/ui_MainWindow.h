/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNewTerrain;
    QAction *actionOpenTerrain;
    QAction *actionExit;
    QAction *actionControlWidget;
    QAction *actionSaveTerrain;
    QAction *actionSaveRoads;
    QAction *actionPropertyWidget;
    QAction *actionLoadRoads;
    QAction *actionClear;
    QAction *actionSaveImage;
    QAction *actionLoadCamera;
    QAction *actionSaveCamera;
    QAction *actionSaveImageHD;
    QAction *actionResetCamera;
    QAction *actionGenerateBlocks;
    QAction *actionGenerateParcels;
    QAction *actionGenerateBuildings;
    QAction *actionGenerateVegetation;
    QAction *actionGenerateAll;
    QAction *actionLoadParcels;
    QAction *actionLoadBuildings;
    QAction *actionGenerateRoads;
    QAction *actionLoadZone;
    QAction *actionSaveParcels;
    QAction *actionSaveBuildings;
    QAction *actionGenerateScenarios;
    QAction *actionView2D;
    QAction *actionView3D;
    QAction *actionSeaLevel;
    QAction *actionShowWater;
    QAction *actionLoadOSMRoads;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuPM;
    QMenu *menuView;
    QToolBar *fileToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1006, 800);
        MainWindow->setMinimumSize(QSize(1000, 800));
        actionNewTerrain = new QAction(MainWindow);
        actionNewTerrain->setObjectName(QStringLiteral("actionNewTerrain"));
        actionOpenTerrain = new QAction(MainWindow);
        actionOpenTerrain->setObjectName(QStringLiteral("actionOpenTerrain"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionControlWidget = new QAction(MainWindow);
        actionControlWidget->setObjectName(QStringLiteral("actionControlWidget"));
        actionSaveTerrain = new QAction(MainWindow);
        actionSaveTerrain->setObjectName(QStringLiteral("actionSaveTerrain"));
        actionSaveRoads = new QAction(MainWindow);
        actionSaveRoads->setObjectName(QStringLiteral("actionSaveRoads"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveRoads->setIcon(icon);
        actionPropertyWidget = new QAction(MainWindow);
        actionPropertyWidget->setObjectName(QStringLiteral("actionPropertyWidget"));
        actionLoadRoads = new QAction(MainWindow);
        actionLoadRoads->setObjectName(QStringLiteral("actionLoadRoads"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoadRoads->setIcon(icon1);
        actionClear = new QAction(MainWindow);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        actionSaveImage = new QAction(MainWindow);
        actionSaveImage->setObjectName(QStringLiteral("actionSaveImage"));
        actionLoadCamera = new QAction(MainWindow);
        actionLoadCamera->setObjectName(QStringLiteral("actionLoadCamera"));
        actionSaveCamera = new QAction(MainWindow);
        actionSaveCamera->setObjectName(QStringLiteral("actionSaveCamera"));
        actionSaveImageHD = new QAction(MainWindow);
        actionSaveImageHD->setObjectName(QStringLiteral("actionSaveImageHD"));
        actionResetCamera = new QAction(MainWindow);
        actionResetCamera->setObjectName(QStringLiteral("actionResetCamera"));
        actionGenerateBlocks = new QAction(MainWindow);
        actionGenerateBlocks->setObjectName(QStringLiteral("actionGenerateBlocks"));
        actionGenerateParcels = new QAction(MainWindow);
        actionGenerateParcels->setObjectName(QStringLiteral("actionGenerateParcels"));
        actionGenerateBuildings = new QAction(MainWindow);
        actionGenerateBuildings->setObjectName(QStringLiteral("actionGenerateBuildings"));
        actionGenerateVegetation = new QAction(MainWindow);
        actionGenerateVegetation->setObjectName(QStringLiteral("actionGenerateVegetation"));
        actionGenerateAll = new QAction(MainWindow);
        actionGenerateAll->setObjectName(QStringLiteral("actionGenerateAll"));
        actionLoadParcels = new QAction(MainWindow);
        actionLoadParcels->setObjectName(QStringLiteral("actionLoadParcels"));
        actionLoadBuildings = new QAction(MainWindow);
        actionLoadBuildings->setObjectName(QStringLiteral("actionLoadBuildings"));
        actionGenerateRoads = new QAction(MainWindow);
        actionGenerateRoads->setObjectName(QStringLiteral("actionGenerateRoads"));
        actionLoadZone = new QAction(MainWindow);
        actionLoadZone->setObjectName(QStringLiteral("actionLoadZone"));
        actionSaveParcels = new QAction(MainWindow);
        actionSaveParcels->setObjectName(QStringLiteral("actionSaveParcels"));
        actionSaveBuildings = new QAction(MainWindow);
        actionSaveBuildings->setObjectName(QStringLiteral("actionSaveBuildings"));
        actionGenerateScenarios = new QAction(MainWindow);
        actionGenerateScenarios->setObjectName(QStringLiteral("actionGenerateScenarios"));
        actionView2D = new QAction(MainWindow);
        actionView2D->setObjectName(QStringLiteral("actionView2D"));
        actionView2D->setCheckable(true);
        actionView3D = new QAction(MainWindow);
        actionView3D->setObjectName(QStringLiteral("actionView3D"));
        actionView3D->setCheckable(true);
        actionSeaLevel = new QAction(MainWindow);
        actionSeaLevel->setObjectName(QStringLiteral("actionSeaLevel"));
        actionShowWater = new QAction(MainWindow);
        actionShowWater->setObjectName(QStringLiteral("actionShowWater"));
        actionShowWater->setCheckable(true);
        actionLoadOSMRoads = new QAction(MainWindow);
        actionLoadOSMRoads->setObjectName(QStringLiteral("actionLoadOSMRoads"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1006, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuPM = new QMenu(menuBar);
        menuPM->setObjectName(QStringLiteral("menuPM"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        MainWindow->setMenuBar(menuBar);
        fileToolBar = new QToolBar(MainWindow);
        fileToolBar->setObjectName(QStringLiteral("fileToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, fileToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuPM->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuFile->addAction(actionLoadRoads);
        menuFile->addAction(actionSaveRoads);
        menuFile->addSeparator();
        menuFile->addAction(actionClear);
        menuFile->addSeparator();
        menuFile->addAction(actionSaveImage);
        menuFile->addAction(actionSaveImageHD);
        menuFile->addSeparator();
        menuFile->addAction(actionLoadCamera);
        menuFile->addAction(actionSaveCamera);
        menuFile->addAction(actionResetCamera);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuPM->addAction(actionGenerateBlocks);
        menuPM->addAction(actionGenerateParcels);
        menuPM->addAction(actionGenerateBuildings);
        menuPM->addAction(actionGenerateVegetation);
        menuPM->addAction(actionGenerateAll);
        menuView->addAction(actionView2D);
        menuView->addAction(actionView3D);
        menuView->addSeparator();
        menuView->addAction(actionShowWater);
        menuView->addAction(actionSeaLevel);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Simple Cities", 0));
        actionNewTerrain->setText(QApplication::translate("MainWindow", "New Terrain", 0));
        actionNewTerrain->setShortcut(QString());
        actionOpenTerrain->setText(QApplication::translate("MainWindow", "Open Terrain", 0));
        actionOpenTerrain->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0));
        actionControlWidget->setText(QApplication::translate("MainWindow", "Control Widget", 0));
        actionSaveTerrain->setText(QApplication::translate("MainWindow", "Save Terrain", 0));
        actionSaveTerrain->setShortcut(QString());
        actionSaveRoads->setText(QApplication::translate("MainWindow", "Save Roads", 0));
        actionSaveRoads->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionPropertyWidget->setText(QApplication::translate("MainWindow", "Property Widget", 0));
        actionLoadRoads->setText(QApplication::translate("MainWindow", "Load Roads", 0));
        actionLoadRoads->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionClear->setText(QApplication::translate("MainWindow", "Clear", 0));
        actionClear->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0));
        actionSaveImage->setText(QApplication::translate("MainWindow", "Save Image", 0));
        actionSaveImage->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", 0));
        actionLoadCamera->setText(QApplication::translate("MainWindow", "Load Camera", 0));
        actionLoadCamera->setShortcut(QApplication::translate("MainWindow", "Ctrl+1", 0));
        actionSaveCamera->setText(QApplication::translate("MainWindow", "Save Camera", 0));
        actionSaveCamera->setShortcut(QApplication::translate("MainWindow", "Ctrl+2", 0));
        actionSaveImageHD->setText(QApplication::translate("MainWindow", "Save Image HD", 0));
        actionSaveImageHD->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionResetCamera->setText(QApplication::translate("MainWindow", "Reset Camera", 0));
        actionGenerateBlocks->setText(QApplication::translate("MainWindow", "Generate Blocks", 0));
        actionGenerateParcels->setText(QApplication::translate("MainWindow", "Generate Parcels", 0));
        actionGenerateBuildings->setText(QApplication::translate("MainWindow", "Generate Buildings", 0));
        actionGenerateVegetation->setText(QApplication::translate("MainWindow", "Generate Vegetation", 0));
        actionGenerateAll->setText(QApplication::translate("MainWindow", "Generate All", 0));
        actionLoadParcels->setText(QApplication::translate("MainWindow", "Load Parcels", 0));
        actionLoadBuildings->setText(QApplication::translate("MainWindow", "Load Buildings", 0));
        actionGenerateRoads->setText(QApplication::translate("MainWindow", "Generate Roads", 0));
        actionGenerateRoads->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0));
        actionLoadZone->setText(QApplication::translate("MainWindow", "Load Zone", 0));
        actionSaveParcels->setText(QApplication::translate("MainWindow", "Save Parcels", 0));
        actionSaveBuildings->setText(QApplication::translate("MainWindow", "Save Buildings", 0));
        actionGenerateScenarios->setText(QApplication::translate("MainWindow", "Generate Scenarios", 0));
        actionView2D->setText(QApplication::translate("MainWindow", "2D", 0));
        actionView3D->setText(QApplication::translate("MainWindow", "3D", 0));
        actionSeaLevel->setText(QApplication::translate("MainWindow", "Sea Level", 0));
        actionShowWater->setText(QApplication::translate("MainWindow", "Show Water", 0));
        actionLoadOSMRoads->setText(QApplication::translate("MainWindow", "Load OSM Roads", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuPM->setTitle(QApplication::translate("MainWindow", "PM", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
