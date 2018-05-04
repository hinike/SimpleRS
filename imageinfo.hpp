#pragma once
#include <QWidget>
#include "ui_imageinfo.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>

class ImageInfo : public QWidget {
	Q_OBJECT

public:
	ImageInfo(QWidget * parent = Q_NULLPTR);
	~ImageInfo();
	QLabel *imagetype;
	QLabel *xsize;
	QLabel *ysize;
	QLabel *bands;
	QLabel *datatype;
	//QLabel *

private:
	Ui::ImageInfo ui;
	QScrollArea *area;
	QVBoxLayout *layout;
	QFormLayout *form;
};
