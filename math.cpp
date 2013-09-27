#include "math.hpp"

qreal Polynom::operator()(QPointF point) {
	qreal result = 0;
	for (ConstIterator i = begin(); i != end(); ++i) {
		result += qPow(point.x(), i->xPow) * qPow(point.y(), i->yPow) * i->c;
	}
	return result;
}

QPointF PolynomSystem::getNextValue(QPointF point, qreal eps) {
	QPointF result = point;
	result.setX(result.x() + first(point) * eps);
	result.setY(result.y() + second(point) * eps);
	return result;
}

qreal PolynomSystem::poincareFunction(qreal p, qreal eps) {
	QPointF point(p, 0);
	quint32 step = 0;
	while (step < 10 || point.x() < 0 || qAbs(point.y()) > eps) {
		point = getNextValue(point, eps);
		++step;
	}
	return point.x();
}

qreal PolynomSystem::findPoincareStaticPoint(qreal a, qreal b, qreal eps) {
	qreal pdiffa, pdiffb;
	while (qAbs(b - a) > eps) {
		pdiffa = poincareFunction(a, eps) - a;
		pdiffb = poincareFunction(b, eps) - b;
		a = b - (b - a) * pdiffb / (pdiffb - pdiffa);
		pdiffa = poincareFunction(a, eps) - a;
		b = a - (a - b) * pdiffa / (pdiffa - pdiffb);
	}
	return b;
}
