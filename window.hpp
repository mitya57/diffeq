#include <QtGui/QWheelEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>
#include "math.hpp"

class DrawArea: public QWidget {
Q_OBJECT

public:
	DrawArea(PolynomSystem *system, QWidget *parent = 0);
	virtual void paintEvent(QPaintEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void loadFile(QAction *action);

private:
	qreal scale;
	QPointF startPoint;
	PolynomSystem *system;

	void drawAxes(qreal ticksize);
	void drawFunction();
	void drawPath(QPointF start, qreal eps);
};

class MyMainWindow: public QMainWindow {
Q_OBJECT

public:
	MyMainWindow(PolynomSystem *system);

private:
	DrawArea drawArea;
	QToolBar toolBar;
	QActionGroup fileActionGroup;
};
