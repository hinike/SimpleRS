#include "scrollpicview.hpp"

ScrollPicView::ScrollPicView(QWidget * parent) : QGraphicsView(parent) {
	ui.setupUi(this);
	scene = new QGraphicsScene;
	this->verticalScrollBar()->setTracking(true);
	this->horizontalScrollBar()->setTracking(true);
	this->setFocusPolicy(Qt::ClickFocus);
}

ScrollPicView::~ScrollPicView() {
	
}

void ScrollPicView::updateview(QImage *image)
{
	scene->clear();
	//this->setFixedSize(QSize(image.width(),image.height()));
	scene->addPixmap(QPixmap::fromImage(*image));
	
	W = ((*image).width() >= this->geometry().width()) ? (*image).width() 
		: this->geometry().width();
	H = ((*image).height() >= this->geometry().height()) ? (*image).height()
		: this->geometry().height();

	scene->setSceneRect(0,0,W,H);
	
	if (grid) {
		QPen pen;
		pen.setStyle(Qt::DashLine);
		pen.setColor(Qt::white);
		int x_posi = 0;
		while (x_posi < W) {
			scene->addLine(QLineF(x_posi + settings[0] * ratio[0], 0,
				x_posi + settings[0] * ratio[0], H),
				pen);
			int y_posi = 0;
			while (y_posi<H)
			{
				scene->addLine(QLineF(0, y_posi + settings[1] * ratio[1],
					W, y_posi + settings[1] * ratio[1]),
					pen);
				y_posi += settings[1] * ratio[1];
			}
			x_posi += settings[0] * ratio[0];
		}
	}

	this->setScene(scene);

	this->show();
}

