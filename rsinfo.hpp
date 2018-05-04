#pragma once
#ifndef RSINFO_HPP
#define RSINFO_HPP
#include <QWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include "ui_rsinfo.h"

class RSInfo : public QWidget {
	Q_OBJECT

public:
	RSInfo(QWidget * parent = Q_NULLPTR);
	~RSInfo();
	QLabel *longitude_start;
	QLabel *longitude_stop;
	QLabel *latitude_start;
	QLabel *latitude_stop;

private:
	Ui::RSInfo ui;
	QScrollArea *area;
	QVBoxLayout *layout;
	QFormLayout *form;
};

#endif // RSINFO_HPP