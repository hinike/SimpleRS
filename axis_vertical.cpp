#include "axis_vertical.hpp"

Axis_Vertical::Axis_Vertical(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);
	src = NULL;
	setFixedWidth(30);
}

Axis_Vertical::~Axis_Vertical() {
	
}

int Axis_Vertical::getGISinfo(double * dst)
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

			*dst = Geo[0] + *dst * Geo[1];
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

void Axis_Vertical::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setPen(QPen(Qt::black, 1));

	//QSize s = this->sizeHint();
	size[0] = this->geometry().height();
	size[1] = this->geometry().width();

	painter.drawLine(size[1], 0, size[1], size[0]);

	int position = 0;
	while (position <= size[0]) {
		painter.setPen(QPen(Qt::black, 2));
		painter.drawLine(5, position, size[0], position);
		painter.setPen(QPen(Qt::black, 1));
		int subposition = settings[0]*ratio / 10;
		while (subposition<settings[0]*ratio)
		{
			painter.drawLine(size[1]-4, position + subposition, size[0], position + subposition);
			subposition += int(ratio*settings[0] / 10);
		}
		
		if (src) {
			/*get gis info at
			[startposition[0]+position,startposition[0]]
			pixel*/
			double *geo = new double[2];
			*(geo) = double(startposition[0]);
			*(geo + 1) = double(startposition[1]+position/ratio);


			/*******/

			if (getGISinfo(geo) == 0) {
				/*draw text*/

				painter.save();
				/*test draw text*/
				QFont font;
				font.setPointSize(10);
				painter.setFont(font);
				QFontMetrics fm = painter.fontMetrics();
				QString text = QString::number(abs(*(geo+1))) + QString::fromLocal8Bit("°") + (*(geo+1)>0 ? ('N') : ('S'));
				int width = fm.width(text);
				int height = fm.ascent() + fm.descent();

				QTransform transform;
				transform.translate(1 + height, position + 1 + width);
				transform.rotate(-90);
				transform.translate(-1 - height, -(position + 1 + width));
				painter.setTransform(transform);
				painter.drawText(QPointF(1 + height, position + 1 + width), text);

				painter.restore();
			}

		}
		position += int(settings[0]*ratio);
	}
}
