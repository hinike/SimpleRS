#include "rs_demo_gdal.h"
#include "axis_horizontal.hpp"
#include "axis_vertical.hpp"
#include "resampleinput.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//Axis_Horizontal w;
	//Axis_Vertical w;
	RS_DEMO_GDAL w;
	//resampleinput w;
	//w.exec();
	w.showMaximized();

	return a.exec();
}