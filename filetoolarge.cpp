#include "filetoolarge.hpp"

FileTooLarge::FileTooLarge(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	resample = new QPushButton(this);
	resample->setText(QString::fromLocal8Bit("重采样"));
	//cut = new QPushButton(this);
	//cut->setText(QString::fromLocal8Bit("裁切"));
	cancle = new QPushButton(this);
	cancle->setText(QString::fromLocal8Bit("取消"));
	label = new QLabel(this);

	this->setFixedHeight(60);
	this->setFixedWidth(400);
	label->setWordWrap(true);
	label->setText(QString::fromLocal8Bit("图像太大，请问是否需要进行重采样？"));

	QVBoxLayout *vl = new QVBoxLayout;
	QHBoxLayout *v2 = new QHBoxLayout;

	vl->addWidget(label);
	v2->addWidget(resample);
	//v2->addWidget(cut);
	v2->addWidget(cancle);
	vl->addLayout(v2);
	setLayout(vl);

	connect(cancle, SIGNAL(clicked()), this, SLOT(close()));
	//connect(cut, SIGNAL(clicked()), this, SLOT(close()));
	//connect(resample, SIGNAL(clicked()), this, SLOT(close()));
}

FileTooLarge::~FileTooLarge() {
	
}
