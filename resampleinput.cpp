#include "resampleinput.hpp"

resampleinput::resampleinput(QWidget * parent) : QDialog(parent) {
	ui.setupUi(this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	QLabel *xlabel;
	QLabel *ylabel;
	xLineEdit = new QLineEdit;
	yLineEdit = new QLineEdit;
	QPushButton *okbutton = new QPushButton(tr("ok"));
	layout->addWidget(xlabel = new QLabel(QStringLiteral("输入横向倍率：")));
	layout->addWidget(xLineEdit);
	layout->addWidget(ylabel = new QLabel(QStringLiteral("输入纵向倍率：")));
	layout->addWidget(yLineEdit);
	layout->addWidget(okbutton);
	setLayout(layout);
	setWindowTitle(QStringLiteral("输入采样率："));
	xLineEdit->setPlaceholderText("1.0");
	yLineEdit->setPlaceholderText("1.0");

	setFixedSize(sizeHint());

	connect(okbutton, SIGNAL(clicked()), this, SLOT(accept()));
}

resampleinput::~resampleinput() {
	
}

double resampleinput::getdoublex()
{
	QString str = xLineEdit->text();
	
	double val = 0;
	val=str.toDouble();
	
	if (val <= 0) {
		return 1;
	}
	else {
		return val;
	}
}

double resampleinput::getdoubley()
{
	QString str = yLineEdit->text();
	double val = 0;
	val = str.toDouble();
	if (val <= 0) {
		return 1;
	}
	else {
		return val;
	}
}