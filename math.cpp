#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtGui/QVector2D>
#include "math.hpp"

qreal Polynom::operator()(qreal x, qreal y) const {
	qreal result = 0;
	for (ConstIterator i = begin(); i != end(); ++i) {
		result += qPow(x, i->xPow) * qPow(y, i->yPow) * i->c;
	}
	return result;
}

qreal Polynom::operator()(QPointF point) const {
	return operator()(point.x(), point.y());
}

Polynom Polynom::differentiateX() const {
	Polynom result;
	Monom m;
	for (ConstIterator i = begin(); i != end(); ++i) {
		if (i->xPow) {
			m.xPow = i->xPow - 1;
			m.yPow = i->yPow;
			m.c = i->c * i->xPow;
			result.push_back(m);
		}
	}
	return result;
}

Polynom Polynom::differentiateY() const {
	Polynom result;
	Monom m;
	for (ConstIterator i = begin(); i != end(); ++i) {
		if (i->yPow) {
			m.xPow = i->xPow;
			m.yPow = i->yPow - 1;
			m.c = i->c * i->yPow;
			result.push_back(m);
		}
	}
	return result;
}

QPointF PolynomSystem::getNextValue(QPointF point, qreal eps, bool useRungeKutta) const {
	QVector2D diff(first(point), second(point));
	if (useRungeKutta) {
		qreal x = point.x(), y = point.y();
		qreal first25  = first (x + eps * first(point) / 2, y + eps * second(point) / 2);
		qreal second25 = second(x + eps * first(point) / 2, y + eps * second(point) / 2);
		qreal first50  = first (x + eps * first25 / 2, y + eps * second25 / 2);
		qreal second50 = second(x + eps * first25 / 2, y + eps * second25 / 2);
		qreal first75  = first (x + eps * first50, y + eps * second50);
		qreal second75 = second(x + eps * first50, y + eps * second50);
		diff.setX((first (point) + 2 * first25  + 2 * first50  + first75 ) / 6);
		diff.setY((second(point) + 2 * second25 + 2 * second50 + second75) / 6);
	}
	if (diff.length() > 1) {
		diff /= diff.length();
	}
	diff *= eps;
	return point + diff.toPointF();
}

qreal PolynomSystem::poincareFunction(qreal p, qreal eps) const {
	QPointF point(p, 0);
	quint32 step = 0;
	while (step < 20 || point.x() < 0 || qAbs(point.y()) > eps) {
		point = getNextValue(point, eps);
		if (step > 50. / eps) {
			return 0;
		}
		++step;
	}
	return point.x();
}

qreal PolynomSystem::findPoincareStaticPoint(qreal a, qreal b, qreal eps) const {
	qreal pdiffa = poincareFunction(a, eps) - a;
	qreal pdiffb = poincareFunction(b, eps) - b;
	if (pdiffa * pdiffb > 0) {
		return 0;
	}
	while (qAbs(b - a) > eps) {
		pdiffa = poincareFunction(a, eps) - a;
		pdiffb = poincareFunction(b, eps) - b;
		a = b - (b - a) * pdiffb / (pdiffb - pdiffa);
		pdiffa = poincareFunction(a, eps) - a;
		b = a - (a - b) * pdiffa / (pdiffa - pdiffb);
	}
	return b;
}

qreal PolynomSystem::getPeriod(qreal staticPoint, qreal eps) const {
	QPointF point(staticPoint, 0);
	quint32 step = 0;
	qreal diff = 0;
	while (step < 5 || qAbs(diff) > eps / 2) {
		point = getNextValue(point, eps, true);
		diff = qPow(point.x() - staticPoint, 2) + qPow(point.y(), 2);
		++step;
	}
	return step * eps;
}

void fillPolynom(QString line, Polynom &polynom, qreal param) {
	QStringList parts = line.split("  ");
	QStringList subParts;
	Monom m;
	polynom.clear();
	for (int i = 0; i < parts.size(); ++i) {
		subParts = parts.at(i).split(' ');
		m.xPow = subParts.at(1).toInt();
		m.yPow = subParts.at(2).toInt();
		m.c = subParts.at(0) == "c" ? param : subParts.at(0).toDouble();
		polynom.append(m);
	}
}

ComplexPair PolynomSystem::getEigenValues() const {
	ComplexPair result;
	qreal a = 0, b = 0, c = 0, d = 0;
	Polynom::ConstIterator i;
	for (i = first.begin(); i != first.end(); ++i) {
		if (i->xPow == 1 && i->yPow == 0) {
			a = i->c;
		} else if (i->xPow == 0 && i->yPow == 1) {
			b = i->c;
		}
	}
	for (i = second.begin(); i != second.end(); ++i) {
		if (i->xPow == 1 && i->yPow == 0) {
			c = i->c;
		} else if (i->xPow == 0 && i->yPow == 1) {
			d = i->c;
		}
	}
	qreal determinant = (a - d) * (a - d) + 4 * b * c;
	Complex detSqrt = std::sqrt(Complex(determinant, 0));
	result.first  = (Complex(a + d, 0) + detSqrt) * Complex(.5, 0);
	result.second = (Complex(a + d, 0) - detSqrt) * Complex(.5, 0);
	return result;
}

PointType PolynomSystem::getPointType() const {
	ComplexPair eigenValues = getEigenValues();
	Complex v1 = eigenValues.first, v2 = eigenValues.second;
	if (qFuzzyIsNull(v1.real()) && qFuzzyIsNull(v2.real())) {
		return PointCenter;
	} else if (qFuzzyIsNull(v1.imag()) && qFuzzyIsNull(v2.imag())) {
		if (qFuzzyCompare(v1.real(), v2.real())) {
			if (v1.real() > 0) {
				return PointKnotSingularUnstable;
			} else {
				return PointKnotSingularStable;
			}
		} else if (v1.real() > 0 && v2.real() > 0) {
			return PointKnotUnstable;
		} else if (v1.real() < 0 && v2.real() < 0) {
			return PointKnotStable;
		} else {
			return PointSaddle;
		}
	} else if (v1.real() > 0) {
		return PointFocusUnstable;
	} else {
		return PointFocusStable;
	}
}

void fillSystem(QString fileName, PolynomSystem &system, qreal param) {
	QFile inputFile(fileName);
	inputFile.open(QFile::ReadOnly);
	QTextStream input(&inputFile);
	fillPolynom(input.readLine(), system.first, param);
	fillPolynom(input.readLine(), system.second, param);
	inputFile.close();
}
