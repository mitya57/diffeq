#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtGui/QVector2D>
#include "math.hpp"

qreal Polynom::operator()(QPointF point) {
	qreal result = 0;
	for (ConstIterator i = begin(); i != end(); ++i) {
		result += qPow(point.x(), i->xPow) * qPow(point.y(), i->yPow) * i->c;
	}
	return result;
}

QPointF PolynomSystem::getNextValue(QPointF point, qreal eps) {
	QVector2D diff(first(point), second(point));
	if (diff.length() > 1) {
		diff /= diff.length();
	}
	diff *= eps;
	return point += diff.toPointF();
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

void fillSystem(QString fileName, PolynomSystem &system) {
	QFile inputFile(fileName);
	inputFile.open(QFile::ReadOnly);
	QTextStream input(&inputFile);
	fillPolynom(input.readLine(), system.first);
	fillPolynom(input.readLine(), system.second);
	inputFile.close();
}
