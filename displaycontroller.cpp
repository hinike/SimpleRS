#include "displaycontroller.hpp"

DisplayController::DisplayController(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	noticetext = new QLabel(this);
	noticetext->setText(QString::fromLocal8Bit("选择波段生成RGB图像"));
	noticetext->setWordWrap(true);

	RSelector = new QComboBox(this);
	GSelector = new QComboBox(this);
	BSelector = new QComboBox(this);
	CancelButton = new QPushButton(this);
	CancelButton->setText(QString::fromLocal8Bit("取消"));
	ApplyButton = new QPushButton(this);
	ApplyButton->setText(QString::fromLocal8Bit("应用"));

	connect(CancelButton, SIGNAL(clicked()), this, SLOT(close()));


	QVBoxLayout *mainlayout=new QVBoxLayout;
	QHBoxLayout *hlayout1 = new QHBoxLayout;
	QHBoxLayout *hlayout2 = new QHBoxLayout;
	QFormLayout *formlayout=new QFormLayout;

	hlayout1->addWidget(noticetext);

	formlayout->addRow("R:", RSelector);
	formlayout->addRow("G:", GSelector);
	formlayout->addRow("B:", BSelector);

	
	hlayout2->addWidget(CancelButton);
	hlayout2->addWidget(ApplyButton);

	mainlayout->setStretchFactor(hlayout1, 1);
	mainlayout->setStretchFactor(hlayout2, 1);
	mainlayout->setStretchFactor(formlayout, 3);
	mainlayout->addLayout(hlayout1);
	mainlayout->addLayout(formlayout);
	mainlayout->addLayout(hlayout2);

	area = new QScrollArea(this);
	area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area->setWidgetResizable(true);

	QWidget *widget = new QWidget(area);
	widget->setLayout(mainlayout);
	area->setWidget(widget);
	
	QVBoxLayout *lay = new QVBoxLayout;
	lay->addWidget(area);

	setLayout(lay);

	this->CancelButton->setVisible(false);
}

DisplayController::~DisplayController() {
	
}
