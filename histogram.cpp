#include "histogram.hpp"

Histogram::Histogram(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	hist = new QLabel(this);
	Apply = new QPushButton(this);
	Apply->setText(QString::fromLocal8Bit("应用"));
	box = new QComboBox(this);

	formlayout = new QFormLayout();
	formlayout->addRow(QString::fromLocal8Bit("选择波段"), box);

	range = new QLabel(this);
	range->setAlignment(Qt::AlignCenter);

	this->setMaximumHeight(400);
	this->setFixedWidth(500);
	hist->setFixedHeight(320);
	hist->setScaledContents(true);
	hist->show();
	mainlayout = new QVBoxLayout;
	mainlayout->addWidget(range);
	mainlayout->addWidget(hist);
	mainlayout->addLayout(formlayout);
	mainlayout->addWidget(Apply);
	mainlayout->setSpacing(5);
	setLayout(mainlayout);

	connect(Apply, SIGNAL(clicked()), this, SLOT(update_view()));
}

Histogram::~Histogram() {
	
}

void Histogram::init_combo()
{
	box->clear();

	hist->clear();

	for (int i = 1; i <= src->GetRasterCount(); i++) {
		GDALRasterBand *pBand = src->GetRasterBand(i);
		int min, max;
		double adfMinMax[2] = {0,0};
		adfMinMax[0] = pBand->GetMinimum(&min);
		adfMinMax[1] = pBand->GetMaximum(&max);
		string banddescription;

		banddescription = "波段 " + to_string(i) + ": " + '[' + to_string(int(adfMinMax[0])) + ','
			+ to_string(int(adfMinMax[1])) + ']';
		box->addItem(QString::fromLocal8Bit(banddescription.c_str()));
	}
}

void Histogram::update_view()
{
	int band = box->currentIndex()+1;

	int xsize = src->GetRasterXSize();
	int ysize = src->GetRasterYSize();

	GDALDataType type= src->GetRasterBand(1)->GetRasterDataType();

	switch (type)
	{
	case 1: {
		typedef GByte DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2,size)-1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[*(data + i*xsize + j)]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 2: {
		typedef ushort DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[*(data + i*xsize + j)]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 3: {
		typedef short DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout <<"最高灰度级："<< pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);


		double mmin = 0;
		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[abs(*(data + i*xsize + j))]++;
			}
		}
		
		double mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 4: {
		typedef uint DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[*(data + i*xsize + j)]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 5: {
		typedef int DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[abs(*(data + i*xsize + j))]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 6: {
		typedef float DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[abs(long(*(data + i*xsize + j)))]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	case 7: {
		typedef double DATATYPE;
		/*get highest bit*/
		int size = 0;
		DATATYPE* data = new DATATYPE[xsize*ysize];
		DATATYPE max = 0;
		GDALRasterBand *pSrcBandImg = src->GetRasterBand(band);		//GDALDataSet->GDALRasterBand
		pSrcBandImg->RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);
		for (int i = 0; i < xsize*ysize; i++)
		{
			if (*data>max)
				max = *data;
			data++;
		}
		data -= xsize*ysize;
		while (max >= 1)
		{
			size++;
			max = max / 2;
		}
		/**************/

		string banddescription;

		banddescription = "波段 " + to_string(band) + ": " + '[' + to_string(int(0)) + ','
			+ to_string(int(pow(2, size) - 1)) + ']';

		range->setText(QString::fromLocal8Bit(banddescription.c_str()));

		cout << pow(2, size) << endl;

		double *histo = new double[pow(2, size)];
		for (int i = 0; i < pow(2, size); i++) {
			*(histo + i) = 0;
		}

		src->GetRasterBand(band)->
			RasterIO(GF_Read, 0, 0, xsize, ysize, data, xsize, ysize, type, 0, 0);

		for (int i = 0; i < ysize; i++) {
			for (int j = 0; j < xsize; j++) {
				histo[abs(long(*(data + i*xsize + j)))]++;
			}
		}

		float mmax = 0;
		for (int i = 0; i < pow(2, size); i++) {
			histo[i] /= (xsize*ysize);
			if (histo[i] > mmax)
				mmax = histo[i];
		}

		Mat temp(256, pow(2, size), CV_8U, Scalar(0));
		for (int i = 0; i < pow(2, size); i++) {
			int hight = int(256 * (histo[i] / mmax));
			rectangle(temp, Point(i, 255), Point(i, 256 - hight), Scalar(255));
		}

		image = QImage((uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Grayscale8);

		hist->setPixmap(QPixmap::fromImage(image));
		delete[]data;
		delete[]histo;
		break;
	}
	}
}

