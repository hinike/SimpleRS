#pragma once
#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP
#include <QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QImage>
#include <QtGui/QPixmap>
#include <gdal.h>  
#include <gdal_priv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;

#include <CString>

using namespace std;
#include "ui_histogram.h"

class Histogram : public QWidget {
	Q_OBJECT

public:
	Histogram(QWidget * parent = Q_NULLPTR);
	~Histogram();
	GDALDataset *src;
	void init_combo();

private:
	Ui::Histogram ui;
	
	QLabel *hist;
	QLabel *range;
	QImage image;
	QComboBox *box;
	QPushButton *Apply;
	QVBoxLayout *mainlayout;
	QFormLayout *formlayout;

private slots:
	void update_view();
};

#endif // HISTOGRAM_HPP