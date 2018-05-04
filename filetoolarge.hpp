#pragma once
#ifndef FILETOOLARGE_HPP
#define FILETOOLARGE_HPP
#include <QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QVboxLayout>
#include <QHboxLayout>
#include "ui_filetoolarge.h"

class FileTooLarge : public QWidget {
	Q_OBJECT

public:
	FileTooLarge(QWidget * parent = Q_NULLPTR);
	~FileTooLarge();
	QPushButton *resample;
	QPushButton *cut;
	QPushButton *cancle;

private:
	Ui::FileTooLarge ui;
	QLabel* label;
};

#endif // FILETOOLARGE_HPP