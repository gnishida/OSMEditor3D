#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	G::global()["road_min_level"] = 0;
	G::global()["major_road_width"] = 5;
	G::global()["minor_road_width"] = 5;
	G::global()["sidewalk_width"] = 2;
	G::global()["parksRatio"] = 0.0;
	G::global()["parcel_area_mean"] = 1000;
	G::global()["parcel_area_deviation"] = 100;
	G::global()["parcel_setback_front"] = 2;
	G::global()["parcel_setback_rear"] = 2;
	G::global()["parcel_setback_sides"] = 2;
	G::global()["building_stories_min"] = 1;
	G::global()["building_stories_max"] = 15;
	G::global()["building_min_dimension"] = 10;
	G::global()["tree_setback"] = 1;
	G::global()["shader2D"] = false;
	G::global()["show_water"] = false;
	G::global()["MAX_Z"] = 1000;

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
