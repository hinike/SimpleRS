#include "rsinfo.hpp"

RSInfo::RSInfo(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	area = new QScrollArea(this);
	area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setWidgetResizable(true);

	QWidget *widget = new QWidget(area);

	form = new QFormLayout;

	longitude_start = new QLabel;
	longitude_stop = new QLabel;
	latitude_start = new QLabel;
	latitude_stop = new QLabel;

	form->addRow(QString::fromLocal8Bit("起始经度："), longitude_start);
	form->addRow(QString::fromLocal8Bit("起始纬度："), latitude_start);
	form->addRow(QString::fromLocal8Bit("终止经度："), longitude_stop);
	form->addRow(QString::fromLocal8Bit("终止纬度："), latitude_stop);

	widget->setLayout(form);
	area->setWidget(widget);

	QVBoxLayout *mainlayout = new QVBoxLayout;
	mainlayout->addWidget(area);
	setLayout(mainlayout);
}

RSInfo::~RSInfo() {
	
}
