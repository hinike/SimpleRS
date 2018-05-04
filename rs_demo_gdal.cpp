#include "rs_demo_gdal.h"


RS_DEMO_GDAL::RS_DEMO_GDAL(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	pDataOri = NULL;
	pData = NULL;
	pDataSrc = NULL;
	init_ui();//initialize ui
	std::cout << "init ui ok" << endl;
	init_signal_slot();//initialize signals and slots
	std::cout << "init signal & slot ok" << endl;
}

RS_DEMO_GDAL::~RS_DEMO_GDAL()
{
	if (pData&&pDataSrc) {
		pData->Release();
		pDataSrc->Release();
		remove(".temp");
		remove("src.temp");
		remove("data.temp");
		remove("cut.temp");
		remove("resample.temp");
		remove("src_copy.temp");
	}
}

void RS_DEMO_GDAL::keyPressEvent(QKeyEvent * event)
{
	clock_t cb, ce;
	cb = clock();
	if (!pDataSrc || !pData) {
		return;
	}
	
	if (event->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && event->key() == Qt::Key_Plus) {
		if ((view_after->ratio)[0] < 5) {
			(view_after->ratio)[0] *= 1.2;
			(view_after->ratio)[1] *= 1.2;
			ratio_after[1] *= 1.2;
			ratio_after[0] *= 1.2;
			SetViewFocus = 1;
			updatecontroller();
			updateinfo();
		}
	}
	else if (event->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && event->key() == Qt::Key_Underscore)
	{
		if ((view_after->ratio)[0] > 0.2) {
			(view_after->ratio)[0] /= 1.2;
			(view_after->ratio)[1] /= 1.2;
			ratio_after[1] /= 1.2;
			ratio_after[0] /= 1.2;
			SetViewFocus = 1;
			updatecontroller();
			updateinfo();
		}
	}
	else if (event->modifiers() == (Qt::ControlModifier) && event->key() == Qt::Key_Equal)
	{
		if ((view_origin->ratio)[0] < 5) {
			(view_origin->ratio)[0] *= 1.2;
			(view_origin->ratio)[1] *= 1.2;
			ratio_origin[1] *= 1.2;
			ratio_origin[0] *= 1.2;
			SetViewFocus = 0;
			updatecontroller();
			updateinfo();
		}
	}
	else if (event->modifiers() == (Qt::ControlModifier) && event->key() == Qt::Key_Minus)
	{
		if ((view_origin->ratio)[0] > 0.2) {
			(view_origin->ratio)[0] /= 1.2;
			(view_origin->ratio)[1] /= 1.2;
			ratio_origin[1] /= 1.2;
			ratio_origin[0] /= 1.2;
			SetViewFocus = 0;
			updatecontroller();
			updateinfo();
		}
	}
	ce = clock();
	cout << "缩放用时：" << ce - cb << endl;
}


void RS_DEMO_GDAL::init_ui()
{
	errordlg = new QErrorMessage(this);
	
	filetoolarge = new FileTooLarge;
	//menu bar init
	menu_file = (ui.menuBar)->addMenu(QString::fromLocal8Bit("文件"));
	act_open = menu_file->addAction(QString::fromLocal8Bit("打开"));
	act_open->setIcon(QIcon(":/img/Resources/Open_64px.png"));
	act_open->setShortcut(QKeySequence::Open);
	act_save = menu_file->addAction(QString::fromLocal8Bit("保存"));
	act_save->setIcon(QIcon(":/img/Resources/Save_64px.png"));
	act_save->setShortcut(QKeySequence::Save);
	//act_saveas = menu_file->addAction("Save as...");
	//act_saveas->setIcon(QIcon(":/img/Resources/Save_as_64px.png"));
	
	menu_enhance= (ui.menuBar)->addMenu(QString::fromLocal8Bit("图像增强"));
	menu_filter = menu_enhance->addMenu(QString::fromLocal8Bit("滤波"));
	menu_filter->setIcon(QIcon(":/img/Resources/More_64px.png"));
	act_filt_mean = menu_filter->addAction(QString::fromLocal8Bit("均值滤波"));
	act_filt_mean->setIcon(QIcon(":/img/Resources/Filter_64px.png"));
	act_filt_median = menu_filter->addAction(QString::fromLocal8Bit("中值滤波"));
	act_filt_median->setIcon(QIcon(":/img/Resources/Filter_64px.png"));
	act_filt_gaussian = menu_filter->addAction(QString::fromLocal8Bit("高斯滤波"));
	act_filt_gaussian->setIcon(QIcon(":/img/Resources/Filter_64px.png"));
	act_sharpen = menu_enhance->addAction(QString::fromLocal8Bit("锐化"));
	act_sharpen->setIcon(QIcon(":/img/Resources/Pencil Sharpener_64px.png"));
	act_histequ = menu_enhance->addAction(QString::fromLocal8Bit("直方图均衡"));
	act_histequ->setIcon(QIcon(":/img/Resources/Bar Chart_64px.png"));
	act_help = menu_enhance->addAction(QString::fromLocal8Bit("帮助"));
	act_help->setIcon(QIcon(":/img/Resources/Info_64px.png"));

	menu_tool = (ui.menuBar)->addMenu(QString::fromLocal8Bit("工具"));
	act_resample = menu_tool->addAction(QString::fromLocal8Bit("重采样"));
	act_resample->setIcon(QIcon(":/img/Resources/Collapse_64px.png"));
	act_resample->setCheckable(true);
	act_resample->setChecked(false);
	act_cut = menu_tool->addAction(QString::fromLocal8Bit("裁切"));
	act_cut->setIcon(QIcon(":/img/Resources/Crop_64px.png"));
	menu_edgedetect = menu_tool->addMenu(QString::fromLocal8Bit("边缘检测"));
	menu_edgedetect->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	act_sobel = menu_edgedetect->addAction(QString::fromLocal8Bit("Sobel算子"));
	act_sobel->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	act_prewitt = menu_edgedetect->addAction(QString::fromLocal8Bit("Prewwit算子"));
	act_prewitt->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	act_laplacian = menu_edgedetect->addAction(QString::fromLocal8Bit("拉普拉斯算子"));
	act_laplacian->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	act_canny = menu_edgedetect->addAction(QString::fromLocal8Bit("Canny算子"));
	act_canny->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	act_roberts = menu_edgedetect->addAction(QString::fromLocal8Bit("Roberts算子"));
	act_roberts->setIcon(QIcon(":/img/Resources/Burglary_64px.png"));
	menu_segmentation = menu_tool->addMenu(QString::fromLocal8Bit("图像分割"));
	menu_segmentation->setIcon(QIcon(":/img/Resources/Switch_64px.png"));
	act_otsu = menu_segmentation->addAction(QString::fromLocal8Bit("大津阈值"));
	act_otsu->setIcon(QIcon(":/img/Resources/Services_64px.png"));
	act_fixed = menu_segmentation->addAction(QString::fromLocal8Bit("固定阈值"));
	act_fixed->setIcon(QIcon(":/img/Resources/Services_64px.png"));
	act_adapt = menu_segmentation->addAction(QString::fromLocal8Bit("自适应阈值"));
	act_adapt->setIcon(QIcon(":/img/Resources/Services_64px.png"));

	menu_view= (ui.menuBar)->addMenu(QString::fromLocal8Bit("视图"));
	act_grid = menu_view->addAction(QString::fromLocal8Bit("网格"));
	act_grid->setIcon(QIcon(":/img/Resources/Activity Grid_64px.png"));
	act_grid->setCheckable(true);
	act_grid->setChecked(false);
	
	menu_changedetect = (ui.menuBar)->addMenu(QString::fromLocal8Bit("变化检测"));
	act_changedetect = menu_changedetect->addAction(QString::fromLocal8Bit("变化检测"));
	act_changedetect->setIcon(QIcon(":/img/Resources/UNICEF_64px.png"));
	act_changedetect_help = menu_changedetect->addAction(QString::fromLocal8Bit("帮助"));
	act_changedetect_help->setIcon(QIcon(":/img/Resources/Info_64px.png"));

	act_confirm= menu_tool->addAction(QString::fromLocal8Bit("确认"));
	act_confirm->setIcon(QIcon(":/img/Resources/Checkmark_64px.png"));

	act_helpmain = (ui.menuBar)->addAction(QString::fromLocal8Bit("帮助"));


	ui.mainToolBar->addAction(act_open);
	ui.mainToolBar->addAction(act_save);
	ui.mainToolBar->addAction(act_grid);
	ui.mainToolBar->addAction(act_resample);
	ui.mainToolBar->addAction(act_cut);
	ui.mainToolBar->addAction(act_confirm);
	ui.mainToolBar->addAction(act_confirm);

	
	//main area init
	widget_center = new QWidget(this);
	setCentralWidget(widget_center);

	//status bar
	status = new QLabel;


	//info box
	info_origin = new ImageInfo(widget_center);
	info_after = new ImageInfo(widget_center);

	//pic view
	view_wid_origin = new QWidget(widget_center);
	view_wid_after = new QWidget(widget_center);
	view_origin = new ScrollPicView(view_wid_origin);
	view_after = new ScrollPicView(view_wid_after);
	axis_h_origin = new Axis_Horizontal(view_origin);
	axis_h_after = new Axis_Horizontal(view_wid_after);
	axis_v_origin = new Axis_Vertical(view_wid_origin);
	axis_v_after = new Axis_Vertical(view_wid_after);
	QWidget *tmp1 = new QWidget(view_wid_origin);
	tmp1->setFixedSize(30, 30);
	QWidget *tmp2 = new QWidget(view_wid_after);
	tmp2->setFixedSize(30, 30);

	QVBoxLayout *view_layout_origin = new QVBoxLayout;
	QHBoxLayout *tmplayout1 = new QHBoxLayout;
	tmplayout1->addWidget(tmp1);
	tmplayout1->addWidget(axis_h_origin);
	tmplayout1->setSpacing(0);
	QHBoxLayout *view_hlayout_origin = new QHBoxLayout;
	view_hlayout_origin->addWidget(axis_v_origin);
	view_hlayout_origin->addWidget(view_origin);
	view_hlayout_origin->setSpacing(0);
	view_layout_origin->addLayout(tmplayout1);
	view_layout_origin->addLayout(view_hlayout_origin);
	view_layout_origin->setSpacing(0);
	view_wid_origin->setLayout(view_layout_origin);

	QVBoxLayout *view_layout_after = new QVBoxLayout;
	QHBoxLayout *view_hlayout_after = new QHBoxLayout;
	QHBoxLayout *tmplayout2 = new QHBoxLayout;
	tmplayout2->addWidget(tmp2);
	tmplayout2->addWidget(axis_h_after);
	tmplayout2->setSpacing(0);
	view_hlayout_after->addWidget(axis_v_after);
	view_hlayout_after->addWidget(view_after);
	view_hlayout_after->setSpacing(0);
	view_layout_after->addLayout(tmplayout2);
	view_layout_after->addLayout(view_hlayout_after);
	view_layout_after->setSpacing(0);
	view_wid_after->setLayout(view_layout_after);

	//change detect
	dischange = new display_change;
	dischange->setWindowTitle("Change Detect");

	//controller
	displaycontroller_origin = new DisplayController(widget_center);
	displaycontroller_after = new DisplayController(widget_center);

	//rs info
	rsinfo_origin = new RSInfo(widget_center);
	rsinfo_after = new RSInfo(widget_center);

	//histogram
	hist= new Histogram;
	hist->setWindowTitle("Histogram");

	//extension
	DrawHist_origin = new QAction(QString::fromLocal8Bit("绘制直方图"), view_origin);
	DrawHist_origin->setIcon(QIcon(":/img/Resources/Bar Chart_64px.png"));
	Save_origin = new QAction(QString::fromLocal8Bit("另存为…"), view_origin);
	Save_origin->setIcon(QIcon(":/img/Resources/Save_64px.png"));
	

	Zoom_origin= new QAction(QString::fromLocal8Bit("重置视图"), view_origin);
	Zoom_origin->setIcon(QIcon(":/img/Resources/Zoom to Extents_64px.png"));

	view_origin->addAction(DrawHist_origin);
	view_origin->addAction(Save_origin);
	view_origin->addAction(Zoom_origin);
	view_origin->setContextMenuPolicy(Qt::ActionsContextMenu);


	Zoom_after = new QAction(QString::fromLocal8Bit("重置视图"), view_origin);
	Zoom_after->setIcon(QIcon(":/img/Resources/Zoom to Extents_64px.png"));

	DrawHist_after = new QAction(QString::fromLocal8Bit("绘制直方图"), view_after);
	DrawHist_after->setIcon(QIcon(":/img/Resources/Bar Chart_64px.png"));
	Save_after = new QAction(QString::fromLocal8Bit("另存为"), view_after);
	Save_after->setIcon(QIcon(":/img/Resources/Save_64px.png"));
	view_after->addAction(DrawHist_after);
	view_after->addAction(Save_after);
	view_after->addAction(Zoom_after);
	view_after->setContextMenuPolicy(Qt::ActionsContextMenu);
	
	//layout
	layout_main = new QVBoxLayout;
	layout_up = new QHBoxLayout;
	layout_down = new QHBoxLayout;

	layout_up->addWidget(view_wid_origin);
	layout_up->addWidget(view_wid_after);

	layout_down->addWidget(displaycontroller_origin);
	layout_down->addWidget(info_origin);
	layout_down->addWidget(rsinfo_origin);
	layout_down->addWidget(displaycontroller_after);
	layout_down->addWidget(info_after);
	layout_down->addWidget(rsinfo_after);
	
	layout_main->addLayout(layout_up);
	layout_main->addLayout(layout_down);
	layout_main->setStretchFactor(layout_up, 8);
	layout_main->setStretchFactor(layout_down, 2);
	widget_center->setLayout(layout_main);
	cout << "okk" << endl;
}

void RS_DEMO_GDAL::init_signal_slot()
{
	connect(act_open, SIGNAL(triggered()), this, SLOT(open()));
	connect(act_confirm, SIGNAL(triggered()), this, SLOT(confirmaction()));
	connect(displaycontroller_origin->ApplyButton, SIGNAL(clicked()), this, SLOT(updateview_origin()));
	connect(displaycontroller_after->ApplyButton, SIGNAL(clicked()), this, SLOT(updateview_after()));
	connect(DrawHist_origin, SIGNAL(triggered(bool)), this, SLOT(drawhist_origin()));
	connect(DrawHist_after, SIGNAL(triggered(bool)), this, SLOT(drawhist_after()));
	connect(Zoom_origin, SIGNAL(triggered(bool)), this, SLOT(enable_zoom_origin()));
	connect(Zoom_after, SIGNAL(triggered(bool)), this, SLOT(enable_zoom_after()));

	
	connect(act_save, SIGNAL(triggered()), this, SLOT(save_menu()));
	connect(Save_origin,SIGNAL(triggered()),this,SLOT(save_origin()));
	connect(Save_after, SIGNAL(triggered()), this, SLOT(save_after()));
	connect(act_fixed, SIGNAL(triggered()), this, SLOT(segmentation_fixed()));
	connect(view_origin->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updatecursor_origin()));
	connect(view_origin->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updatecursor_origin()));
	connect(view_after->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updatecursor_after()));
	connect(view_after->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updatecursor_after()));
	connect(act_grid, SIGNAL(triggered()), this, SLOT(setgrid()));
	connect(act_resample, SIGNAL(triggered()), this, SLOT(ImageResample()));
	connect(act_helpmain, SIGNAL(triggered()), this, SLOT(help()));

	connect(filetoolarge->resample, SIGNAL(clicked()), this, SLOT(ImageResample()));
	//connect(filetoolarge->cut, SIGNAL(clicked()), this, SLOT(ImageCut()));
	//
	connect(act_cut, SIGNAL(triggered()), this, SLOT(ImageCut()));
	
	//高
	connect(act_sobel, SIGNAL(triggered()), this, SLOT(sobel()));
	connect(act_prewitt, SIGNAL(triggered()), this, SLOT(prewitt()));
	connect(act_laplacian, SIGNAL(triggered()), this, SLOT(laplacian()));
	connect(act_canny, SIGNAL(triggered()), this, SLOT(canny()));
	connect(act_roberts, SIGNAL(triggered()), this, SLOT(roborts()));
	connect(act_otsu, SIGNAL(triggered()), this, SLOT(otsu()));
	connect(act_adapt, SIGNAL(triggered()), this, SLOT(Ostu2()));
	
	//
	connect(act_sharpen, SIGNAL(triggered()), this, SLOT(sharpen()));
	connect(act_filt_median, SIGNAL(triggered()), this, SLOT(filter_mid()));
	connect(act_filt_mean, SIGNAL(triggered()), this, SLOT(filter_mean()));
	connect(act_filt_gaussian, SIGNAL(triggered()), this, SLOT(filter_gauss()));
	connect(act_histequ, SIGNAL(triggered()), this, SLOT(histequalization()));/**/
	connect(act_help, SIGNAL(triggered()), this, SLOT(helpmean()));
	connect(act_help, SIGNAL(triggered()), this, SLOT(helphist()));
	connect(act_help, SIGNAL(triggered()), this, SLOT(helpsharpen()));

	//
	connect(act_changedetect, SIGNAL(triggered()), this, SLOT(change_detection()));
	connect(act_changedetect_help, SIGNAL(triggered()), this, SLOT(change_detection_help()));

}

void RS_DEMO_GDAL::_nBitTo8Bit(GDALDataset * src, GDALDataset * dst)
{//new:can handle n bit

	GDALDataType type = src->GetRasterBand(1)->GetRasterDataType();

	switch (type)
	{
	default: {
		cout << "unsupported datatype: " << type << endl;
		break; 
	}
	case 1: {
		typedef unsigned char DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;

		//get highest bit in src
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		cout << "highbit: " << highbit<<endl;

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					
					double dTemp;
					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}
					
					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 2: {
		typedef unsigned short DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));
		
		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 3: {
		typedef short DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 4: {
		typedef unsigned int DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 5: {
		typedef int DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 6: {
		typedef float DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	case 7: {
		typedef double DataType;
		int XSize = src->GetRasterXSize();
		int YSize = src->GetRasterYSize();
		int nBands = src->GetRasterCount();

		int size = 0;
		DataType* data = new DataType[XSize*YSize];
		DataType max = 0;
		for (int n = 1; n <= nBands; n++)
		{
			GDALRasterBand *pSrcBandImg = src->GetRasterBand(n);		//GDALDataSet->GDALRasterBand
			pSrcBandImg->RasterIO(GF_Read, 0, 0, XSize, YSize, data, XSize, YSize, type, 0, 0);
			for (int i = 0; i < XSize*YSize; i++)
			{
				if (*data>max)
					max = *data;
				data++;
			}
			data -= XSize*YSize;
		}
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		delete[]data;


		DataType *pSrcData = new DataType[XSize];//data for one line
		uchar *pDstData = new uchar[XSize];

		//dst = driver->Create("a", XSize, YSize, nBands, GDT_Byte, NULL);

		double highbit = double(pow(2, size));

		//for every band
		for (int iBand = 1; iBand <= nBands; iBand++)
		{
			GDALRasterBand *pSrcBand = src->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = dst->GetRasterBand(iBand);

			for (int i = 0; i<YSize; i++)  //for every row 
			{
				//get data for one line 
				pSrcBand->RasterIO(GF_Read, 0, i, XSize, 1, pSrcData, XSize, 1, type, 0, 0);

				//highbit to 8bit,for every pixel  
				for (int j = 0; j<XSize; j++)
				{
					double dTemp ;

					if (highbit <= 255) {
						dTemp = pSrcData[j];
					}
					else {
						dTemp = pSrcData[j] * 255.0 / highbit;
					}

					if (dTemp > 255.0)
						pDstData[j] = 255;
					else
						pDstData[j] = (uchar)dTemp;
					//cout << int(pDstData[j]) << ' ';
				}
				pDstBand->RasterIO(GF_Write, 0, i, XSize, 1, pDstData, XSize, 1, GDT_Byte, 0, 0);
			}
		}

		delete[]pDstData;
		delete[]pSrcData;
		break; }
	}
}

int RS_DEMO_GDAL::getGISinfo(GDALDataset *src, double *dst)
{
	//dst:[lon_start,lat_start,lon_end,lat_end]
	const char* projRef = NULL; 
	projRef= src->GetProjectionRef();
	OGRSpatialReference fRef, tRef;
	double Geo[6];
	
	if (projRef && (src->GetGeoTransform(Geo) == CE_None)) {
		char *tmp = new char[strlen(projRef) + 1];
		strcpy_s(tmp, strlen(projRef) + 1, projRef);
		fRef.importFromWkt(&tmp);
		tRef.SetWellKnownGeogCS("WGS84");

		OGRCoordinateTransformation *coordTrans = NULL;
		
		coordTrans = OGRCreateCoordinateTransformation(&fRef, &tRef);
		
		if (!coordTrans) {
			return -1;
		}
		else {
			*(dst) = Geo[0];
			*(dst+1) = Geo[3];
			*(dst + 2) = Geo[0] + src->GetRasterXSize() * Geo[1];
			*(dst + 3) = Geo[3] + src->GetRasterXSize() * Geo[5];

			coordTrans->Transform(1, dst, dst + 1);
			coordTrans->Transform(1, dst + 2, dst + 3);

			return 0;
		}
	}
	else
	{
		return -1;
	}
}

void RS_DEMO_GDAL::GDALtoQImage()
{
	//this function converts GDALDataSet to a RGB QImage
	//this QImage will then be used to display
	//this function should be able to handle diffrent datatypes
	//if GDALDataSet is too large,this function should only return a part of original dataset
	//check this->SetViewFocus to determin which dataset to use(available pData or pDataSrc)
	//check this->SetViewFocus to determin which QImage will be used to save the result
	//(available image_origin or image_after)
	GDALDataset *temp;
	GDALDataType type;
	int XSize, YSize, nBands;
	int *resolution = new int[2];
	switch (SetViewFocus)
	{
	case 0: {
		type = pDataSrc->GetRasterBand(1)->GetRasterDataType();
		XSize = pDataSrc->GetRasterXSize();
		YSize = pDataSrc->GetRasterYSize();
		nBands = pDataSrc->GetRasterCount();
		GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
		temp = driver->Create(".temp", XSize, YSize, nBands, GDT_Byte, NULL);
		_nBitTo8Bit(pDataSrc, temp);
		cout << "view: 0, converting..."<<endl;
		getre(resolution,pDataSrc);
		break; 
	}
	case 1: {
		type = pData->GetRasterBand(1)->GetRasterDataType();
		XSize = pData->GetRasterXSize();
		YSize = pData->GetRasterYSize();
		nBands = pData->GetRasterCount();
		GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
		temp = driver->Create(".temp", XSize, YSize, nBands, GDT_Byte, NULL);
		cout << "view: 1, converting..."<<endl;
		_nBitTo8Bit(pData, temp);
		getre(resolution, pData);
		break; }
	}

	vector<Mat> img_list;

	GDALRasterBand *r = NULL;
	GDALRasterBand *g = NULL;
	GDALRasterBand *b = NULL;

	switch (SetViewFocus)
	{
	case 0:{
		r = temp->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
		g = temp->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
		b = temp->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
		break; }
	case 1: {
		r = temp->GetRasterBand(displaycontroller_after->RSelector->currentIndex() + 1);
		g = temp->GetRasterBand(displaycontroller_after->GSelector->currentIndex() + 1);
		b = temp->GetRasterBand(displaycontroller_after->BSelector->currentIndex() + 1);
	}
	}
	
	
	Mat r_mat(*(resolution+1), *(resolution), CV_8UC1);
	Mat g_mat(*(resolution + 1), *(resolution), CV_8UC1);
	Mat b_mat(*(resolution + 1), *(resolution), CV_8UC1);

	cout << "xx:" << XSize << " yy:" << YSize << endl;


	r->RasterIO(GF_Read, 0, 0, XSize, YSize, r_mat.data, *(resolution), *(resolution + 1), GDT_Byte, 0, 0);
	g->RasterIO(GF_Read, 0, 0, XSize, YSize, g_mat.data, *(resolution), *(resolution + 1), GDT_Byte, 0, 0);
	b->RasterIO(GF_Read, 0, 0, XSize, YSize, b_mat.data, *(resolution), *(resolution + 1), GDT_Byte, 0, 0);

	img_list.push_back(r_mat);
	img_list.push_back(g_mat);
	img_list.push_back(b_mat);

	cv::merge(img_list, image);

	switch (SetViewFocus)
	{
	case 0: {
		img_origin = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
		cout << "making QImage using pDataSrc" << endl;
		break; }
	case 1: {
		img_after = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
		cout << "making QImage using pData" << endl;
		break; }
	}
	

	r_mat.release();
	b_mat.release();
	g_mat.release();
	vector<Mat>().swap(img_list);
	delete[]resolution;
	temp->Release();
	remove(".temp");
}

void RS_DEMO_GDAL::drawhist()
{
	clock_t  clockBegin, clockEnd;
	clockBegin = clock();
	if (SetViewFocus == 0) {
		if (pDataSrc) {
			hist->src = pDataSrc;
			hist->init_combo();
			hist->hide();
			hist->show();
		}
		else {
			errordlg->setWindowTitle(QString::fromLocal8Bit("错误!"));
			errordlg->showMessage(QString::fromLocal8Bit("未加载图像"));
		}
	}
	else
	{
		if (pData) {
			hist->src = pData;
			hist->init_combo();
			hist->hide();
			hist->show();
		}
		else {
			errordlg->setWindowTitle(QString::fromLocal8Bit("错误!"));
			errordlg->showMessage(QString::fromLocal8Bit("未加载图像"));
		}
	}
	clockEnd = clock();
	cout << "绘制直方图用时：" << clockEnd - clockBegin << endl;
}

void RS_DEMO_GDAL::updateinfo()
{
	if (SetViewFocus == 0) {
		int xsize = pDataSrc->GetRasterXSize();
		int ysize = pDataSrc->GetRasterYSize();
		string type = pDataSrc->GetDriverName();
		int bands = pDataSrc->GetRasterCount();
		string datatype = GDALGetDataTypeName(pDataSrc->GetRasterBand(1)->GetRasterDataType());
		
		info_origin->xsize->setText(QString::number(xsize));
		info_origin->ysize->setText(QString::number(ysize));
		info_origin->imagetype->setText(QString::fromStdString(type));
		info_origin->bands->setText(QString::number(bands));
		info_origin->datatype->setText(QString::fromStdString(datatype));

		double *geoinfo = new double[4];

		if (getGISinfo(pDataSrc,geoinfo)==0) {
			rsinfo_origin->longitude_start->
				setText(QString::number(abs(*(geoinfo))) + QString::fromLocal8Bit("°") + ((*(geoinfo) > 0) ? ('E') : ('W')));
			rsinfo_origin->latitude_start->
				setText(QString::number(abs(*(geoinfo + 1))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 1) > 0) ? ('N') : ('S')));
			rsinfo_origin->longitude_stop->
				setText(QString::number(abs(*(geoinfo + 2))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 2) > 0) ? ('E') : ('W')));
			rsinfo_origin->latitude_stop->
				setText(QString::number(abs(*(geoinfo + 3))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 3) > 0) ? ('N') : ('S')));
		}
		else {
			rsinfo_origin->longitude_start->setText("NULL");
			rsinfo_origin->latitude_start->setText("NULL");
			rsinfo_origin->longitude_stop->setText("NULL");
			rsinfo_origin->latitude_stop->setText("NULL");
		}
		delete[]geoinfo;
	}
	else {
		int xsize = pData->GetRasterXSize();
		int ysize = pData->GetRasterYSize();
		string type = pData->GetDriverName();
		int bands = pData->GetRasterCount();
		string datatype = GDALGetDataTypeName(pData->GetRasterBand(1)->GetRasterDataType());

		info_after->xsize->setText(QString::number(xsize));
		info_after->ysize->setText(QString::number(ysize));
		info_after->imagetype->setText(QString::fromStdString(type));
		info_after->bands->setText(QString::number(bands));
		info_after->datatype->setText(QString::fromStdString(datatype));

		double *geoinfo = new double[4];

		if (getGISinfo(pData, geoinfo) == 0) {
			rsinfo_after->longitude_start->
				setText(QString::number(abs(*(geoinfo))) + QString::fromLocal8Bit("°") + ((*(geoinfo) > 0) ? ('E') : ('W')));
			rsinfo_after->latitude_start->
				setText(QString::number(abs(*(geoinfo + 1))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 1) > 0) ? ('N') : ('S')));
			rsinfo_after->longitude_stop->
				setText(QString::number(abs(*(geoinfo + 2))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 2) > 0) ? ('E') : ('W')));
			rsinfo_after->latitude_stop->
				setText(QString::number(abs(*(geoinfo + 3))) + QString::fromLocal8Bit("°") + ((*(geoinfo + 3) > 0) ? ('N') : ('S')));
		}
		else {
			rsinfo_after->longitude_start->setText("NULL");
			rsinfo_after->latitude_start->setText("NULL");
			rsinfo_after->longitude_stop->setText("NULL");
			rsinfo_after->latitude_stop->setText("NULL");
		}
		delete[]geoinfo;
	}
}

void RS_DEMO_GDAL::updatecontroller()
{
	if (SetViewFocus == 0) {
		//obtain image info from pDataSrc, update display controller
		//clean up current items
		displaycontroller_origin->RSelector->clear();
		displaycontroller_origin->GSelector->clear();
		displaycontroller_origin->BSelector->clear();

		int nBands = pDataSrc->GetRasterCount();
		for (int i = 0; i < nBands; ++i) {
			GDALRasterBand *pBand = pDataSrc->GetRasterBand(i + 1);
			int min, max;
			double adfMinMax[2];
			adfMinMax[0] = pBand->GetMinimum(&min);
			adfMinMax[1] = pBand->GetMaximum(&max);
			if (min&&max) {
				string banddescription;

				banddescription = "波段 " + to_string(i + 1) + ": " + '[' + to_string(adfMinMax[0]) + ','
					+ to_string(adfMinMax[1]) + ']';
				displaycontroller_origin->RSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_origin->GSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_origin->BSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
			}
			else {
				string banddescription;
				banddescription = "波段 " + to_string(i + 1);
				displaycontroller_origin->RSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_origin->GSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_origin->BSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
			}
		}
		if (nBands >= 3) {
			displaycontroller_origin->RSelector->setCurrentIndex(0);
			displaycontroller_origin->GSelector->setCurrentIndex(1);
			displaycontroller_origin->BSelector->setCurrentIndex(2);
		}
		updateview_origin();
		displaycontroller_origin->show();
	}
	else {
		//obtain image info from pData, update display controller
		//clean up current items
		displaycontroller_after->RSelector->clear();
		displaycontroller_after->GSelector->clear();
		displaycontroller_after->BSelector->clear();

		int nBands = pData->GetRasterCount();
		for (int i = 0; i < nBands; ++i) {
			GDALRasterBand *pBand = pData->GetRasterBand(i + 1);
			int min, max;
			double adfMinMax[2];
			adfMinMax[0] = pBand->GetMinimum(&min);
			adfMinMax[1] = pBand->GetMaximum(&max);
			if (min&&max) {
				string banddescription;

				banddescription = "波段 " + to_string(i + 1) + ": " + '[' + to_string(adfMinMax[0]) + ','
					+ to_string(adfMinMax[1]) + ']';
				displaycontroller_after->RSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_after->GSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_after->BSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
			}
			else {
				string banddescription;
				banddescription = "波段 " + to_string(i + 1);
				displaycontroller_after->RSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_after->GSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
				displaycontroller_after->BSelector->addItem(QString::fromLocal8Bit(banddescription.c_str()));
			}
		}
		if (nBands >= 3) {
			displaycontroller_after->RSelector->setCurrentIndex(0);
			displaycontroller_after->GSelector->setCurrentIndex(1);
			displaycontroller_after->BSelector->setCurrentIndex(2);
		}
		updateview_after();
		displaycontroller_after->show();
	}
}

void RS_DEMO_GDAL::confirmaction()
{
	cout << "start transaction" << endl;

	clock_t  clockBegin, clockEnd;
	clockBegin = clock();
	if (!pData || !pDataSrc) {
		return;
	}
	
	if (pDataSrc) {
		pDataSrc->Release();
		remove("src_copy.temp");
	}

	GDALDataType type = pData->GetRasterBand(1)->GetRasterDataType();
	int XSize = pData->GetRasterXSize();
	int YSize = pData->GetRasterYSize();
	int nBands = pData->GetRasterCount();
	GDALDriver *newdriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	pDataSrc = newdriver->CreateCopy("src_copy.temp", pData, 0, 0, 0, 0);
	SetViewFocus = 0;

	updatecontroller();//update image display controller
	//cout << "update controller ok" << endl;	//to determin display method
	updateinfo();
	//cout << "update info ok" << endl;
	updatecursor();
	//cout << "update coursor ok" << endl;
	//cout << "transaction ended" << endl;
	clockEnd = clock();
	cout << "确认操作用时：" << clockEnd - clockBegin << endl;
}


void RS_DEMO_GDAL::setgrid()
{
	if (pData && pDataSrc) {
		switch (act_grid->isChecked())
		{
		case true: {
			view_origin->grid = true;
			view_after->grid = true;
			updateview_origin();
			updateview_after();
			break; }
		case false: {
			view_origin->grid = false;
			view_after->grid = false;
			updateview_origin();
			updateview_after();
		}
		}
	}
	else {
		QMessageBox::warning(this, QString::fromLocal8Bit("图片还没有读入"),
			QString::fromLocal8Bit("图片还没有读入"));
	}
}

void RS_DEMO_GDAL::save()
{
	if (SetViewFocus == 0)
	{
		if (pDataSrc == 0)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("图片还没有读入"),
				QString::fromLocal8Bit("图片还没有读入"));
			return;
		}
		else
		{
			QString path = QFileDialog::getSaveFileName(this,
				QString::fromLocal8Bit("保存"),
				".",
				tr("Image Files (*.jpg);;Image Files(*.bmp);;Image Files(*.png);; Image Files(*.tif);;All Files(*.*)"));
			//获取原图像的长宽和数据类型
			int XSize = pDataSrc->GetRasterXSize();
			int YSize = pDataSrc->GetRasterYSize();
			GDALDataType type = pDataSrc->GetRasterBand(1)->GetRasterDataType();
			//
			if (!path.isEmpty())
			{

				CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//为了支持中文路径

				QByteArray pathstring = path.toLocal8Bit();
				//将路径转换成字符串
				char *pszRasterFile = pathstring.data();
				char *dstExtension = strlwr(strrchr(pszRasterFile, '.') + 1);//截取字符串"."之后的文件格式
				char *Gtype = NULL;
				//根据保存文件名称获取相应的驱动格式
				if (0 == strcmp(dstExtension, "bmp")) Gtype = "BMP";
				else if (0 == strcmp(dstExtension, "jpg")) Gtype = "JPEG";
				else if (0 == strcmp(dstExtension, "png")) Gtype = "PNG";
				else if (0 == strcmp(dstExtension, "tif")) Gtype = "GTiff";
				else if (0 == strcmp(dstExtension, "gif")) Gtype = "GIF";
				else Gtype = NULL;
				if (Gtype == NULL)
				{
					QMessageBox::warning(this, tr("Sorry"),
						QString::fromLocal8Bit("暂不支持这种格式文件的保存"));
					return;
				}
				else if (type!=GDT_Byte&&type!=GDT_UInt16) 
				{
					QMessageBox::warning(this, tr("Sorry"),
						QString::fromLocal8Bit("暂不支持这种格式文件的保存"));
					return;
				}
				else
				{
					//波段数小于3可以以任何格式存储
					if (pDataSrc->GetRasterCount() <= 3)
					{
						if (Gtype == "BMP" || Gtype == "JPEG")
						{
							if (type == GDT_Byte)
							{
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, pDataSrc, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pDataSrc->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pDataSrc, temp);
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, temp, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
								temp->Release();
							}
						}
						else
						{
							//PNG和tiff支持的位数比较高
							GDALDataset *poDstDS;
							GDALDriver *pDstDriver = NULL;
							pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
							poDstDS = pDstDriver->CreateCopy(pszRasterFile, pDataSrc, FALSE, NULL, NULL, NULL);
							poDstDS->Release();
						}

					}
					else
					{
						if (Gtype == "PNG")
						{
							//最多可以保存4个通道
							cout << pDataSrc->GetRasterCount();
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 4, GDT_Byte, NULL);
								cout << type;

								for (int n = 1; n < 5; n++)
								{
									GDALRasterBand *pBand = t->GetRasterBand(n);

									GDALRasterBand *pSrcBandImg = pDataSrc->GetRasterBand(n);
									double *lineData = new double[XSize];
									for (int i = 0; i < YSize; i++)
									{

										pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
										pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									}
									delete[]lineData;
								}
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								t = driver->Create(".t", XSize, YSize, 4, GDT_UInt16, NULL);
								cout << type;

								for (int n = 1; n < 5; n++)
								{
									GDALRasterBand *pBand = t->GetRasterBand(n);

									GDALRasterBand *pSrcBandImg = pDataSrc->GetRasterBand(n);
									double *lineData = new double[XSize];
									for (int i = 0; i < YSize; i++)
									{

										pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
										pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									}
									delete[]lineData;
								}
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							GDALClose(t);
						}//成功
						else if (Gtype == "JPEG")
						{
							cout << pDataSrc->GetRasterCount();
							//波段数大于3，只取三个波段进行存储
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pDataSrc->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pDataSrc, temp);
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								delete[]lineData;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								temp->Release();
							}
							GDALClose(t);
						}
						else if (Gtype == "BMP")
						{
							cout << pDataSrc->GetRasterCount();
							//波段数大于3，只取三个波段进行存储
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pDataSrc->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pDataSrc, temp);
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
								temp->Release();
							}
							GDALClose(t);
						}
						//GTiff格式
						else
						{
							GDALDataset *poDstDS;
							GDALDriver *pDstDriver = NULL;
							pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
							poDstDS = pDstDriver->CreateCopy(pszRasterFile, pDataSrc, FALSE, NULL, NULL, NULL);
							poDstDS->Release();
						}


					}

				}

			}
			else
			{
				QMessageBox::warning(this, tr("Sorry"),
					QString::fromLocal8Bit("文件路径为空"));
				return;
			}
		}
	}
	else
	{
		if (pData == 0)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("图片还没有读入"),
				QString::fromLocal8Bit("图片还没有读入"));
			return;
		}
		else
		{
			QString path = QFileDialog::getSaveFileName(this,
				QString::fromLocal8Bit("保存"),
				".",
				tr("Image Files (*.jpg);;Image Files(*.bmp);;Image Files(*.png);; Image Files(*.tif);;All Files(*.*)"));
			//获取原图像的长宽和数据类型
			int XSize = pData->GetRasterXSize();
			int YSize = pData->GetRasterYSize();
			GDALDataType type = pData->GetRasterBand(1)->GetRasterDataType();
			//
			if (!path.isEmpty())
			{

				CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//为了支持中文路径
				QByteArray pathstring = path.toLocal8Bit();
				char *pszRasterFile = pathstring.data();//将路径转换成字符串


				char *dstExtension = strlwr(strrchr(pszRasterFile, '.') + 1);//截取字符串"."之后的文件格式
				char *Gtype = NULL;
				//根据保存文件名称获取相应的驱动格式
				if (0 == strcmp(dstExtension, "bmp")) Gtype = "BMP";
				else if (0 == strcmp(dstExtension, "jpg")) Gtype = "JPEG";
				else if (0 == strcmp(dstExtension, "png")) Gtype = "PNG";
				else if (0 == strcmp(dstExtension, "tif")) Gtype = "GTiff";
				else if (0 == strcmp(dstExtension, "gif")) Gtype = "GIF";
				else Gtype = NULL;
				if (Gtype == NULL)
				{
					QMessageBox::warning(this, tr("Sorry"),
						QString::fromLocal8Bit("暂不支持这种格式文件的保存"));
					return;
				}
				else if (type != GDT_Byte&&type != GDT_UInt16) 
				{
					QMessageBox::warning(this, tr("Sorry"),
						QString::fromLocal8Bit("暂不支持这种格式文件的保存"));
					return;
				}
				else
				{
					//波段数小于3可以以任何格式存储
					if (pDataSrc->GetRasterCount() <= 3)
					{
						if (Gtype == "BMP" || Gtype == "JPEG")
						{
							if (type == GDT_Byte)
							{
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, pData, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pData->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pData, temp);
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, temp, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
								temp->Release();
							}
						}
						else
						{
							//PNG和tiff支持的位数比较高
							GDALDataset *poDstDS;
							GDALDriver *pDstDriver = NULL;
							pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
							poDstDS = pDstDriver->CreateCopy(pszRasterFile, pData, FALSE, NULL, NULL, NULL);
							poDstDS->Release();
						}

					}
					else
					{
						if (Gtype == "PNG")
						{
							//最多可以保存4个通道
							cout << pData->GetRasterCount();
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 4, GDT_Byte, NULL);
								cout << type;

								for (int n = 1; n < 5; n++)
								{
									GDALRasterBand *pBand = t->GetRasterBand(n);

									GDALRasterBand *pSrcBandImg = pData->GetRasterBand(n);
									double *lineData = new double[XSize];
									for (int i = 0; i < YSize; i++)
									{

										pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
										pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									}
									delete[]lineData;
								}
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								t = driver->Create(".t", XSize, YSize, 4, GDT_UInt16, NULL);
								cout << type;

								for (int n = 1; n < 5; n++)
								{
									GDALRasterBand *pBand = t->GetRasterBand(n);

									GDALRasterBand *pSrcBandImg = pData->GetRasterBand(n);
									double *lineData = new double[XSize];
									for (int i = 0; i < YSize; i++)
									{

										pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
										pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									}
									delete[]lineData;
								}
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							GDALClose(t);
						}//成功
						else if (Gtype == "JPEG")
						{
							cout << pDataSrc->GetRasterCount();
							//波段数大于3，只取三个波段进行存储
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = pData->GetRasterBand(displaycontroller_origin->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = pData->GetRasterBand(displaycontroller_origin->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = pData->GetRasterBand(displaycontroller_origin->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pData->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pData, temp);
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = temp->GetRasterBand(displaycontroller_after->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_after->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_after->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								delete[]lineData;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								temp->Release();
							}
							GDALClose(t);
						}
						else if (Gtype == "BMP")
						{
							cout << pDataSrc->GetRasterCount();
							//波段数大于3，只取三个波段进行存储
							//创建
							GDALDataset *t = NULL;
							//只能以GTiff格式创建，所以此处只是把原图创建到3通道的GTiff
							GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
							if (type == GDT_Byte)
							{
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = pData->GetRasterBand(displaycontroller_after->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_after->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = pDataSrc->GetRasterBand(displaycontroller_after->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, type, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
							}
							else
							{
								QMessageBox::warning(this, tr("Warning!"),
									QString::fromLocal8Bit("数据类型发生变化"));
								GDALDataset *temp;
								GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
								temp = driver->Create(".temp", XSize, YSize, pData->GetRasterCount(), GDT_Byte, NULL);
								_nBitTo8Bit(pData, temp);
								t = driver->Create(".t", XSize, YSize, 3, GDT_Byte, NULL);
								cout << type;

								//第一通道R通道
								GDALRasterBand *pBand = t->GetRasterBand(1);
								GDALRasterBand *pSrcBandImg = temp->GetRasterBand(displaycontroller_after->RSelector->currentIndex() + 1);
								double *lineData = new double[XSize];
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第二通道G通道
								pBand = t->GetRasterBand(2);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_after->GSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								//第三通道B通道
								pBand = t->GetRasterBand(3);
								pSrcBandImg = temp->GetRasterBand(displaycontroller_after->BSelector->currentIndex() + 1);
								for (int i = 0; i < YSize; i++)
								{

									pSrcBandImg->RasterIO(GF_Read, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
									pBand->RasterIO(GF_Write, 0, i, XSize, 1, lineData, XSize, 1, GDT_Byte, 0, 0);
								}
								delete[]lineData;
								GDALDataset *poDstDS;
								GDALDriver *pDstDriver = NULL;
								pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
								poDstDS = pDstDriver->CreateCopy(pszRasterFile, t, FALSE, NULL, NULL, NULL);
								poDstDS->Release();
								temp->Release();
							}
							GDALClose(t);
						}
						//GTiff格式
						else
						{
							GDALDataset *poDstDS;
							GDALDriver *pDstDriver = NULL;
							pDstDriver = (GDALDriver *)GDALGetDriverByName(Gtype);
							poDstDS = pDstDriver->CreateCopy(pszRasterFile, pData, FALSE, NULL, NULL, NULL);
							poDstDS->Release();
						}


					}

				}

			}
			else
			{
				QMessageBox::warning(this, tr("Sorry"),
					QString::fromLocal8Bit("文件路径为空"));
				return;
			}
		}

	}
}

void RS_DEMO_GDAL::getre(int * out, GDALDataset *src)
{
	switch (SetViewFocus)
	{
	case 0:
	{
		*(out) = src->GetRasterXSize()*ratio_origin[0];
		*(out + 1) = src->GetRasterYSize()*ratio_origin[1];
		break;
	}
	case 1:
	{
		*(out) = src->GetRasterXSize()*ratio_after[0];
		*(out + 1) = src->GetRasterYSize()*ratio_after[1];
		break;
	}
	}
}

void RS_DEMO_GDAL::updateview_after()
{
	if (!pData) return;
	SetViewFocus = 1;
	cout << "update after" << endl;
	GDALtoQImage();
	cout << "making qimage ok" << endl;
	view_after->updateview(&img_after);
	updateinfo();
	updatecursor_after();
}

void RS_DEMO_GDAL::drawhist_after()
{
	SetViewFocus = 1;
	drawhist();
}

void RS_DEMO_GDAL::save_origin()
{
	SetViewFocus = 0;
	save();
}

void RS_DEMO_GDAL::save_after()
{
	cout << "ok" << endl;
	SetViewFocus = 1;
	save();
}

void RS_DEMO_GDAL::save_menu()
{
	SetViewFocus = 1;
	save();
}

void RS_DEMO_GDAL::segmentation_fixed()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();
		bool ok;
		int Threshold;
		cout << "tresh: " << Threshold << endl;

		for (int k = 1; k <= bandcount; k++) {

			pBand = pDataSrc->GetRasterBand(k);

			///////no problem//////
			uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
			uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
			unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
			unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];

			///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				if (k == 1)
					Threshold = QInputDialog::getInt(this, QStringLiteral("阈值"), QStringLiteral("請輸入阈值(0——65535)："), 100, 0, 65535, 1, &ok);
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);


				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp[i*nImgSizeX + j] > Threshold) {
							Temp[i*nImgSizeX + j] = -1;
						}
						else {
							Temp[i*nImgSizeX + j] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[]Temp;

				delete[]Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				if (k == 1)
					Threshold = QInputDialog::getInt(this, QStringLiteral("阈值"), QStringLiteral("請輸入阈值(0——255)："), 100, 0, 255, 1, &ok);
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);

				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp2[i*nImgSizeX + j] > Threshold) {
							Temp2[i*nImgSizeX + j] = -1;
						}
						else {
							Temp2[i*nImgSizeX + j] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[]Temp2;
				delete[]Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}


		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::updatecursor_origin()
{
	SetViewFocus = 0;
	updatecursor();
}

void RS_DEMO_GDAL::updatecursor_after()
{
	SetViewFocus = 1;
	updatecursor();
}

void RS_DEMO_GDAL::ImageResample()
{
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("图片还没有读入"),
			QString::fromLocal8Bit("图片还没有读入"));
		return;
	}
	filetoolarge->close();
	/* 定义缓存类型 */
	typedef unsigned char  DT_8U;
	typedef unsigned short DT_16U;
	//GDALAllRegister();
	GDALDataType eDT = pDataSrc->GetRasterBand(1)->GetRasterDataType();//返回当前波段中的数据类型
	int iBandCount = pDataSrc->GetRasterCount();//返回波段数
	int iSrcWidth = pDataSrc->GetRasterXSize();
	int iSrcHeight = pDataSrc->GetRasterYSize();

	
	double x_ratio = 1.0;
	double y_ratio = 1.0;//采样率

						 //对话框输入
	resampleinput *reinput = new resampleinput(this);

	reinput->exec();

	//pData = NULL;

	cout << "ok1" << endl;
	x_ratio = reinput->getdoublex();
	y_ratio = reinput->getdoubley();
	
	if (x_ratio <= 0 || y_ratio <= 0|| x_ratio >10|| y_ratio >10)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("采样率超出范围"),
			QString::fromLocal8Bit("采样率不能小于零也不能大于十"));
		return;
	}

	/*ratio_after[0] = x_ratio;
	ratio_after[1] = y_ratio;*/

	int iDstWidth = static_cast<int>(iSrcWidth*x_ratio + 0.5); cout << "get width ok" << iDstWidth << endl;
	int iDstHeight = static_cast<int>(iSrcHeight*y_ratio + 0.5); cout << "get height ok" << iDstHeight << endl;
	pData->Release();
	
	GDALDriver *newdriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	pData = newdriver->Create("resample.temp", iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	cout << "create temp ok" << endl;
	//地理信息
	double adfGeoTransform[6] = { 0, 1, 0, 0, 0, -1 };
	pDataSrc->GetGeoTransform(adfGeoTransform);
	adfGeoTransform[1] = adfGeoTransform[1] / x_ratio;
	adfGeoTransform[5] = adfGeoTransform[5] / y_ratio;

	pData->SetGeoTransform(adfGeoTransform);
	pData->SetProjection(pDataSrc->GetProjectionRef());

	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
	{
		pBandMap[i] = i + 1;
	}

	//处理8位图像
	if (eDT == GDT_Byte)
	{
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];
		pDataSrc->RasterIO(GF_Read, 0, 0, iSrcWidth, iSrcHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pData->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		delete[]pDataBuff;
	}
	//处理16位图像
	else
	{
		DT_16U *pDataBuff = new DT_16U[iDstWidth*iDstHeight*iBandCount];
		pDataSrc->RasterIO(GF_Read, 0, 0, iSrcWidth, iSrcHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pData->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		delete[]pDataBuff;
	}

	delete[]pBandMap;
	//更新视图及信息
	SetViewFocus = 1;
	updatecontroller();
	updateinfo();
	SetViewFocus = 0;
}

void RS_DEMO_GDAL::changedetect()
{
	resampleinput *reinput = new resampleinput(this);
	reinput->exec();
}

void RS_DEMO_GDAL::sobel()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();


		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);


			int Threshold;
			long sum = 0;
			long area = 0;
			long long sum_square = 0;
			long mean = 0;//均值
			long sigma = 0;//方差


						   ///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);



				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_16UC1);

				//sobel算子
				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<_int16>(i, j) = 0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
							2 * Temp[(j)*nImgSizeX + i - 1] +
							Temp[(j + 1)*nImgSizeX + i - 1] -
							Temp[(j - 1)*nImgSizeX + i + 1] -
							2 * Temp[(j)*nImgSizeX + i + 1] -
							Temp[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
								2 * Temp[(j - 1)*nImgSizeX + i] +
								Temp[(j - 1)*nImgSizeX + i + 1] -
								Temp[(j + 1)*nImgSizeX + i - 1] -
								2 * Temp[(j + 1)*nImgSizeX + i] -
								Temp[(j + 1)*nImgSizeX + i + 1]);
					}
				}




				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp1[j*nImgSizeX + i] = Img.at<__int16>(i, j);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum += Temp1[j*nImgSizeX + i];
							area++;

						}
					}
				}
				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum_square += ((Temp1[j*nImgSizeX + i] - mean)*(Temp1[j*nImgSizeX + i] - mean));
							if (i == 1) {
								printf("该点像素值为%d,sum_square=%ld\n", Temp1[j*nImgSizeX + i], sum_square);
							}


						}
					}
				}

				sigma = sqrt(sum_square / area);
				printf("%lld", sigma);

				Threshold = mean + 0.5*sigma;


				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp1[j*nImgSizeX + i] > Threshold) {
							Temp1[j*nImgSizeX + i] = 4095;
						}
						else {
							Temp1[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {

						Img.at<uchar>(i, j) = Temp2[j*nImgSizeX + i];
					}
				}


				//sobel算子
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<uchar>(i, j) = 0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
							2 * Temp2[(j)*nImgSizeX + i - 1] +
							Temp2[(j + 1)*nImgSizeX + i - 1] -
							2 * Temp2[(j - 1)*nImgSizeX + i + 1] -
							Temp2[(j)*nImgSizeX + i + 1] -
							Temp2[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
								2 * Temp2[(j - 1)*nImgSizeX + i] +
								Temp2[(j - 1)*nImgSizeX + i + 1] -
								Temp2[(j + 1)*nImgSizeX + i - 1] -
								2 * Temp2[(j + 1)*nImgSizeX + i] -
								Temp2[(j + 1)*nImgSizeX + i + 1]);
					}
				}

				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum += Temp3[j*nImgSizeX + i];
							area++;
						}
					}
				}

				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum_square += (Temp3[j*nImgSizeX + i] - mean)*(Temp3[j*nImgSizeX + i] - mean);
							if (sum_square < 0) {
								printf("%lld\n", sum_square);
							}
						}
					}
				}

				sigma = sqrt(sum_square / area);
				Threshold = mean;


				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp3[j*nImgSizeX + i] > Threshold) {
							Temp3[j*nImgSizeX + i] = -1;
						}
						else {
							Temp3[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}

		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::prewitt()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else
	{
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();


		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);

			int Threshold;
			long sum = 0;
			long area = 0;
			long long sum_square = 0;
			long mean = 0;//均值
			long sigma = 0;//方差


						   ///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);



				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_16UC1);

				//prewitt算子
				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<_int16>(i, j) = 0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
							Temp[(j)*nImgSizeX + i - 1] +
							Temp[(j + 1)*nImgSizeX + i - 1] -
							Temp[(j - 1)*nImgSizeX + i + 1] -
							Temp[(j)*nImgSizeX + i + 1] -
							Temp[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
								Temp[(j - 1)*nImgSizeX + i] +
								Temp[(j - 1)*nImgSizeX + i + 1] -
								Temp[(j + 1)*nImgSizeX + i - 1] -
								Temp[(j + 1)*nImgSizeX + i] -
								Temp[(j + 1)*nImgSizeX + i + 1]);
					}
				}




				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp1[j*nImgSizeX + i] = Img.at<ushort>(i, j);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum += Temp1[j*nImgSizeX + i];
							area++;

						}
					}
				}
				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum_square += ((Temp1[j*nImgSizeX + i] - mean)*(Temp1[j*nImgSizeX + i] - mean));
							if (i == 1) {
								printf("该点像素值为%d,sum_square=%ld\n", Temp1[j*nImgSizeX + i], sum_square);
							}


						}
					}
				}

				sigma = sqrt(sum_square / area);
				printf("%lld", sigma);

				Threshold = mean + 0.5*sigma;


				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp1[j*nImgSizeX + i] > Threshold) {
							Temp1[j*nImgSizeX + i] = 4095;
						}
						else {
							Temp1[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {

				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {

						Img.at<uchar>(i, j) = Temp2[j*nImgSizeX + i];
					}
				}


				//prewitt算子
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<uchar>(i, j) = 0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
							Temp2[(j)*nImgSizeX + i - 1] +
							Temp2[(j + 1)*nImgSizeX + i - 1] -
							Temp2[(j - 1)*nImgSizeX + i + 1] -
							Temp2[(j)*nImgSizeX + i + 1] -
							Temp2[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
								Temp2[(j - 1)*nImgSizeX + i] +
								Temp2[(j - 1)*nImgSizeX + i + 1] -
								Temp2[(j + 1)*nImgSizeX + i - 1] -
								Temp2[(j + 1)*nImgSizeX + i] -
								Temp2[(j + 1)*nImgSizeX + i + 1]);
					}
				}

				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum += Temp3[j*nImgSizeX + i];
							area++;
						}
					}
				}

				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum_square += (Temp3[j*nImgSizeX + i] - mean)*(Temp3[j*nImgSizeX + i] - mean);
							if (sum_square < 0) {
								printf("%lld\n", sum_square);
							}
						}
					}
				}

				sigma = sqrt(sum_square / area);

				Threshold = mean;


				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp3[j*nImgSizeX + i] > Threshold) {
							Temp3[j*nImgSizeX + i] = -1;
						}
						else {
							Temp3[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}
		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::canny()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();


		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);


			int Threshold1, Threshold2;





			///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);



				Mat Img, Img1;
				Img.create(nImgSizeX, nImgSizeY, CV_16UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {

						Img.at<_int16>(i, j) = Temp[j*nImgSizeX + i];
					}
				}


				//Canny算子
				//深度转换

				Img.convertTo(Img1, CV_8UC1, 1 / 1.99);


				Canny(Img1, Img1, 70, 110, 3);
				Img1.convertTo(Img, CV_16UC1, 1.99);

				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Img.at<ushort>(i, j) > 0)
							Temp1[j*nImgSizeX + i] = 4095;
						else
							Temp1[j*nImgSizeX + i] = 0;
					}
				}

				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {

						Img.at<uchar>(i, j) = Temp2[j*nImgSizeX + i];
					}
				}


				//Canny算子
				Canny(Img, Img, 70, 110, 3);

				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}
				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}
		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::otsu() {

	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();
		bool ok;
		int Threshold = 0;

		for (int k = 1; k <= bandcount; k++) {

			pBand = pDataSrc->GetRasterBand(k);
			cout << "DataType编号" << pBand->GetRasterDataType() << endl;
			cout << "Color Interpretation:" << pBand->GetColorInterpretation() << endl;
			cout << "Color Interpretation:" << GDALGetColorInterpretationName(pBand->GetColorInterpretation()) << endl;

			///////no problem//////






			///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);

				Threshold = calc_Ostu_Threshold_Unit16(Temp, nImgSizeX, nImgSizeY);
				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp[i*nImgSizeX + j] > Threshold) {
							Temp[i*nImgSizeX + j] = -1;
						}
						else {
							Temp[i*nImgSizeX + j] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);


				Threshold = calc_Ostu_Threshold_Unit8(Temp2, nImgSizeX, nImgSizeY);



				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp2[i*nImgSizeX + j] > Threshold) {
							Temp2[i*nImgSizeX + j] = -1;
						}
						else {
							Temp2[i*nImgSizeX + j] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {

				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}


		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}

}

void RS_DEMO_GDAL::laplacian()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();



		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);


			int  Threshold;
			long sum = 0;
			long area = 0;
			long long sum_square = 0;
			long mean = 0;//均值
			long sigma = 0;//方差


						   ///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);

				cout << "ok1" << endl;

				Mat Img;
				Img.create(nImgSizeY, nImgSizeX, CV_16UC1);
				cout << "ok2" << endl;
				//laplacian算子
				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<ushort>(j, i) = abs(4 * Temp[j*nImgSizeX + i] -
							Temp[(j + 1)*nImgSizeX + i] -
							Temp[j*nImgSizeX + i + 1] -
							Temp[(j - 1)*nImgSizeX + i] -
							Temp[j*nImgSizeX + i - 1]);
					}
				}
				cout << "ok3" << endl;

				//Img = Img.mul(256);

				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp1[j*nImgSizeX + i] = Img.at<ushort>(j, i);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 2; i++) {
					for (int j = 1; j < nImgSizeY - 2; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum += Temp1[j*nImgSizeX + i];
							area++;

						}
					}
				}
				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum_square += ((Temp1[j*nImgSizeX + i] - mean)*(Temp1[j*nImgSizeX + i] - mean));


						}
					}
				}

				sigma = sqrt(sum_square / area);


				Threshold = mean + 1 * sigma;



				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp1[j*nImgSizeX + i] > Threshold) {
							Temp1[j*nImgSizeX + i] = 4095;
						}
						else {
							Temp1[j*nImgSizeX + i] = 0;
						}

					}
				}
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Img.at<ushort>(j, i) = Temp1[j*nImgSizeX + i];
					}
				}


				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);

				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;



				//laplacian算子
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<uchar>(i, j) = abs(4 * Temp2[j*nImgSizeX + i] -
							Temp2[(j + 1)*nImgSizeX + i] -
							Temp2[j*nImgSizeX + i + 1] -
							Temp2[(j - 1)*nImgSizeX + i] -
							Temp2[j*nImgSizeX + i - 1]);
					}
				}

				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}



				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum += Temp3[j*nImgSizeX + i];
							area++;
						}
					}
				}

				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum_square += (Temp3[j*nImgSizeX + i] - mean)*(Temp3[j*nImgSizeX + i] - mean);

						}
					}
				}


				sigma = sqrt(sum_square / area);

				Threshold = mean + 0.5 * sigma;


				//阈值分割

				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp3[j*nImgSizeX + i] > Threshold) {
							Temp3[j*nImgSizeX + i] = 255;
						}
						else {
							Temp3[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}

		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::roborts()
{
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();

		/*pData->Release();
		GDALDriver *newdriver = GetGDALDriverManager()->GetDriverByName("GTiff");

		pData = newdriver->Create("copy.temp", nImgSizeX, nImgSizeY, bandcount, pBand->GetRasterDataType(), NULL);*/

		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);


			int Threshold;
			long sum = 0;
			long area = 0;
			long long sum_square = 0;
			long mean = 0;//均值
			long sigma = 0;//方差


						   ///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
				uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);



				Mat Img;
				Img.create(nImgSizeY, nImgSizeX, CV_16UC1);

				//robort算子
				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX - 1; i++) {
					for (int j = 0; j < nImgSizeY - 1; j++) {

						Img.at<ushort>(j, i) = abs(Temp[j*nImgSizeX + i] - Temp[(j + 1)*nImgSizeX + i + 1]) +
							abs(Temp[j*nImgSizeX + i + 1] - Temp[(j + 1)*nImgSizeX + i]);



					}
				}

				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp1[j*nImgSizeX + i] = Img.at<ushort>(j, i);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 2; i++) {
					for (int j = 1; j < nImgSizeY - 2; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum += Temp1[j*nImgSizeX + i];
							area++;

						}
					}
				}
				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp1[j*nImgSizeX + i] != 0) {
							sum_square += ((Temp1[j*nImgSizeX + i] - mean)*(Temp1[j*nImgSizeX + i] - mean));


						}
					}
				}

				sigma = sqrt(sum_square / area);


				Threshold = mean + 1 * sigma;



				//阈值分割
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp1[j*nImgSizeX + i] > Threshold) {
							Temp1[j*nImgSizeX + i] = 4095;
						}
						else {
							Temp1[j*nImgSizeX + i] = 0;
						}

					}
				}


				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);

				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;



				//robert算子
				for (int i = 0; i < nImgSizeX - 1; i++) {
					for (int j = 0; j < nImgSizeY - 1; j++) {

						Img.at<uchar>(i, j) = abs(Temp2[j*nImgSizeX + i] - Temp2[(j + 1)*nImgSizeX + i + 1]) +
							abs(Temp2[j*nImgSizeX + i + 1] - Temp2[(j + 1)*nImgSizeX + i]);



					}
				}

				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}


				//计算均值
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum += Temp3[j*nImgSizeX + i];
							area++;
						}
					}
				}

				mean = sum / area;

				//计算方差
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {
						if (Temp3[j*nImgSizeX + i] != 0) {
							sum_square += (Temp3[j*nImgSizeX + i] - mean)*(Temp3[j*nImgSizeX + i] - mean);

						}
					}
				}

				sigma = sqrt(sum_square / area);

				Threshold = mean + 1 * sigma;



				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						if (Temp3[j*nImgSizeX + i] > Threshold) {
							Temp3[j*nImgSizeX + i] = -1;
						}
						else {
							Temp3[j*nImgSizeX + i] = 0;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {
				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}


		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}

void RS_DEMO_GDAL::filter_mid() {
	if (pDataSrc)
	{
		QLabel*sizeLabel = new QLabel;//创建新的标签
		QDialog *dialog = new QDialog;//创建新的对话框
		bool ok;//设置判断是否输入模板大小
		QString title = title.fromLocal8Bit("模板大小");
		QString content = content.fromLocal8Bit("请输入模板半径：");
		int tSize = QInputDialog::getInt(this, title, content, sizeLabel->text().toInt(), 0, 140, 1, &ok);
		if (ok)
			sizeLabel->setText(QString(tr("%1")).arg(tSize));
		if (!ok)
			return;
		if (tSize % 2 == 0) {
			QString error = error.fromLocal8Bit("错误");
			QString content = content.fromLocal8Bit("请输入奇数!");
			QMessageBox::information(this, error, content);
		}
		else {
			int xSize = pDataSrc->GetRasterXSize();
			int ySize = pDataSrc->GetRasterYSize();
			int bands = pDataSrc->GetRasterCount();//遥感图像波段数
			GDALDataType datatype = pDataSrc->GetRasterBand(1)->GetRasterDataType();
			if (datatype == GDT_Byte) {
				for (int n = 1; n <= bands; n++) {
					GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

					cv::Mat data1 = cv::Mat(ySize, xSize, CV_8UC1);
					cv::Mat data2 = cv::Mat(ySize, xSize, CV_8UC1);
					uchar*data = data1.ptr<uchar>(0);
					uchar*linedata = new uchar[xSize];
					//input
					for (int i = 0; i < ySize; i++) {
						data = data1.ptr<uchar>(i);
						poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
						for (int j = 0; j < xSize; j++) {
							data[j] = linedata[j];
						}

					}
					//median filter
					cv::medianBlur(data1, data2, tSize);
					//output
					for (int i = 0; i < ySize; i++) {
						data = data2.ptr<uchar>(i);
						for (int j = 0; j < xSize; j++) {
							linedata[j] = data[j];
						}
						pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
					}
					if (linedata != NULL)
						delete linedata;
				}
			}
			if (datatype == GDT_UInt16) {
				if (tSize > 5) {
					QString error = error.fromLocal8Bit("警告");
					QString content = content.fromLocal8Bit("16位深数据类型模板半径大于5暂不适用！");
					QMessageBox::information(this, error, content);
				}
				else {
					for (int n = 1; n <= bands; n++) {
						GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

						cv::Mat data1 = cv::Mat(ySize, xSize, CV_16U);
						cv::Mat data2 = cv::Mat(ySize, xSize, CV_16U);
						ushort*data = data1.ptr<ushort>(0);
						ushort*linedata = new ushort[xSize];
						//input
						for (int i = 0; i < ySize; i++) {
							data = data1.ptr<ushort>(i);
							poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
							for (int j = 0; j < xSize; j++) {
								data[j] = linedata[j];
							}
						}

						//median filter
						cv::medianBlur(data1, data2, tSize);
						//output
						for (int i = 0; i < ySize; i++) {
							data = data2.ptr<ushort>(i);
							for (int j = 0; j < xSize; j++) {
								linedata[j] = data[j];
							}
							pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
						}
						if (linedata != NULL)
							delete linedata;
					}
				}
			}
			else if (datatype != GDT_Byte&&datatype != GDT_UInt16) {
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("抱歉！本版本只支持无符号8位和无符号16位数据类型的图像！"));
			}
			SetViewFocus = 1;
			updatecontroller();
			updateinfo();
			SetViewFocus = 0;
		}

	}
	else {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("未载入图片"));
		return;
	}

}
void RS_DEMO_GDAL::filter_mean() {
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("未载入图片"));
		return;
	}
	else {
		//弹框选择模板大小

		QLabel*sizeLabel = new QLabel;//创建新的标签
		QDialog *dialog = new QDialog;//创建新的对话框
		bool ok;//设置判断是否输入模板大小
		QString title = title.fromLocal8Bit("模板大小");
		QString content = content.fromLocal8Bit("请输入模板半径：");
		int tSize = QInputDialog::getInt(this, title, content, sizeLabel->text().toInt(), 0, 140, 1, &ok);
		if (ok)
			sizeLabel->setText(QString(tr("%1")).arg(tSize));
		if (!ok)
			return;
		int xSize = pDataSrc->GetRasterXSize();
		int ySize = pDataSrc->GetRasterYSize();
		int bands = pDataSrc->GetRasterCount();//遥感图像波段数
		GDALDataType datatype = pDataSrc->GetRasterBand(1)->GetRasterDataType();
		if (datatype == GDT_Byte) {
			if (tSize == 0) {
				QString error = error.fromLocal8Bit("错误");
				QString content = content.fromLocal8Bit("请输入大于0的模板半径!");
				QMessageBox::information(this, error, content);
			}
			else {
				for (int n = 1; n <= bands; n++) {
					GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

					cv::Mat data1 = cv::Mat(ySize, xSize, CV_8UC1);
					cv::Mat data2 = cv::Mat(ySize, xSize, CV_8UC1);
					uchar*data = data1.ptr<uchar>(0);
					uchar*linedata = new uchar[xSize];
					//input
					for (int i = 0; i < ySize; i++) {
						data = data1.ptr<uchar>(i);
						poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
						for (int j = 0; j < xSize; j++) {
							data[j] = linedata[j];
						}
					}
					//mean filter
					cv::blur(data1, data2, Size(tSize, tSize));
					//output
					for (int i = 0; i < ySize; i++) {
						data = data2.ptr<uchar>(i);
						for (int j = 0; j < xSize; j++) {
							linedata[j] = data[j];
						}
						pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
					}
					if (linedata != NULL)
						delete linedata;
				}
			}
		}
		if (datatype == GDT_UInt16) {
			if (tSize == 0) {
				QString error = error.fromLocal8Bit("错误");
				QString content = content.fromLocal8Bit("请输入大于0的模板半径!");
				QMessageBox::information(this, error, content);
			}
			else {
				for (int n = 1; n <= bands; n++) {
					GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

					cv::Mat data1 = cv::Mat(ySize, xSize, CV_16U);
					cv::Mat data2 = cv::Mat(ySize, xSize, CV_16U);
					ushort*data = data1.ptr<ushort>(0);
					ushort*linedata = new ushort[xSize];
					//input
					for (int i = 0; i < ySize; i++) {
						data = data1.ptr<ushort>(i);
						poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
						for (int j = 0; j < xSize; j++) {
							data[j] = linedata[j];
						}

					}
					//mean filter
					cv::blur(data1, data2, Size(tSize, tSize));
					//output
					for (int i = 0; i < ySize; i++) {
						data = data2.ptr<ushort>(i);
						for (int j = 0; j < xSize; j++) {
							linedata[j] = data[j];
						}
						pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
					}
					if (linedata != NULL)
						delete linedata;
				}
			}
		}
		else if (datatype != GDT_Byte&&datatype != GDT_UInt16) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("抱歉！本版本只支持无符号8位和无符号16位数据类型的图像！"));
		}
		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}
}
void RS_DEMO_GDAL::filter_gauss() {
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("未载入图片"));
		return;
	}
	else {
		//弹框选择模板大小

		QLabel*sizeLabel = new QLabel;//创建新的标签
		QLabel*sigmaLabel = new QLabel;
		QDialog *dialog = new QDialog;//创建新的对话框
		bool ok;//设置判断是否输入模板大小
		QString error = error.fromLocal8Bit("模板大小");
		QString content = content.fromLocal8Bit("请输入模板半径：");
		int tSize = QInputDialog::getInt(this, error, content, sizeLabel->text().toInt(), 0, 140, 1, &ok);
		if (tSize % 2 == 0) {
			QString error = error.fromLocal8Bit("错误");
			QString content = content.fromLocal8Bit("请输入奇数！");
			QMessageBox::information(this, error, content);
		}
		else {
			QString error = error.fromLocal8Bit("方差");
			QString content = content.fromLocal8Bit("请输入方差：");
			float sigma = QInputDialog::getDouble(this, error, content, sigmaLabel->text().toDouble(), 0, 140, 1, &ok);
			if (ok)
				sizeLabel->setText(QString(tr("%1")).arg(tSize));
			sigmaLabel->setText(QString(tr("%1")).arg(sigma));
			if (!ok)
				return;

			int xSize = pDataSrc->GetRasterXSize();
			int ySize = pDataSrc->GetRasterYSize();
			int bands = pDataSrc->GetRasterCount();//遥感图像波段数
			GDALDataType datatype = pDataSrc->GetRasterBand(1)->GetRasterDataType();
			if (datatype == GDT_Byte) {
				for (int n = 1; n <= bands; n++) {
					GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

					cv::Mat data1 = cv::Mat(ySize, xSize, CV_8UC1);
					cv::Mat data2 = cv::Mat(ySize, xSize, CV_8UC1);
					uchar*data = data1.ptr<uchar>(0);
					uchar*linedata = new uchar[xSize];
					//input
					for (int i = 0; i < ySize; i++) {
						data = data1.ptr<uchar>(i);
						poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
						for (int j = 0; j < xSize; j++) {
							data[j] = linedata[j];
						}

					}
					//Gaussian filter
					cv::GaussianBlur(data1, data2, Size(tSize, tSize), sigma);
					//output
					for (int i = 0; i < ySize; i++) {
						data = data2.ptr<uchar>(i);
						for (int j = 0; j < xSize; j++) {
							linedata[j] = data[j];
						}
						pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
					}
					if (linedata != NULL)
						delete linedata;
				}
			}
			if (datatype == GDT_UInt16) {
				for (int n = 1; n <= bands; n++) {
					GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

					cv::Mat data1 = cv::Mat(ySize, xSize, CV_16U);
					cv::Mat data2 = cv::Mat(ySize, xSize, CV_16U);
					ushort*data = data1.ptr<ushort>(0);
					ushort*linedata = new ushort[xSize];
					//input
					for (int i = 0; i < ySize; i++) {
						data = data1.ptr<ushort>(i);
						poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
						for (int j = 0; j < xSize; j++) {
							data[j] = linedata[j];
						}

					}
					//Gaussian filter
					cv::GaussianBlur(data1, data2, Size(tSize, tSize), sigma);
					//output
					for (int i = 0; i < ySize; i++) {
						data = data2.ptr<ushort>(i);
						for (int j = 0; j < xSize; j++) {
							linedata[j] = data[j];
						}
						pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_UInt16, 0, 0);
					}
					if (linedata != NULL)
						delete linedata;
				}
			}
			else if (datatype != GDT_Byte&&datatype != GDT_UInt16) {
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("抱歉！本版本只支持无符号8位和无符号16位数据类型的图像！"));
			}
			SetViewFocus = 1;
			updatecontroller();
			updateinfo();
			SetViewFocus = 0;
		}
	}
}
void RS_DEMO_GDAL::sharpen() {
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("未载入图片"));
		return;
	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();
		GDALDataType datatype = pDataSrc->GetRasterBand(1)->GetRasterDataType();
		for (int k = 1; k <= bandcount; k++) {
			pBand = pDataSrc->GetRasterBand(k);
			uint16_t* Temp = new uint16_t[nImgSizeX*nImgSizeY];
			uint16_t* Temp1 = new uint16_t[nImgSizeX*nImgSizeY];
			unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
			unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
			int Threshold1, Threshold2;
			long sum = 0;
			long area = 0;
			long long sum_square = 0;
			long mean = 0;//均值
			long sigma = 0;//方差
						   ///////////16 bit////////////
			if (datatype == GDT_UInt16) {
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_16UC1);

				//sobel算子
				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<_int16>(i, j) = 0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
							2 * Temp[(j)*nImgSizeX + i - 1] +
							Temp[(j + 1)*nImgSizeX + i - 1] +
							Temp[(j)*nImgSizeX + i] -
							Temp[(j - 1)*nImgSizeX + i + 1] -
							2 * Temp[(j)*nImgSizeX + i + 1] -
							Temp[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp[(j - 1)*nImgSizeX + i - 1] +
								2 * Temp[(j - 1)*nImgSizeX + i] +
								Temp[(j - 1)*nImgSizeX + i + 1] +
								Temp[(j)*nImgSizeX + i] -
								Temp[(j + 1)*nImgSizeX + i - 1] -
								2 * Temp[(j + 1)*nImgSizeX + i] -
								Temp[(j + 1)*nImgSizeX + i + 1]);
					}
				}
				//赋回Temp1
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp1[j*nImgSizeX + i] = Img.at<__int16>(i, j);
					}
				}
				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp1, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
			}
			///////////8 bit////////////
			if (datatype == GDT_Byte) {
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				Mat Img;
				Img.create(nImgSizeX, nImgSizeY, CV_8UC1);


				Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {

						Img.at<uchar>(i, j) = Temp2[j*nImgSizeX + i];
					}
				}
				//sobel算子
				for (int i = 1; i < nImgSizeX - 1; i++) {
					for (int j = 1; j < nImgSizeY - 1; j++) {

						Img.at<uchar>(i, j) = 0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
							2 * Temp2[(j)*nImgSizeX + i - 1] +
							Temp2[(j + 1)*nImgSizeX + i - 1] +
							Temp2[(j)*nImgSizeX + i] -
							2 * Temp2[(j - 1)*nImgSizeX + i + 1] -
							Temp2[(j)*nImgSizeX + i + 1] -
							Temp2[(j + 1)*nImgSizeX + i + 1]) +
							0.5*abs(Temp2[(j - 1)*nImgSizeX + i - 1] +
								2 * Temp2[(j - 1)*nImgSizeX + i] +
								Temp2[(j - 1)*nImgSizeX + i + 1] +
								Temp2[(j)*nImgSizeX + i] -
								Temp2[(j + 1)*nImgSizeX + i - 1] -
								2 * Temp2[(j + 1)*nImgSizeX + i] -
								Temp2[(j + 1)*nImgSizeX + i + 1]);
					}
				}
				//赋回Temp3
				for (int i = 0; i < nImgSizeX; i++) {
					for (int j = 0; j < nImgSizeY; j++) {
						Temp3[j*nImgSizeX + i] = Img.at<uchar>(i, j);
					}
				}
				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp3, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
			}
			else if (datatype != GDT_Byte&&datatype != GDT_UInt16) {
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
					QString::fromLocal8Bit("抱歉！本版本只支持无符号8位和无符号16位数据类型的图像！"));
			}
		}
		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}

}
void RS_DEMO_GDAL::histequalization() {
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("未载入图片"));
		return;
	}
	else {
		int xSize = pDataSrc->GetRasterXSize();
		int ySize = pDataSrc->GetRasterYSize();
		int bands = pDataSrc->GetRasterCount();//遥感图像波段数
		GDALDataType datatype = pDataSrc->GetRasterBand(1)->GetRasterDataType();
		if (datatype == GDT_Byte) {
			for (int n = 1; n <= bands; n++) {
				GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

				cv::Mat data1 = cv::Mat(ySize, xSize, CV_8UC1);
				cv::Mat data2 = cv::Mat(ySize, xSize, CV_8UC1);
				uchar*data = data1.ptr<uchar>(0);
				uchar*linedata = new uchar[xSize];
				//input
				for (int i = 0; i < ySize; i++) {
					data = data1.ptr<uchar>(i);
					poBand->RasterIO(GF_Read, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
					for (int j = 0; j < xSize; j++) {
						data[j] = linedata[j];
					}
				}
				//equalize the histogram
				cv::equalizeHist(data1, data2);
				//output
				for (int i = 0; i < ySize; i++) {
					data = data2.ptr<uchar>(i);
					for (int j = 0; j < xSize; j++) {
						linedata[j] = data[j];
					}
					pData->GetRasterBand(n)->RasterIO(GF_Write, 0, i, xSize, 1, linedata, xSize, 1, GDT_Byte, 0, 0);
				}
				if (linedata != NULL)
					delete linedata;
			}
		}
		if (datatype == GDT_UInt16) {
			for (int n = 1; n <= bands; n++) {
				GDALRasterBand *poBand = pDataSrc->GetRasterBand(n);//指向第i个波段的指针

				ushort*linedata = new ushort[xSize*ySize];
				ushort*data1 = new ushort[xSize*ySize];
				poBand->RasterIO(GF_Read, 0, 0, xSize, ySize, linedata, xSize, ySize, GDT_UInt16, 0, 0);

				double *p = new double[4096];
				int m[4096] = { 0 };
				int *q = new int[4096];
				double *t = new double[4096];
				double s = xSize*ySize;

				for (int i = 0; i < ySize*xSize; i++) {
					m[linedata[i]]++;
				}
				for (int i = 0; i < 4096; i++)
				{
					p[i] = m[i] / s;
				}
				t[0] = p[0];
				for (int i = 1; i < 4096; i++)
				{
					t[i] = t[i - 1] + p[i];
				}
				for (int i = 0; i < 4096; i++)
				{
					q[i] = t[i] * 4095;
				}
				for (int i = 0; i < ySize*xSize; i++)
				{
					data1[i] = q[linedata[i]];
				}
				pData->GetRasterBand(n)->RasterIO(GF_Write, 0, 0, xSize, ySize, data1, xSize, ySize, GDT_UInt16, 0, 0);
				if (linedata != NULL)
					delete linedata;
				//if (m != NULL)
				//	delete m;
				if (p != NULL)
					delete p;
				if (q != NULL)
					delete q;
				if (t != NULL)
					delete t;
			}
		}
		else if (datatype != GDT_Byte&&datatype != GDT_UInt16) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("抱歉！本版本只支持无符号8位和无符号16位数据类型的图像！"));
		}
		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;
	}

}

void RS_DEMO_GDAL::updatecursor()
{
	switch (SetViewFocus)
	{
	case 0: {
		axis_h_origin->src = pDataSrc;
		axis_v_origin->src = pDataSrc;
		int hvalue = int(view_origin->horizontalScrollBar()->value() / ratio_origin[0]);
		int vvalue = int(view_origin->verticalScrollBar()->value()/ratio_origin[1]);
		//re-draw cursor
		axis_h_origin->startposition[0] = hvalue;
		axis_h_origin->startposition[1] = vvalue;
		axis_v_origin->startposition[0] = hvalue;
		axis_v_origin->startposition[1] = vvalue;
		axis_v_origin->ratio = ratio_origin[1];
		axis_h_origin->ratio = ratio_origin[0];
		axis_h_origin->update();
		axis_v_origin->update();
		break;
	}
	case 1: {
		axis_h_after->src = pData;
		axis_v_after->src = pData;
		int hvalue = int(view_after->horizontalScrollBar()->value()/ ratio_after[0]);
		//真实的位置，重绘坐标轴将依据这个值
		int vvalue = int(view_after->verticalScrollBar()->value() / ratio_after[1]);
		//真实的位置，重绘坐标轴将依据这个值
		axis_h_after->startposition[0] = hvalue;
		axis_h_after->startposition[1] = vvalue;
		axis_v_after->startposition[0] = hvalue;
		axis_v_after->startposition[1] = vvalue;
		axis_v_after->ratio = ratio_after[1];
		axis_h_after->ratio = ratio_after[0];
		axis_h_after->update();
		axis_v_after->update();
		break;
	}
	}
}

int RS_DEMO_GDAL::calc_Ostu_Threshold_Unit16(uint16_t* Buf, int ImagesizeX, int ImagesizeY) {
	int histArray[4096] = { 0 };
	int J[4096];
	float u0, u1;//c0和c1组的均值
	int count0;
	float w0, w1;//c0和c1组产生的概率

	for (int i = 0; i<ImagesizeY; i++)
		for (int j = 0; j < ImagesizeX; j++) {
			histArray[Buf[i*ImagesizeX + j]]++;
		}



	//统计直方图中像素的个数，存放在sum中
	int sum = 0;
	for (int i = 0; i<4096; i++)
		sum = sum + histArray[i];

	float devi[4095];//存放每个阈值对应的方差，共4095个可能的阈值

					 //阈值t从0至254变化，计算对应的方差
	for (int t = 0; t<4095; t++) {
		//计算阈值为t时，c0组的均值和产生的概率
		u0 = 0;
		count0 = 0;//c0组的像素数
		for (int i = 0; i <= t; i++) {
			u0 += i*histArray[i];
			count0 += histArray[i];
		}
		if (count0 == 0 || count0 == sum) {//C0组没有像素或者所有的像素都分到了C0组
			devi[t] = -1;//
			continue;
		}
		if (count0 != 0)
			u0 = u0 / count0;
		else
			u0 = 0;
		if (sum != 0)
			w0 = (float)count0 / sum;
		else
			w0 = 0;//c0组的概率

				   //计算阈值为t时，c1组的均值和产生的概率
		u1 = 0;
		for (int i = t + 1; i<4096; i++)
			u1 += i*histArray[i];
		if ((sum - count0) != 0) {
			u1 = u1 / (sum - count0);
		}
		else {
			u1 = 0;
		}
		//C0组像素数与C1组像素数之和为图像总像素数。
		w1 = 1 - w0;

		//两组间的方差
		devi[t] = w0*w1*(u1 - u0)*(u1 - u0);

	}
	//在255个阈值中，搜索最大方差对应的阈值
	float maxDevi = devi[0];
	int maxT = 0;
	for (int t = 1; t<4095; t++) {
		if (devi[t]>maxDevi) {
			maxDevi = devi[t];
			maxT = t;
		}
	}

	//返回最佳阈值

	return maxT;
}

int RS_DEMO_GDAL::calc_Ostu_Threshold_Unit8(unsigned char* Buf, int ImagesizeX, int ImagesizeY) {
	int histArray[256] = { 0 };
	int J[256];
	float u0, u1;//c0和c1组的均值
	int count0;
	float w0, w1;//c0和c1组产生的概率

	for (int i = 0; i<ImagesizeY; i++)
		for (int j = 0; j < ImagesizeX; j++) {
			histArray[Buf[i*ImagesizeX + j]]++;
			if (j == 0)
				printf("(%d)\n", i);
		}



	//统计直方图中像素的个数，存放在sum中
	int sum = 0;
	for (int i = 0; i<256; i++)
		sum = sum + histArray[i];

	float devi[255];//存放每个阈值对应的方差，共4095个可能的阈值

					//阈值t从0至254变化，计算对应的方差
	for (int t = 0; t<255; t++) {
		//计算阈值为t时，c0组的均值和产生的概率
		u0 = 0;
		count0 = 0;//c0组的像素数
		for (int i = 0; i <= t; i++) {
			u0 += i*histArray[i];
			count0 += histArray[i];
		}
		if (count0 == 0 || count0 == sum) {//C0组没有像素或者所有的像素都分到了C0组
			devi[t] = -1;//
			continue;
		}
		u0 = u0 / count0;
		w0 = (float)count0 / sum;//c0组的概率

								 //计算阈值为t时，c1组的均值和产生的概率
		u1 = 0;
		for (int i = t + 1; i<256; i++)
			u1 += i*histArray[i];

		u1 = u1 / (sum - count0);//C0组像素数与C1组像素数之和为图像总像素数。
		w1 = 1 - w0;

		//两组间的方差
		devi[t] = w0*w1*(u1 - u0)*(u1 - u0);

	}
	//在255个阈值中，搜索最大方差对应的阈值
	float maxDevi = devi[0];
	int maxT = 0;
	for (int t = 1; t<255; t++) {
		if (devi[t]>maxDevi) {
			maxDevi = devi[t];
			maxT = t;
		}
	}

	//返回最佳阈值

	return maxT;
}

void RS_DEMO_GDAL::drawchange()
{
	if (pData) {
		dischange->src1 = pDataSrc;
		dischange->src2 = pData;
		dischange->init_set();
		dischange->hide();
		dischange->show();
	}
	else {
		errordlg->setWindowTitle("Error!");
		errordlg->showMessage("Currently no dataset is loaded!");
	}
}

void RS_DEMO_GDAL::drawhist_origin()
{
	SetViewFocus = 0;
	drawhist();
}

void RS_DEMO_GDAL::updateview_origin()
{	
	if (!pDataSrc) return;
	SetViewFocus = 0;
	cout << "update origin" << endl;
	GDALtoQImage();
	cout << "making qimage ok" << endl;
	view_origin->updateview(&img_origin);
	updateinfo();
	updatecursor_origin();
}

void RS_DEMO_GDAL::open() {
	clock_t  clockBegin, clockEnd;
	//CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//为了支持中文路径
	qfilename = QFileDialog::getOpenFileName(this, "Open Image File", QString(),
		"Image Files (*.jpg *.bmp *.tif *.tiff *.png *.img *.int);;  All Files(*.*)");
	string filename = qfilename.toStdString();

	clockBegin = clock();

	QProgressDialog opendialog(QString::fromLocal8Bit("文件载入中"),
		QString::fromLocal8Bit("知道了"), 0, 3000, this);
	opendialog.setWindowTitle(QString::fromLocal8Bit("文件载入中"));
	opendialog.setWindowModality(Qt::WindowModal);
	opendialog.show();

	GDALAllRegister();
	pDataOri = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
	opendialog.setValue(1000);
	QCoreApplication::processEvents();

	if (pDataOri) {//read success
		//updateinfo();//update image information
					//&display in application
		std::cout << "open file ok" << endl;

		(view_origin->ratio)[0] = 1;
		(view_origin->ratio)[1] *= 1;
		ratio_origin[1] = 1;
		ratio_origin[0] = 1;
		(view_after->ratio)[0] = 1;
		(view_after->ratio)[1] = 1;
		ratio_after[1] = 1;
		ratio_after[0] = 1;

		GDALDataType type = pDataOri->GetRasterBand(1)->GetRasterDataType();
		int XSize = pDataOri->GetRasterXSize();
		int YSize = pDataOri->GetRasterYSize();
		int nBands = pDataOri->GetRasterCount();
		//GDALDriver *driver = pDataOri->GetDriver();

		GDALDriver *driver1 = GetGDALDriverManager()->GetDriverByName("GTiff");

		if (pDataSrc) {
			pDataSrc->Release();
			remove("src.temp");
		}

		pDataSrc = driver1->CreateCopy("src.temp", pDataOri, 0, 0, 0, 0);
		axis_h_origin->src = pDataSrc;
		axis_v_origin->src = pDataSrc;
		SetViewFocus = 0;
		updatecursor();
		updatecontroller();
		updateinfo();
		opendialog.setValue(2000);
		QCoreApplication::processEvents();

		GDALDriver *driver2 = GetGDALDriverManager()->GetDriverByName("GTiff");

		if (pData) {
			pData->Release();
			remove("data.temp");
		}
		pData = driver2->CreateCopy("data.temp",pDataOri,0,0,0,0);
		axis_h_after->src = pData;
		axis_v_after->src = pData;
		SetViewFocus = 1;
		updatecursor();
		updatecontroller();
		updateinfo();
		opendialog.setValue(3000);
		QCoreApplication::processEvents();

		if (XSize > 8000 || YSize > 8000) {
			filetoolarge->hide();
			filetoolarge->show();
		}

	}
	else {
		//errordlg->setWindowTitle("Error!");
		//errordlg->showMessage("Open File Failed!\nThis is because you selected a non-image file, or the file itself is invalid/broken!");
	}
	clockEnd = clock();
	cout << "打开用时：" << clockEnd - clockBegin << endl;
}

void RS_DEMO_GDAL::ImageCut()
{
	
	if (pDataSrc == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("图片还没有读入"),
			QString::fromLocal8Bit("图片还没有读入"));
		return;
	}

	filetoolarge->hide();
	/* 定义缓存类型 */
	typedef unsigned char  DT_8U;
	typedef double DT_16U;
	//GDALAllRegister();
	GDALDataType eDT = pDataSrc->GetRasterBand(1)->GetRasterDataType();//返回当前波段中的数据类型
	int iBandCount = pDataSrc->GetRasterCount();//返回波段数

	QRect rect;
	mouserect* mourect = new mouserect;
	mourect->exec();

	
	//connect(mourect, SIGNAL(signalCompleteCature(QPixmap)), this, SLOT(mourect->getRect()));
	rect = mourect->getRect();
	//rect.setLeft(1000);
	//rect.setTop(1000);
	//rect.setWidth(1000);
	//rect.setHeight(700);//根据裁切范围确定裁切后的图像宽高
	int iDstWidth = rect.width()/ratio_origin[0];//裁切列数
	int iDstHeight = rect.height() / ratio_origin[1];//裁切行数
	int iStartX = rect.left() - view_origin->pos().x() + view_origin->mapFromGlobal(view_origin->pos()).x()/*65*/ + view_origin->horizontalScrollBar()->value();//起始列号
	iStartX = iStartX / ratio_origin[0];
	cout << iStartX << endl;
	int iStartY = rect.top() - view_origin->pos().y() + view_origin->mapFromGlobal(view_origin->pos()).y()/*75*/ + view_origin->verticalScrollBar()->value();//起始行号
	iStartY = iStartY / ratio_origin[1];
	cout << iStartY << endl;
	if ((iDstWidth <= 5) || (iDstHeight <= 5) || ((iStartX == 0) && (iStartY == 0)))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("裁切区域选择错误"),
			QString::fromLocal8Bit("您还没有选择裁切区域"));
		return;
	}
	if ((iStartX <0) || (iStartY < 0) || ((iStartX + iDstWidth) > pDataSrc->GetRasterXSize()) || ((iStartY + iDstHeight) > pDataSrc->GetRasterYSize()))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("裁切区域选择错误"),
			QString::fromLocal8Bit("裁切区域不能超过图像范围"));
		cout << iStartY << endl;
		return;
	}
	pData->Release();
	GDALDriver *newdriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	pData = newdriver->Create("cut.temp", iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	//地理信息
	double adfGeoTransform[6] = { 0, 1, 0, 0, 1, 0 };
	pDataSrc->GetGeoTransform(adfGeoTransform);
	adfGeoTransform[0] = adfGeoTransform[0] + iStartX*adfGeoTransform[1] + iStartY*adfGeoTransform[2];
	adfGeoTransform[3] = adfGeoTransform[3] + iStartX*adfGeoTransform[4] + iStartY*adfGeoTransform[5];

	pData->SetGeoTransform(adfGeoTransform);
	pData->SetProjection(pDataSrc->GetProjectionRef());

	
	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
	{
		pBandMap[i] = i + 1;
	}
	//处理8位图像
	if (eDT == GDT_Byte)
	{
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];
		pDataSrc->RasterIO(GF_Read, iStartX, iStartY, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pData->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		delete[]pDataBuff;
	}
	//处理16位图像
	else
	{
		DT_16U *pDataBuff = new DT_16U[iDstWidth*iDstHeight*iBandCount];
		pDataSrc->RasterIO(GF_Read, iStartX, iStartY, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pData->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		delete[]pDataBuff;
	}
	delete[]pBandMap;
	//更新视图及信息
	SetViewFocus = 1;
	updatecontroller();
	updateinfo();
	SetViewFocus = 0;

}

void RS_DEMO_GDAL::enable_zoom_origin()
{
	if (!pData || !pDataSrc) {
		return;
	}
	
	(view_origin->ratio)[0] = 1;
	(view_origin->ratio)[1] = 1;
	ratio_origin[1] = 1;
	ratio_origin[0] = 1;
	SetViewFocus = 0;
	updatecontroller();
	updateinfo();
}

void RS_DEMO_GDAL::enable_zoom_after()
{
	if (!pData || !pDataSrc) {
		return;
	}
	
	(view_after->ratio)[0] = 1;
	(view_after->ratio)[1] = 1;
	ratio_after[1] = 1;
	ratio_after[0] = 1;
	SetViewFocus = 1;
	updatecontroller();
	updateinfo();
}

void RS_DEMO_GDAL::helpmean() {
	QDesktopServices::openUrl(QUrl("https://imalan.cn/simplers/%E6%BB%A4%E6%B3%A2.html"));
}
void RS_DEMO_GDAL::helphist() {
	QDesktopServices::openUrl(QUrl("https://imalan.cn/simplers/%E7%9B%B4%E6%96%B9%E5%9B%BE%E5%9D%87%E8%A1%A1.html"));
}
void RS_DEMO_GDAL::help()
{
	QDesktopServices::openUrl(QUrl("https://imalan.cn/simplers/"));
}
void RS_DEMO_GDAL::helpsharpen() {
	QDesktopServices::openUrl(QUrl("https://imalan.cn/simplers/%E9%94%90%E5%8C%96.html"));
}

void RS_DEMO_GDAL::change_detection()
{
	if (pDataSrc)
	{
		QString qname = QFileDialog::getOpenFileName(this, "Open Image File", QString(),
			"Image Files (*.jpg *.bmp *.tif *.tiff *.png);;  All Files(*.*)");
		if (!qname.isEmpty())
		{
			string filename = qname.toStdString();
			GDALAllRegister();
			pData->Release();
			pData = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
			if (pData)
			{
				cout << "Open another file ok" << endl;
				SetViewFocus = 1;
				updatecontroller();
				cout << "init controller when open file ok" << endl;
				updateinfo();
				cout << "update info when open file ok" << endl;
			}
			else
			{
				errordlg->setWindowTitle("Error!");
				errordlg->showMessage("Open Another File Failed!");
			}

			int XSize1 = pDataSrc->GetRasterXSize();
			int XSize2 = pData->GetRasterXSize();

			int YSize1 = pDataSrc->GetRasterYSize();
			int YSize2 = pData->GetRasterYSize();

			int n1 = pDataSrc->GetRasterCount();
			int n2 = pData->GetRasterCount();

			GDALDataType type1 = pDataSrc->GetRasterBand(1)->GetRasterDataType();
			GDALDataType type2 = pData->GetRasterBand(1)->GetRasterDataType();
			if (n1 == n2&&XSize1 == XSize2&&YSize1 == YSize2&&type1 == type2)
			{
				drawchange();
			}
			else
			{
				QMessageBox::warning(this, tr("Sorry"),
					QString::fromLocal8Bit("请重新选择第二张图片"));
				return;
			}
		}
		else
		{
			QMessageBox::warning(this, tr("Sorry"),
				QString::fromLocal8Bit("没有打开第二个文件"));
			return;
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Sorry"),
			QString::fromLocal8Bit("请先打开第一张图片"));
		return;
	}

}

void RS_DEMO_GDAL::change_detection_help()
{
	QDesktopServices::openUrl(QUrl("https://imalan.cn/simplers/%E5%8F%98%E5%8C%96%E6%A3%80%E6%B5%8B.html"));
}

void RS_DEMO_GDAL::Ostu2() {
	if (pDataSrc == NULL) {
		QMessageBox::StandardButton reply;
		QString title = title.fromLocal8Bit("未载入图片！");
		QString information = information.fromLocal8Bit("请载入图片后再执行此操作");

		reply = QMessageBox::information(this, title, information);

	}
	else {
		GDALRasterBand *pBand;
		int nImgSizeX = pDataSrc->GetRasterXSize();
		int nImgSizeY = pDataSrc->GetRasterYSize();
		int bandcount = pDataSrc->GetRasterCount();
		int* Thres1 = new int[1];
		int* Thres2 = new int[1];
		int* topbound = new int[1];



		///////no problem//////

		bool ok;
		int Threshold1 = 0, Threshold2 = 0;

		for (int k = 1; k <= bandcount; k++) {

			pBand = pDataSrc->GetRasterBand(k);









			///////////16 bit////////////
			if (pBand->GetRasterDataType() == 2) {
				unsigned short* Temp = new unsigned short[nImgSizeX*nImgSizeY];
				unsigned short* Temp1 = new unsigned short[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);


				calc_double_Ostu_Threshold_Unit16(Temp, nImgSizeX, nImgSizeY, Thres1, Thres2, topbound);
				Threshold1 = *Thres1;
				Threshold2 = *Thres2;

				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp[i*nImgSizeX + j] > Threshold2) {
							Temp[i*nImgSizeX + j] = -1;
						}
						else if (Temp[i*nImgSizeX + j] < Threshold1) {
							Temp[i*nImgSizeX + j] = 0;
						}
						else if (Temp[i*nImgSizeX + j] >= Threshold1&&Temp[i*nImgSizeX + j] <= Threshold2) {

							Temp[i*nImgSizeX + j] = (*topbound) / 2;
						}
					}
				}



				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp;
				delete[] Temp1;
			}
			///////////8 bit////////////
			else if (pBand->GetRasterDataType() == 1) {
				unsigned char* Temp2 = new unsigned char[nImgSizeX*nImgSizeY];
				unsigned char* Temp3 = new unsigned char[nImgSizeX*nImgSizeY];
				pBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);


				calc_double_Ostu_Threshold_Unit8(Temp2, nImgSizeX, nImgSizeY, Thres1, Thres2);



				for (int i = 1; i < nImgSizeY - 1; i++) {
					for (int j = 1; j < nImgSizeX - 1; j++) {
						if (Temp2[i*nImgSizeX + j] > *Thres2) {
							Temp2[i*nImgSizeX + j] = -1;
						}
						else if (Temp2[i*nImgSizeX + j] < *Thres1) {
							Temp2[i*nImgSizeX + j] = 0;
						}
						else {
							Temp2[i*nImgSizeX + j] = 128;
						}
					}
				}




				pData->GetRasterBand(k)->RasterIO(GF_Write, 0, 0, nImgSizeX, nImgSizeY,
					Temp2, nImgSizeX, nImgSizeY, pBand->GetRasterDataType(), 0, 0);
				delete[] Temp2;
				delete[] Temp3;
			}
			else {


				QMessageBox::StandardButton reply;
				QString title = title.fromLocal8Bit("不能处理这种位图！");
				QString information = information.fromLocal8Bit("本功能只能处理无符号8位图像和无符号16位图像");

				reply = QMessageBox::information(this, title, information);
				break;
			}





		}

		SetViewFocus = 1;
		updatecontroller();
		updateinfo();
		SetViewFocus = 0;

		delete Thres1;
		delete Thres2;
	}

}

void RS_DEMO_GDAL::calc_double_Ostu_Threshold_Unit16(unsigned short* Buf, int ImagesizeX, int ImagesizeY, int* posit1, int* posit2, int* topbound)
{

	int histArray[4096] = { 0 };




	float u0, u1, u2;//c0和c1和c2的均值
	float w0 = 0, w1 = 0, w2 = 0;//c0和c1组和c3产生的概率
	int count0, count1;
	int i, t1, t2;
	double v01, v12, v02, ans;
	int thresh1, thresh2;//输入端阈值

	int threshold1, threshold2;//输出端阈值
							   //统计直方图
	for (int i = 0; i < ImagesizeY; i++) {
		for (int j = 0; j < ImagesizeX; j++) {
			if (Buf[i*ImagesizeX + j] != 0)
				histArray[Buf[i*ImagesizeX + j]]++;
		}
	}

	for (int i = 0; i < 4096; i++) {
		if (histArray[i] != 0) {
			thresh2 = i;
		}
		if (histArray[4095 - i] != 0) {
			thresh1 = 4095 - i;
		}
	}



	//统计直方图中像素的个数，存放在sum中
	int sum = 0;
	for (int i = 0; i<4096; i++)
		sum = sum + histArray[i];

	float devi[4095][4095] = { 0 };//存放每个阈值对应的方差，共4095个可能的阈值

	for (t1 = thresh1; t1 < thresh2; t1++) {
		for (t2 = t1 + 1; t2 < thresh2; t2++) {
			if (histArray[t1] <ImagesizeX*ImagesizeY / 30000 || histArray[t2] <ImagesizeX*ImagesizeY / 30000) {
				devi[t1][t2] = ans;
				continue;
			}

			//计算阈值为t1,t2时，c0组的均值和产生的概率
			u0 = 0;
			count0 = 0;//c0组的像素数
			for (i = 0; i <= t1; i++) {
				u0 += i*histArray[i];
				count0 += histArray[i];
			}
			if (count0 != 0)
				u0 = u0 / count0;
			else
				u0 = 0;

			if (sum != 0)
				w0 = (float)count0 / sum;//c0组的概率
										 //计算阈值为t1,t2时，c1组的均值和产生的概率
			else
				w0 = 0;

			u1 = 0;
			count1 = 0;
			for (i = t1 + 1; i < t2 + 1; i++) {
				u1 += i*histArray[i];
				count1 += histArray[i];
				//C0组像素数与C1组像素数之和为图像总像素数。
			}
			if (count1 != 0)
				u1 = u1 / count1;
			if (sum != 0)
				w1 = (float)count1 / sum;
			/////////////
			u2 = 0;

			for (i = t2 + 1; i < 4096; i++) {
				u2 += i*histArray[i];
			}
			if ((sum - count1 - count0) != 0)
				u2 = u2 / (sum - count1 - count0);//C0组像素数与C1组像素数之和为图像总像素数。
			w2 = 1 - w0 - w1;
			//////////////
			//两组间的方差

			v01 = (w0*u0 + w1*u1) / (w0 + w1);

			v12 = (w2*u2 + w1*u1) / (w2 + w1);

			v02 = (w0*u0 + w2*u2) / (w0 + w2);
			ans = (w0 + w1)*(w0*(u0 - v01)*(u0 - v01) + w1*(u1 - v01)*(u1 - v01)) +
				(w1 + w2)*(w1*(u1 - v12)*(u1 - v12) + w2*(u2 - v12)*(u2 - v12)) +
				(w0 + w2)*(w0*(u0 - v02)*(u0 - v02) + w2*(u2 - v02)*(u2 - v02));
			devi[t1][t2] = ans;


		}
	}

	float maxDevi = devi[0][0];
	int maxT = 0;
	for (t1 = thresh1; t1<thresh2; t1++) {
		for (t2 = t1 + 1; t2<thresh2; t2++) {
			if (devi[t1][t2]>maxDevi) {
				maxDevi = devi[t1][t2];
				threshold1 = t1;
				threshold2 = t2;
			}
		}

	}

	*posit1 = threshold1;

	*posit2 = threshold2;
	*topbound = thresh2;
}

void RS_DEMO_GDAL::calc_double_Ostu_Threshold_Unit8(unsigned char* Buf, int ImagesizeX, int ImagesizeY, int* posit1, int* posit2)
{


	int histArray[256] = { 0 };




	float u0, u1, u2;//c0和c1和c2的均值
	float w0 = 0, w1 = 0, w2 = 0;//c0和c1组和c3产生的概率
	int count0, count1;
	int i, t1, t2;
	double v01, v12, v02;
	int thresh1, thresh2;//输入端阈值

	int threshold1, threshold2;//输出端阈值
							   //统计直方图
	for (int i = 0; i < ImagesizeY; i++) {
		for (int j = 0; j < ImagesizeX; j++) {
			if (Buf[i*ImagesizeX + j] != 0)
				histArray[Buf[i*ImagesizeX + j]]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		if (histArray[i] != 0) {
			thresh2 = i;
		}
		if (histArray[255 - i] != 0) {
			thresh1 = 255 - i;
		}
	}



	//统计直方图中像素的个数，存放在sum中
	int sum = 0;
	for (int i = 0; i<256; i++)
		sum = sum + histArray[i];

	float devi[255][255] = { 0 };//存放每个阈值对应的方差，共4095个可能的阈值

	for (t1 = thresh1; t1 < thresh2; t1++) {



		for (t2 = t1 + 1; t2 < thresh2; t2++) {

			//计算阈值为t1,t2时，c0组的均值和产生的概率
			u0 = 0;
			count0 = 0;//c0组的像素数
			for (i = 0; i <= t1; i++) {
				u0 += i*histArray[i];
				count0 += histArray[i];
			}
			if (count0 != 0)
				u0 = u0 / count0;
			else
				u0 = 0;

			if (sum != 0)
				w0 = (float)count0 / sum;//c0组的概率
										 //计算阈值为t1,t2时，c1组的均值和产生的概率
			else
				w0 = 0;

			u1 = 0;
			count1 = 0;
			for (i = t1 + 1; i < t2 + 1; i++) {
				u1 += i*histArray[i];
				count1 += histArray[i];
				//C0组像素数与C1组像素数之和为图像总像素数。
			}
			if (count1 != 0)
				u1 = u1 / count1;
			if (sum != 0)
				w1 = (float)count1 / sum;
			/////////////
			u2 = 0;
			for (i = t2 + 1; i < 4096; i++) {
				u2 += i*histArray[i];
			}
			if ((sum - count1 - count0) != 0)
				u2 = u2 / (sum - count1 - count0);//C0组像素数与C1组像素数之和为图像总像素数。
			w2 = 1 - w0 - w1;
			//////////////
			//两组间的方差

			v01 = (w0*u0 + w1*u1) / (w0 + w1);

			v12 = (w2*u2 + w1*u1) / (w2 + w1);

			v02 = (w0*u0 + w2*u2) / (w0 + w2);

			devi[t1][t2] =
				(w0 + w1)*(w0*(u0 - v01)*(u0 - v01) + w1*(u1 - v01)*(u1 - v01)) +
				(w1 + w2)*(w1*(u1 - v12)*(u1 - v12) + w2*(u2 - v12)*(u2 - v12)) +
				(w0 + w2)*(w0*(u0 - v02)*(u0 - v02) + w2*(u2 - v02)*(u2 - v02));

		}
	}

	float maxDevi = devi[0][0];
	int maxT = 0;
	for (t1 = 1; t1<255; t1++) {
		for (t2 = t1 + 1; t2<255; t2++) {
			if (devi[t1][t2]>maxDevi) {
				maxDevi = devi[t1][t2];
				threshold1 = t1;
				threshold2 = t2;
			}
		}

	}

	*posit1 = threshold1;

	*posit2 = threshold2;



}