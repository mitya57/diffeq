#include <QtCore/QDebug>
#include <QtGui/QVector2D>
#include "math.hpp"

qreal getDelta(const PolynomSystem &system, qreal tau) {
	qreal a = system.findPoincareStaticPoint(.5, 5, tau), b;
	if (a) {
		b = system.poincareFunction(a, tau);
	} else {
		a = system.poincareFunction(1, tau);
		b = system.poincareFunction(a, tau);
	}
	return qAbs(a - b);
}

qreal getEpsilon(const PolynomSystem &system, QPointF start, qreal tau) {
	QPointF a = start, b = start;
	qreal max = 0;
	for (quint32 i = 0; i < 1000; ++i) {
		a = system.getNextValue(a, tau, true);
		b = system.getNextValue(b, tau / 2, true);
		b = system.getNextValue(b, tau / 2, true);
		max = qMax(max, (qreal)QVector2D(a - b).length());
	}
	return max;
}

void testTau(PolynomSystem &system, qreal tau) {
	qDebug() << tau << getEpsilon(system, QPointF(1, 0), tau) << getDelta(system, tau);
}

void testParam(PolynomSystem &system, qreal param) {
	qDebug() << "----- testing parameter" << param << "-----";
	fillSystem("system.txt", system, param);
	testTau(system, 0.0001);
	testTau(system, 0.0002);
	testTau(system, 0.0005);
	testTau(system, 0.001);
	testTau(system, 0.002);
	testTau(system, 0.005);
	testTau(system, 0.01);
	testTau(system, 0.02);
	testTau(system, 0.05);
	testTau(system, 0.1);
}

int main() {
	PolynomSystem system;
	testParam(system, 1);
	testParam(system, -1);
	testParam(system, 3);
	return 0;
}
