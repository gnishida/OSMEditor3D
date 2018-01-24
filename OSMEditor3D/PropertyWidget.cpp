#include "PropertyWidget.h"

PropertyWidget::PropertyWidget(MainWindow* mainWin) : QDockWidget("Property Window", (QWidget*)mainWin) {
	this->mainWin = mainWin;
	ui.setupUi(this);

	ui.comboBoxEdgeType->addItem("Highway");
	ui.comboBoxEdgeType->addItem("Boulevard");
	ui.comboBoxEdgeType->addItem("Avenue");
	ui.comboBoxEdgeType->addItem("Street");

	connect(ui.pushButtonApply, SIGNAL(clicked()), this, SLOT(onApply()));
}

void PropertyWidget::setRoadEdge(RoadEdgePtr edge) {
	this->edge = edge;
	if (edge) {
		switch (edge->type) {
		case RoadEdge::TYPE_HIGHWAY:
			ui.comboBoxEdgeType->setCurrentIndex(0);
			break;
		case RoadEdge::TYPE_BOULEVARD:
			ui.comboBoxEdgeType->setCurrentIndex(1);
			break;
		case RoadEdge::TYPE_AVENUE:
			ui.comboBoxEdgeType->setCurrentIndex(2);
			break;
		case RoadEdge::TYPE_STREET:
			ui.comboBoxEdgeType->setCurrentIndex(3);
			break;
		}

		ui.spinBoxNumLanes->setValue(edge->lanes);
		ui.checkBoxOneWay->setChecked(edge->oneWay);
	}
}

void PropertyWidget::onApply() {
	if (edge) {
		if (ui.comboBoxEdgeType->currentIndex() == 0) {
			edge->type = RoadEdge::TYPE_HIGHWAY;
		}
		else if (ui.comboBoxEdgeType->currentIndex() == 1) {
			edge->type = RoadEdge::TYPE_BOULEVARD;
		}
		else if (ui.comboBoxEdgeType->currentIndex() == 2) {
			edge->type = RoadEdge::TYPE_AVENUE;
		}
		else if (ui.comboBoxEdgeType->currentIndex() == 3) {
			edge->type = RoadEdge::TYPE_STREET;
		}
		edge->lanes = ui.spinBoxNumLanes->value();
		edge->oneWay = ui.checkBoxOneWay->isChecked();
	}
}