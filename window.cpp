#include <QtCore/QDir>
#include <QtGui/QPainter>
#include "window.hpp"

#define CONVERT_X(x) (x1 + ((x * scale + 1) / 2) * (x2 - x1))
#define CONVERT_Y(y) (y1 + ((y * scale + 1) / 2) * (y2 - y1))
#define CONVERT(point) CONVERT_X(point.x()), CONVERT_Y(point.y())
#define CONVERTBACK_X(x) ((qreal(x - x1) * 2 / (x2 - x1) - 1) / scale)
#define CONVERTBACK_Y(y) ((qreal(y - y1) * 2 / (y2 - y1) - 1) / scale)
#define DEFINE_RECT QRect rectangle = rect(); \
	int x1 = rectangle.topLeft().x(); \
	int x2 = rectangle.topRight().x(); \
	int y1 = rectangle.topLeft().y(); \
	int y2 = rectangle.bottomLeft().y()
#define POINTS 5
#define EPS 0.05

MyMainWindow::MyMainWindow(PolynomSystem *system):
	drawArea(system, this),
	toolBar("Toolbar", this),
	drawMeshAction(tr("Draw Mesh"), this),
	fileActionGroup(this),
	paramSlider(Qt::Horizontal, this)
{
	resize(1000, 800);
	setCentralWidget(&drawArea);
	addToolBar(&toolBar);
	setWindowTitle(tr("Differential equations"));

	QStringList nameFilters, files;
	nameFilters << "sys*.txt";
	files = QDir().entryList(nameFilters,
		QDir::Files | QDir::Readable);
	for (int i = 0; i < files.size(); ++i) {
		QAction *action = fileActionGroup.addAction(files.at(i));
		action->setCheckable(true);
		toolBar.addAction(action);
		if (!i) {
			action->setChecked(true);
			drawArea.loadFile(action);
		}
	}
	connect(&fileActionGroup, SIGNAL(triggered(QAction*)),
		&drawArea, SLOT(loadFile(QAction*)));
	paramSlider.setTickInterval(50);
	paramSlider.setTickPosition(QSlider::TicksBothSides);
	paramSlider.setRange(0, 300);
	paramSlider.setValue(150);
	connect(&paramSlider, SIGNAL(valueChanged(int)),
		&drawArea, SLOT(updateParam(int)));

	drawMeshAction.setCheckable(true);
	toolBar.addSeparator();
	toolBar.addAction(&drawMeshAction);
	connect(&drawMeshAction, SIGNAL(triggered(bool)),
		&drawArea, SLOT(drawMesh(bool)));
	toolBar.addSeparator();
	toolBar.addWidget(&paramSlider);
}

DrawArea::DrawArea(PolynomSystem *system, QWidget *parent):
	QWidget(parent),
	scale(.2),
	param(1),
	doDrawMesh(false),
	startPoint(2, 0),
	system(system)
{}

void DrawArea::loadFile(QAction *action) {
	fileName = action->text();
	updateSystem();
}

void DrawArea::updateParam(int sliderParam) {
	param = sliderParam * .02 - 2;
	updateSystem();
}

void DrawArea::updateSystem() {
	fillSystem(fileName, *system, param);
	PointType pointType = system->getPointType();
	pointTypeName = getPointTypeName(pointType);
	repaint();
}

QString DrawArea::getPointTypeName(PointType type) {
	switch(type) {
		case PointCenter:        return tr("Center");
		case PointFocusStable:   return tr("Stable focus");
		case PointFocusUnstable: return tr("Unstable focus");
		case PointKnotStable:    return tr("Stable knot");
		case PointKnotUnstable:  return tr("Unstable knot");
		case PointSaddle:        return tr("Saddle");
	}
	return QString();
}

void DrawArea::drawMesh(bool yes) {
	doDrawMesh = yes;
	repaint();
}

void DrawArea::paintEvent(QPaintEvent *event) {
	DEFINE_RECT;
	QWidget::paintEvent(event);
	drawAxes(5);
	qreal staticPoint = system->findPoincareStaticPoint(.1, 5, EPS);
	if (qAbs(staticPoint) > EPS) {
		drawPath(QPointF(staticPoint, 0), EPS);
	}
	if (doDrawMesh) {
		for (int i = 0; i <= POINTS; ++i) {
			drawPath(QPointF(CONVERTBACK_X((x1 * i + x2 * (POINTS - i)) / POINTS),
				CONVERTBACK_Y(y1)), EPS);
			drawPath(QPointF(CONVERTBACK_X((x1 * i + x2 * (POINTS - i)) / POINTS),
				CONVERTBACK_Y(y2)), EPS);
		}
	} else {
		drawPath(startPoint, -EPS, Qt::lightGray);
		drawPath(startPoint, EPS);
	}
	QString statusText = tr("Parameter: %1;  Point type: %2;  Scale: %3").arg(
		QString::number(param, 'f', 2),
		pointTypeName, QString::number(scale * 5, 'g', 3));
	QPainter(this).drawText(10, height() - 10, statusText);
}

void DrawArea::wheelEvent(QWheelEvent *event) {
	int delta = event->delta();
	qreal newScale = scale * qExp(qreal(delta) / 2048);
	if (newScale > 0.05 && newScale < 30) {
		scale = newScale;
		repaint();
	}
}

void DrawArea::mouseMoveEvent(QMouseEvent *event) {
	DEFINE_RECT;
	qreal x = CONVERTBACK_X(event->x());
	qreal y = CONVERTBACK_Y(event->y());
	startPoint = QPointF(x, y);
	repaint();
}

void DrawArea::mouseReleaseEvent(QMouseEvent *event) {
	mouseMoveEvent(event);
}

void DrawArea::drawAxes(qreal ticksize) {
	DEFINE_RECT;
	QPainter painter(this);
	painter.setPen(Qt::lightGray);
	painter.drawLine((x1+x2)/2, y1, (x1+x2)/2, y2);
	painter.drawLine(x1, (y1+y2)/2, x2, (y1+y2)/2);
	qreal newx = 0, startx = CONVERT_X(0), starty = CONVERT_Y(0);
	for (int x = 0; startx+newx < x2; ++x) {
		newx = x * scale * (x2 - x1) / 2;
		painter.drawLine(startx+newx, starty-ticksize, startx+newx, starty+ticksize);
		painter.drawLine(startx-newx, starty-ticksize, startx-newx, starty+ticksize);
	}
	qreal newy = 0;
	for (int y = 0; starty+newy < y2; ++y) {
		newy = y * scale * (y2 - y1) / 2;
		painter.drawLine(startx-ticksize, starty+newy, startx+ticksize, starty+newy);
		painter.drawLine(startx-ticksize, starty-newy, startx+ticksize, starty-newy);
	}
}

void DrawArea::drawPath(QPointF start, qreal eps, QColor color) {
	DEFINE_RECT;
	QPainter painter(this);
	painter.setPen(color);
	painter.setRenderHint(QPainter::Antialiasing);
	QPointF point;
	qreal staticPoint = system->findPoincareStaticPoint(.1, 5, qAbs(eps));
	qreal xdiff = !start.isNull();
	quint32 step = 0;
	while (qAbs(xdiff) > 1e-3 && step < 1000) {
		point = system->getNextValue(start, eps);
		painter.drawLine(CONVERT(start), CONVERT(point));
		if (qAbs(point.y()) < qAbs(eps) && point.x() > 0 && staticPoint > 0) {
			xdiff = point.x() - staticPoint;
		}
		start = point;
		++step;
	}
}
