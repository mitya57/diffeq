#include <QtCore/QDir>
#include <QtGui/QPainter>
#include "window.hpp"

#define CONVERT_X(x) (x1 + ((x * scale + 1) / 2) * (x2 - x1))
#define CONVERT_Y(y) (y1 + ((y * scale + 1) / 2) * (y2 - y1))
#define CONVERT(point) CONVERT_X(point.x()), CONVERT_Y(point.y())
#define WINDOW qobject_cast<MyMainWindow *>
#define EPS 0.05

MyMainWindow::MyMainWindow(PolynomSystem *system):
	drawArea(system, this),
	fileActionGroup(this)
{
	resize(1000, 800);
	setCentralWidget(&drawArea);
	addToolBar(&toolBar);

	QStringList nameFilters, files;
	nameFilters << "sys*.txt";
	files = QDir().entryList(nameFilters,
		QDir::Files | QDir::Readable);
	for (int i = 0; i < files.size(); ++i) {
		QAction *action = fileActionGroup.addAction(files.at(i));
		action->setCheckable(true);
		toolBar.addAction(action);
	}
	connect(&fileActionGroup, SIGNAL(triggered(QAction*)),
		&drawArea, SLOT(loadFile(QAction*)));
}

DrawArea::DrawArea(PolynomSystem *system, QWidget *parent):
	QWidget(parent),
	scale(.2),
	startPoint(2, 0),
	system(system)
{}

void DrawArea::loadFile(QAction *action) {
	fillSystem(action->text(), *system);
	repaint();
}

void DrawArea::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
	drawAxes(5);
	drawPath(startPoint, EPS);
	qreal staticPoint = system->findPoincareStaticPoint(.1, 5, EPS);
	if (qAbs(staticPoint) > EPS) {
		drawPath(QPointF(staticPoint, 0), EPS);
	}
}

void DrawArea::wheelEvent(QWheelEvent *event) {
	int delta = event->delta();
	scale *= qExp(qreal(delta) / 2048);
	repaint();
}

void DrawArea::mouseMoveEvent(QMouseEvent *event) {
	QRect rectangle = rect();
	int x1 = rectangle.topLeft().x();
	int x2 = rectangle.topRight().x();
	int y1 = rectangle.topLeft().y();
	int y2 = rectangle.bottomLeft().y();
	qreal x = (qreal(event->x() - x1) * 2 / (x2 - x1) - 1) / scale;
	qreal y = (qreal(event->y() - y1) * 2 / (y2 - y1) - 1) / scale;
	startPoint = QPointF(x, y);
	repaint();
}

void DrawArea::mouseReleaseEvent(QMouseEvent *event) {
	mouseMoveEvent(event);
}

void DrawArea::drawAxes(qreal ticksize) {
	QPainter painter(this);
	QRect rectangle = rect();
	painter.setPen(Qt::lightGray);
	int x1 = rectangle.topLeft().x();
	int x2 = rectangle.topRight().x();
	int y1 = rectangle.topLeft().y();
	int y2 = rectangle.bottomLeft().y();
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

void DrawArea::drawPath(QPointF start, qreal eps) {
	QPainter painter(this);
	painter.setPen(Qt::black);
	painter.setRenderHint(QPainter::Antialiasing);
	QRect rectangle = rect();
	QPointF point;
	qreal staticPoint = system->findPoincareStaticPoint(.1, 5, eps);
	int x1 = rectangle.topLeft().x();
	int x2 = rectangle.topRight().x();
	int y1 = rectangle.topLeft().y();
	int y2 = rectangle.bottomLeft().y();
	qreal xdiff = !start.isNull();
	quint32 step = 0;
	while (qAbs(xdiff) > 1e-3 && step < 1000) {
		point = system->getNextValue(start, eps);
		painter.drawLine(CONVERT(start), CONVERT(point));
		if (qAbs(point.y()) < eps && point.x() > 0 && staticPoint > 0) {
			xdiff = point.x() - staticPoint;
		}
		start = point;
		++step;
	}
}
