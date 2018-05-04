#pragma once
#ifndef SCROLLPICVIEW_HPP
#define SCROLLPICVIEW_HPP
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QPen>
#include <QtWidgets/QScrollBar>
#include "ui_scrollpicview.h"

class ScrollPicView : public QGraphicsView {
	Q_OBJECT

public:
	ScrollPicView(QWidget * parent = Q_NULLPTR);
	~ScrollPicView();
	void updateview(QImage *image);
	QGraphicsScene *scene;
	int settings[2] = {100,100};//[间距，保留位]
	double ratio[2] = { 1,1 };//{x,y}
	bool grid = false;
private:
	Ui::ScrollPicView ui;
	int W;
	int H;
};

#endif // SCROLLPICVIEW_HPP