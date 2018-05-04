#include "imageinfo.hpp"

ImageInfo::ImageInfo(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	area = new QScrollArea(this);
	area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setWidgetResizable(true);

	QWidget *widget = new QWidget(area);

	form = new QFormLayout;

	imagetype = new QLabel;
	xsize = new QLabel;
	ysize = new QLabel;
	bands = new QLabel;
	datatype = new QLabel;

	form->addRow(QString::fromLocal8Bit("临时图像驱动："), imagetype);
	form->addRow(QString::fromLocal8Bit("X方向尺寸："), xsize);
	form->addRow(QString::fromLocal8Bit("Y方向尺寸："), ysize);
	form->addRow(QString::fromLocal8Bit("波段数："), bands);
	form->addRow(QString::fromLocal8Bit("像素数据类型："), datatype);

	widget->setLayout(form);
	area->setWidget(widget);

	QVBoxLayout *mainlayout = new QVBoxLayout;
	mainlayout->addWidget(area);
	setLayout(mainlayout);
}

ImageInfo::~ImageInfo() {
	
}
