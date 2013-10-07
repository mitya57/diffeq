#include <QtGui/QWheelEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
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
	void updateParam(int sliderParam);
	void drawMesh(bool yes);

private:
	qreal scale;
	qreal param;
	bool doDrawMesh;
	QPointF startPoint;
	PolynomSystem *system;
	QString pointTypeName;
	QString fileName;

	void drawAxes(qreal ticksize);
	void drawPath(QPointF start, qreal eps, QColor color = Qt::black);
	QString getPointTypeName(PointType type);
	void updateSystem();
};

class MyMainWindow: public QMainWindow {
Q_OBJECT

public:
	MyMainWindow(PolynomSystem *system);

private:
	DrawArea drawArea;
	QToolBar toolBar;
	QAction drawMeshAction;
	QActionGroup fileActionGroup;
	QSlider paramSlider;
};
