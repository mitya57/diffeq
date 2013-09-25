#include <QtGui/QWheelEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>
#include "math.hpp"

class DrawArea: public QWidget {
Q_OBJECT

public:
	DrawArea(PolynomSystem *system, QWidget *parent = 0);
	virtual void paintEvent(QPaintEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

private:
	qreal scale;
	PolynomSystem *system;

	void drawAxes(qreal ticksize);
	void drawFunction();
	void drawPath(PolynomSystem *system, QPointF start, qreal eps);
};

class MyMainWindow: public QMainWindow {
Q_OBJECT

public:
	MyMainWindow(PolynomSystem *system);

private:
	DrawArea drawArea;
	QToolBar toolBar;
};
