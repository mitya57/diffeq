#include <QtGui/QPainter>
#include "window.hpp"

#define CONVERT_X(x) x1 + ((x * scale + 1) / 2) * (x2 - x1)
#define CONVERT_Y(y) y1 + ((y * scale + 1) / 2) * (y2 - y1)
#define CONVERT(point) CONVERT_X(point.x()), CONVERT_Y(point.y())
#define WINDOW qobject_cast<MyMainWindow *>

MyMainWindow::MyMainWindow(PolynomSystem *system):
	drawArea(system, this)
{
	resize(1000, 800);
	setCentralWidget(&drawArea);
	addToolBar(&toolBar);
}

DrawArea::DrawArea(PolynomSystem *system, QWidget *parent):
	QWidget(parent),
	scale(.2),
	system(system)
{}

void DrawArea::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
	drawAxes(5);
	drawPath(system, QPointF(2, 0), 0.05);
	drawPath(system, QPointF(-2, 0), 0.05);
}

void DrawArea::wheelEvent(QWheelEvent *event) {
	int delta = event->delta();
	scale *= qExp(double(delta) / 2048);
	repaint();
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

void DrawArea::drawPath(PolynomSystem *system, QPointF start, qreal eps) {
	QPainter painter(this);
	painter.setPen(Qt::black);
	QRect rectangle = rect();
	QPointF point;
	int x1 = rectangle.topLeft().x();
	int x2 = rectangle.topRight().x();
	int y1 = rectangle.topLeft().y();
	int y2 = rectangle.bottomLeft().y();
	for (int i = 0; i < 1000; ++i) {
		point = system->getNextValue(start, eps);
		painter.drawLine(CONVERT(start), CONVERT(point));
		start = point;
	}
}
