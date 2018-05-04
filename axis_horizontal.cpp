#include "axis_horizontal.hpp"

Axis_Horizontal::Axis_Horizontal(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);
	src = NULL;
	setFixedHeight(30);
	setWindowTitle("Hello DevDiv!");
}

Axis_Horizontal::~Axis_Horizontal() {
	
}

int Axis_Horizontal::getGISinfo(double * dst)
{
	//dst:[lon_start,lat_start,lon_end,lat_end]
	const char* projRef = NULL;
	projRef = src->GetProjectionRef();
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

			*dst= Geo[0] + *dst * Geo[1];
			*(dst + 1) = Geo[3] + *(dst + 1) * Geo[5];
			coordTrans->Transform(1, dst, dst + 1);
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

void Axis_Horizontal::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setPen(QPen(Qt::black, 1));

	//QSize s = this->sizeHint();
	size[0] = this->geometry().height();
	size[1] = this->geometry().width();

	//std::cout << "settings[0]: " << settings[0] << " settings[1]: " << settings[1] << std::endl;

	painter.drawLine(0,size[0],size[1],size[0]);

	int position = 0;
	while (position <= size[1]) {
		painter.setPen(QPen(Qt::black, 2));
		painter.drawLine(position,5,position,size[0]);
		painter.setPen(QPen(Qt::black, 1));

		int subposition = settings[0]*ratio/10;
		while (subposition<settings[0]*ratio)
		{
			painter.drawLine(position+subposition, size[0]-5, position+subposition, size[0]);
			subposition += int(ratio*settings[0] / 10);
		}

		if (src) {
			/*get gis info at
			[startposition[0]+position,startposition[0]]
			pixel*/
			double *geo = new double[2];
			*(geo) = double(startposition[0] + position/ratio);
			*(geo + 1) = double(startposition[1]);
			
			/*******/

			if (getGISinfo(geo) == 0) {
				/*draw text*/
				QFont font;
				font.setPointSize(10);
				painter.setFont(font);
				QFontMetrics fm = painter.fontMetrics();

				QString text = QString::number(abs(*(geo))) + QString::fromLocal8Bit("°") + (*(geo)>0 ? ('E') : ('W'));
				int width = fm.width(text);
				int height = fm.ascent() + fm.descent();

				QRectF rect(position + 1, 1, width, height);
				painter.drawText(rect, text);
				/*****/
			}
			
		}

		position += int(settings[0]*ratio);
	}
}


