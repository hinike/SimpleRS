#pragma once
#ifndef DISPLAYCONTROLLER_HPP
#define DISPLAYCONTROLLER_HPP
#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QPushButton>
#include "ui_displaycontroller.h"

class DisplayController : public QWidget {
	Q_OBJECT

public:
	DisplayController(QWidget * parent = Q_NULLPTR);
	~DisplayController();
	QPushButton *ApplyButton;
	QComboBox *RSelector;
	QComboBox *GSelector;
	QComboBox *BSelector;

private:
	Ui::DisplayController ui;
	QLabel *noticetext;
	QPushButton *CancelButton;
	QScrollArea *area;
};

#endif // DISPLAYCONTROLLER_HPP