#pragma once
#ifndef MOUSERECT_HPP
#define MOUSERECT_HPP
#include <QWidget>
#include <QDialog>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QScreen>
#include "ui_mouserect.h"

class mouserect : public QDialog {
	Q_OBJECT

public:
	mouserect(QDialog *parent = 0);
	~mouserect();
	QRect getRect();
	public slots:

Q_SIGNALS:
	void signalCompleteCature(QPixmap catureImage);

private:
	void initWindow();
	void loadBackgroundPixmap();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void paintEvent(QPaintEvent *event);

private:
	bool m_isMousePress;
	QPixmap m_loadPixmap, m_capturePixmap;
	int m_screenwidth;
	int m_screenheight;
	QPoint m_beginPoint, m_endPoint;
	QPainter m_painter;
	Ui::mouserect ui;
};

#endif // MOUSERECT_HPP