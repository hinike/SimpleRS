#include "display_change.hpp"

display_change::display_change(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

	dischange = new QLabel(this);
	Apply = new QPushButton(this);
	Apply->setText("Apply");
	box = new QComboBox(this);

	formlayout = new QFormLayout();
	formlayout->addRow("Select Band:", box);


	mainlayout = new QVBoxLayout;
	mainlayout->addWidget(dischange);
	mainlayout->addLayout(formlayout);
	mainlayout->addWidget(Apply);
	setLayout(mainlayout);

	connect(Apply, SIGNAL(clicked()), this, SLOT(update_view()));
}

display_change::~display_change() {

}

void display_change::init_set()
{
	box->clear();
	dischange->clear();
	for (int i = 1; i <= src1->GetRasterCount(); i++)
	{
		GDALRasterBand *pBand = src1->GetRasterBand(i);
		int min, max;
		double adfMinMax[2];
		adfMinMax[0] = pBand->GetMinimum(&min);
		adfMinMax[1] = pBand->GetMaximum(&max);
		if (min&&max)
		{
			string banddescription;
			banddescription = "Band" + to_string(i) + ":" + '[' + to_string(adfMinMax[0]) + ',' +
				to_string(adfMinMax[1]) + ']';
			box->addItem(QString::fromStdString(banddescription));

		}
		else
		{
			string banddescription;
			banddescription = "Band" + to_string(i) + ":This band has no band info.";
			box->addItem(QString::fromStdString(banddescription));
		}
	}
}

void display_change::update_view()
{
	int band = box->currentIndex() + 1;

	int XSize = src1->GetRasterXSize();
	int YSize = src1->GetRasterYSize();

	GDALDataType type = src1->GetRasterBand(1)->GetRasterDataType();
	if (type == GDT_Byte)
	{
		GDALRasterBand *poBand_before = src1->GetRasterBand(band);
		GDALRasterBand *poBand_after = src2->GetRasterBand(band);
		Mat A = cv::Mat(YSize, XSize, CV_8UC1);
		Mat B = cv::Mat(YSize, XSize, CV_8UC1);
		uchar *data_A, *data_B;
		uchar *linedata_A = new uchar[XSize];
		uchar *linedata_B = new uchar[XSize];
		//数据读入矩阵
		for (int i = 0; i < YSize; i++)
		{
			data_A = A.ptr<uchar>(i);//获取第i行的首地址
									 //数据读入
			poBand_before->RasterIO(GF_Read, 0, i, XSize, 1, linedata_A, XSize, 1, GDT_Byte, 0, 0);
			data_B = B.ptr<uchar>(i);
			poBand_after->RasterIO(GF_Read, 0, i, XSize, 1, linedata_B, XSize, 1, GDT_Byte, 0, 0);
			for (int j = 0; j < XSize; j++)
			{
				data_A[j] = linedata_A[j];
				data_B[j] = linedata_B[j];
			}
		}

		//数据读入Mat A,B里面了
		Mat i_b, i_a, i_before, i_after, i_cha, i_bi, Y1, Y2, Y3, Y4, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8;

		GaussianBlur(A, A, Size(3, 3), 0, 0);
		GaussianBlur(B, B, Size(3, 3), 0, 0);
		i_b = A.clone();
		i_a = B.clone();
		i_b.convertTo(i_before, CV_64F);
		i_a.convertTo(i_after, CV_64F);

		//得到差值比值图片
		int i_Height = i_b.rows;
		int i_Width = i_b.cols;
		i_cha = abs(i_before - i_after);
		divide((i_before + 1), (i_after + 1), i_bi);
		log(i_bi, i_bi);
		i_bi = abs(i_bi);

		double lambda = 0;

		F_lrr_2_origin(i_before, i_after, lambda, &Y1, &Y2, &Y3, &Y4);

		F_lrr_4_origin(i_cha, i_bi, Y3, Y4, lambda, &Z1, &Z2, &Z3, &Z4, &Z5, &Z6, &Z7, &Z8);

		Mat Z3_GY;
		F_GY(Z3, &Z3_GY);
		//Mat OUT;
		F_kmeans(Z3_GY, &OUT);
		medianBlur(OUT, OUT, 3);
		//delete[]linedata_A;
		//delete[]linedata_B;
		//imshow("OUT", OUT);
		cout << "&out " << &OUT << endl;
		img = QImage((uchar*)OUT.data, OUT.cols, OUT.rows, OUT.step, QImage::Format_Grayscale8);

		dischange->setPixmap(QPixmap::fromImage(img));
		delete[]linedata_A;
		delete[]linedata_B;
	}
	else if (type == GDT_UInt16)
	{
		GDALRasterBand *poBand_before = src1->GetRasterBand(band);
		GDALRasterBand *poBand_after = src2->GetRasterBand(band);
		Mat A = cv::Mat(YSize, XSize, CV_16U);
		Mat B = cv::Mat(YSize, XSize, CV_16U);
		uchar *data_A, *data_B;
		uchar *linedata_A = new uchar[XSize];
		uchar *linedata_B = new uchar[XSize];
		//数据读入矩阵
		for (int i = 0; i < YSize; i++)
		{
			data_A = A.ptr<uchar>(i);//获取第i行的首地址
									 //数据读入
			poBand_before->RasterIO(GF_Read, 0, i, XSize, 1, linedata_A, XSize, 1, GDT_Byte, 0, 0);
			data_B = B.ptr<uchar>(i);
			poBand_after->RasterIO(GF_Read, 0, i, XSize, 1, linedata_B, XSize, 1, GDT_Byte, 0, 0);
			for (int j = 0; j < XSize; j++)
			{
				data_A[j] = linedata_A[j];
				data_B[j] = linedata_B[j];
			}
		}

		//数据读入Mat A,B里面了
		Mat i_b, i_a, i_before, i_after, i_cha, i_bi, Y1, Y2, Y3, Y4, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8;

		GaussianBlur(A, A, Size(3, 3), 0, 0);
		GaussianBlur(B, B, Size(3, 3), 0, 0);
		i_b = A.clone();
		i_a = B.clone();
		i_b.convertTo(i_before, CV_64F);
		i_a.convertTo(i_after, CV_64F);

		//得到差值比值图片
		int i_Height = i_b.rows;
		int i_Width = i_b.cols;
		i_cha = abs(i_before - i_after);
		divide((i_before + 1), (i_after + 1), i_bi);
		log(i_bi, i_bi);
		i_bi = abs(i_bi);

		double lambda = 0;

		F_lrr_2_origin(i_before, i_after, lambda, &Y1, &Y2, &Y3, &Y4);

		F_lrr_4_origin(i_cha, i_bi, Y3, Y4, lambda, &Z1, &Z2, &Z3, &Z4, &Z5, &Z6, &Z7, &Z8);

		Mat Z3_GY;
		F_GY(Z3, &Z3_GY);
		Mat OUT;
		F_kmeans(Z3_GY, &OUT);
		medianBlur(OUT, OUT, 3);
		img = QImage((uchar*)OUT.data, OUT.cols, OUT.rows, OUT.step, QImage::Format_Grayscale8);

		dischange->setPixmap(QPixmap::fromImage(img));
		delete[]linedata_A;
		delete[]linedata_B;
	}
	else
	{
		QMessageBox::warning(this, tr("Sorry"),
			QString::fromLocal8Bit("暂不支持此种数据类型的处理"));
		return;
	}
}


void display_change::lrr(Mat X, double lambda, Mat *Z, Mat *E)
{
	//参数设置
	double tol = 1e-8;
	double maxIter = 1e6;
	int d = X.rows;//行
	int n = X.cols;
	double rho = 1.1;
	double max_mu = 1e30;
	double mu = 1e-6;
	Mat xtx = X.t()*X;//xtx成功
					  //矩阵求逆

	Mat XX = xtx + Mat::eye(n, n, CV_64F);
	Mat inv_X = XX.inv();
	//矩阵求逆成功
	//Initializing optimization variables
	Mat J = Mat::zeros(n, n, CV_64F);
	*Z = Mat::zeros(n, n, CV_64F);
	*E = Mat::zeros(d, n, CV_64F);
	Mat Y1 = Mat::zeros(d, n, CV_64F);
	Mat Y2 = Mat::zeros(n, n, CV_64F);

	////Start main loop
	int iter = 0;
	while (iter <1)
	{
		iter = iter + 1;
		Mat temp = *Z + Y2 / mu;

		Mat U, sigma, V, sigma1;


		SVDecomp(temp, sigma, U, V, 0);//结果和matlab里面不同呢,sigma是列向量，自己转换成矩阵
		Mat S(sigma.rows, sigma.rows, CV_64FC1, Scalar(0));
		for (int i = 0; i < sigma.rows; i++)
		{
			S.at<double>(i, i) = sigma.at<double>(i, 0);
		}


		sigma = sigma.diag();
		int svp = 0;
		for (int i = 0; i < sigma.rows; i++)
		{
			if (sigma.at<double>(i, 0) > 1 / mu)
				svp = svp + 1;
		}
		//cout << svp<<endl;
		if (svp >= 1)
		{
			for (int i = 0; i < svp; i++)
			{
				sigma.at<double>(i) = sigma.at<double>(i) - 1 / mu;
			}
		}
		else
		{
			svp = 1;
			sigma = Mat::zeros(1, 1, CV_64F);
		}

		J = U(Rect(0, 0, svp, U.rows))*sigma.diag()*V(Rect(0, 0, svp, V.rows)).t();
		*Z = inv_X*(xtx - X.t()*(*E) + J + (X.t()*Y1 - Y2) / mu);
		//X90601行，2列，z2行2列
		Mat xmaz = X - X*(*Z);
		temp = X - X*(*Z) + Y1 / mu;
		//以上没有问题了

		solve_l1l2(temp, lambda / mu, E);


		Mat leq1 = xmaz - *E;
		Mat leq2 = *Z - J;

		double minv1 = 0.0, maxv1 = 0.0;
		double* minp1 = &minv1;
		double* maxp1 = &maxv1;

		minMaxIdx(abs(leq1), minp1, maxp1);
		double minv2 = 0.0, maxv2 = 0.0;
		double* minp2 = &minv2;
		double* maxp2 = &maxv2;

		minMaxIdx(abs(leq2), minp2, maxp2);
		double stopC = max(maxv1, maxv2);
		if (stopC < tol)
		{
			break;
			cout << iter << endl;

		}
		else
		{
			Y1 = Y1 + mu*leq1;
			Y2 = Y2 + mu*leq2;
			mu = min(max_mu, mu*rho);
		}
	}

}
void display_change::solve_l1l2(Mat W, double lambda, Mat *E)
{
	int n = W.cols;
	*E = W;
	for (int i = 0; i < (*E).cols; i++)
	{
		Mat SS;
		Mat WW(W.rows, 1, CV_64F);
		WW = W(Rect(i, 0, 1, W.rows));
		Mat WWW;
		WWW = WW.clone();
		//cout<< "WWW" << WWW.at<double>(304, 0) << endl;
		//以上没有问题了
		solve_l2(WWW, lambda, &SS);
		(*E)(Rect(i, 0, 1, (*E).rows)) = SS.clone();

	}
}

void display_change::solve_l2(Mat w, double lambda, Mat *x)
{
	double nw = norm(w, 4);
	if (nw > lambda)
	{
		*x = (nw - lambda)*w / nw;
	}
	else
	{
		*x = Mat::zeros(w.rows, 1, CV_64F);
	}
}
void display_change::F_lrr_2_origin(Mat Inimg1, Mat Inimg2, double lambda, Mat *Y1, Mat *Y2, Mat *Y3, Mat *Y4)
{
	int iHeight = Inimg1.rows;
	int iWidth = Inimg1.cols;

	Mat v1, v2;
	//因为存储机制不同，所以reshape之前需要转置，转置只是为了reshape
	Inimg1 = Inimg1.t();
	Inimg2 = Inimg2.t();
	v1 = Inimg1.reshape(0, iHeight*iWidth);
	v2 = Inimg2.reshape(0, iHeight*iWidth);
	v1.convertTo(v1, CV_64F);
	v2.convertTo(v2, CV_64F);
	//reshape成功
	Mat X = Mat::zeros(iHeight*iWidth, 2, CV_64F);//行列
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 0) = v1.at<double>(i, 0);
	}
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 1) = v2.at<double>(i, 0);
	}
	//构造X成功
	Mat Z, E;
	lrr(X, lambda, &Z, &E);
	cout << E.at<double>(304, 0) << endl;
	cout << Z.at<double>(0, 0) << endl;
	Mat V1 = X*Z;
	Mat V2 = E;

	Mat y1 = V1(Rect(0, 0, 1, V1.rows)).clone();
	Mat y2 = V1(Rect(1, 0, 1, V1.rows)).clone();
	Mat y3 = V2(Rect(0, 0, 1, V1.rows)).clone();
	Mat y4 = V2(Rect(1, 0, 1, V1.rows)).clone();
	*Y1 = y1.reshape(0, iWidth);
	*Y2 = y2.reshape(0, iWidth);
	*Y3 = y3.reshape(0, iWidth);
	*Y4 = y4.reshape(0, iWidth);
	*Y1 = (*Y1).t();
	*Y2 = (*Y2).t();
	*Y3 = (*Y3).t();
	*Y4 = (*Y4).t();
}


void display_change::F_lrr_4_origin(Mat Inimg1, Mat Inimg2, Mat Inimg3, Mat Inimg4, double lambda, Mat *Y1, Mat *Y2, Mat *Y3, Mat *Y4, Mat *Y5, Mat *Y6, Mat *Y7, Mat *Y8)
{
	int iHeight = Inimg1.rows;
	int iWidth = Inimg1.cols;
	Inimg1 = Inimg1.t();
	Inimg2 = Inimg2.t();
	Inimg3 = Inimg3.t();
	Inimg4 = Inimg4.t();
	Mat v1, v2, v3, v4;
	v1 = Inimg1.reshape(0, iHeight*iWidth);
	v2 = Inimg2.reshape(0, iHeight*iWidth);
	v3 = Inimg3.reshape(0, iHeight*iWidth);
	v4 = Inimg4.reshape(0, iHeight*iWidth);
	v1.convertTo(v1, CV_64F);
	v2.convertTo(v2, CV_64F);
	v3.convertTo(v3, CV_64F);
	v4.convertTo(v4, CV_64F);
	Mat X = Mat::zeros(iHeight*iWidth, 4, CV_64F);
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 0) = v1.at<double>(i, 0);
	}
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 1) = v2.at<double>(i, 0);
	}
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 2) = v3.at<double>(i, 0);
	}
	for (int i = 0; i < X.rows; i++)
	{
		X.at<double>(i, 3) = v4.at<double>(i, 0);
	}
	Mat Z, E;
	lrr(X, lambda, &Z, &E);
	cout << E.at<double>(304, 0) << endl;
	cout << Z.at<double>(0, 0) << endl;
	Mat V1 = X*Z;
	Mat V2 = E;

	Mat y1 = V1(Rect(0, 0, 1, V1.rows)).clone();
	Mat y2 = V1(Rect(1, 0, 1, V1.rows)).clone();
	Mat y3 = V1(Rect(2, 0, 1, V1.rows)).clone();
	Mat y4 = V1(Rect(3, 0, 1, V1.rows)).clone();
	Mat y5 = V2(Rect(0, 0, 1, V1.rows)).clone();
	Mat y6 = V2(Rect(1, 0, 1, V1.rows)).clone();
	Mat y7 = V2(Rect(2, 0, 1, V1.rows)).clone();
	Mat y8 = V2(Rect(3, 0, 1, V1.rows)).clone();
	*Y1 = y1.reshape(0, iWidth);
	*Y2 = y2.reshape(0, iWidth);
	*Y3 = y3.reshape(0, iWidth);
	*Y4 = y4.reshape(0, iWidth);
	*Y5 = y5.reshape(0, iWidth);
	*Y6 = y6.reshape(0, iWidth);
	*Y7 = y7.reshape(0, iWidth);
	*Y8 = y8.reshape(0, iWidth);
	*Y1 = (*Y1).t();
	*Y2 = (*Y2).t();
	*Y3 = (*Y3).t();
	*Y4 = (*Y4).t();
	*Y5 = (*Y5).t();
	*Y6 = (*Y6).t();
	*Y7 = (*Y7).t();
	*Y8 = (*Y8).t();
}

void display_change::F_GY(Mat IN, Mat *OUT)
{
	double MAX, MIN;
	double *maxp = &MAX;
	double *minp = &MIN;
	minMaxIdx(IN, minp, maxp);
	int iHeight, iWidth;
	iHeight = IN.rows;
	iWidth = IN.cols;
	Mat OUT1;
	OUT1 = Mat::zeros(iHeight, iWidth, CV_64F);
	int k, l;
	for (k = 0; k < IN.rows; k++)
	{
		for (l = 0; l < IN.cols; l++)
		{
			OUT1.at<double>(k, l) = (IN.at<double>(k, l) - MIN) / (MAX - MIN) * 255;
		}
	}
	OUT1.convertTo((*OUT), CV_8U);
}
void display_change::F_kmeans(Mat IN, Mat *OUT)
{
	int Height = IN.rows;
	int Width = IN.cols;
	Mat IN1;
	IN1 = IN.t();
	IN1 = IN1.reshape(0, Height*Width);
	IN1.convertTo(IN1, CV_32F);
	cout << "IN1" << IN1.at<double>(5, 0) << endl;
	Mat label;
	kmeans(IN1, 2, label, TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 10, 1.0), 20, KMEANS_RANDOM_CENTERS);

	*OUT = Mat::zeros(IN.size(), CV_64F);
	int k, l;
	//没有办法给OUT1赋值
	for (k = 0; k < IN.rows; k++)
	{
		for (l = 0; l < IN.cols; l++)
		{
			int index = (k*IN.cols) + l;
			if (label.at<int>(index) == 1)
			{
				(*OUT).at<double>(k, l) = 255;
			}
			else
			{
				(*OUT).at<double>(k, l) = 0;
			}
		}
	}
	*OUT = (*OUT).reshape(0, Width);
	*OUT = (*OUT).t();
	(*OUT).convertTo((*OUT), CV_8U);
}
