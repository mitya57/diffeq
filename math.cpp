#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtGui/QVector2D>
#include "math.hpp"

qreal Polynom::operator()(qreal x, qreal y) {
	qreal result = 0;
	for (ConstIterator i = begin(); i != end(); ++i) {
		result += qPow(x, i->xPow) * qPow(y, i->yPow) * i->c;
	}
	return result;
}

qreal Polynom::operator()(QPointF point) {
	return operator()(point.x(), point.y());
}

QPointF PolynomSystem::getNextValue(QPointF point, qreal eps, bool useRungeKutta) {
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

void fillPolynom(QString line, Polynom &polynom) {
	QStringList parts = line.split("  ");
	QStringList subParts;
	Monom m;
	polynom.clear();
	for (int i = 0; i < parts.size(); ++i) {
		subParts = parts.at(i).split(' ');
		m.xPow = subParts.at(1).toDouble();
		m.yPow = subParts.at(2).toDouble();
		m.c = subParts.at(0).toInt();
		polynom.append(m);
	}
}

ComplexPair PolynomSystem::getEigenValues() {
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

PointType PolynomSystem::getPointType() {
	ComplexPair eigenValues = getEigenValues();
	Complex v1 = eigenValues.first, v2 = eigenValues.second;
	if (qFuzzyIsNull(v1.real()) && qFuzzyIsNull(v2.real())) {
		return PointCenter;
	} else if (qFuzzyIsNull(v1.imag()) && qFuzzyIsNull(v2.imag())) {
		if (v1.real() > 0 && v2.real() > 0) {
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

void fillSystem(QString fileName, PolynomSystem &system) {
	QFile inputFile(fileName);
	inputFile.open(QFile::ReadOnly);
	QTextStream input(&inputFile);
	fillPolynom(input.readLine(), system.first);
	fillPolynom(input.readLine(), system.second);
	inputFile.close();
}
