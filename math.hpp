#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPointF>
#include <QtCore/qmath.h>
#include <complex>

struct Monom {
	quint32 xPow;
	quint32 yPow;
	qreal c;
};

enum PointType {
	PointCenter,
	PointFocusStable,
	PointFocusUnstable,
	PointKnotStable,
	PointKnotUnstable,
	PointKnotSingularStable,
	PointKnotSingularUnstable,
	PointSaddle
};

typedef std::complex<qreal> Complex;
typedef QPair<Complex, Complex> ComplexPair;

class Polynom: public QList<Monom> {
public:
	qreal operator()(qreal x, qreal y) const;
	qreal operator()(QPointF point) const;
};

class PolynomSystem: public QPair<Polynom, Polynom> {
public:
	QPointF getNextValue(QPointF point, qreal eps, bool useRungeKutta = false) const;
	qreal poincareFunction(qreal p, qreal eps) const;
	qreal findPoincareStaticPoint(qreal a, qreal b, qreal eps) const;
	qreal getPeriod(qreal staticPoint, qreal eps) const;
	ComplexPair getEigenValues() const;
	PointType getPointType() const;
};

void fillPolynom(QString line, Polynom &polynom, qreal param = 0);
void fillSystem(QString fileName, PolynomSystem &system, qreal param = 0);
