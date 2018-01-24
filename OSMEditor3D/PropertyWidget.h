#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QDockWidget>
#include "ui_PropertyWidget.h"
#include "RoadGraph.h"

class MainWindow;

class PropertyWidget : public QDockWidget {
	Q_OBJECT

private:
	Ui::PropertyWidget ui;
	MainWindow* mainWin;
	RoadEdgePtr edge;

public:
	PropertyWidget(MainWindow* mainWin);

	void setRoadEdge(RoadEdgePtr edge);

public slots:
	void onApply();
};

#endif // PROPERTYWIDGET_H
