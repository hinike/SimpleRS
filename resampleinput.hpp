#pragma once
#ifndef RESAMPLEINPUT_HPP
#define RESAMPLEINPUT_HPP
#include <QDialog>
#include <QLineEdit> 
#include <QLabel> 
#include <QPushButton> 
#include <QVBoxLayout> 
#include <QDialog>  
#include "ui_resampleinput.h"
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE


class resampleinput : public QDialog {
	Q_OBJECT


public:
	resampleinput(QWidget * parent = Q_NULLPTR);
	~resampleinput();
	double getdoublex();
	double getdoubley();
	//public slots:
	//	void accept();
private:
	Ui::resampleinput ui;
	QLineEdit *xLineEdit;
	QLineEdit *yLineEdit;

};

#endif // RESAMPLEINPUT_HPP