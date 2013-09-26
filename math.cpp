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
