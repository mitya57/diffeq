#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPointF>
#include <QtCore/qmath.h>

struct Monom {
	quint32 xPow;
	quint32 yPow;
	qreal c;
};

class Polynom: public QList<Monom> {
public:
	qreal operator()(QPointF point);
	void operator+=(const Monom &m);
	void operator+=(const Polynom &p);
};

class PolynomSystem: public QPair<Polynom, Polynom> {
public:
	QPointF getNextValue(QPointF point, qreal eps);
};
