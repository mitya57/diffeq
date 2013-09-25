#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "window.hpp"

void fillPolynom(QString line, Polynom &polynom) {
	QStringList parts = line.split("  ");
	QStringList subParts;
	Monom m;
	for (int i = 0; i < parts.size(); ++i) {
		subParts = parts.at(i).split(' ');
		m.xPow = subParts.at(1).toDouble();
		m.yPow = subParts.at(2).toDouble();
		m.c = subParts.at(0).toInt();
		polynom += m;
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

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	PolynomSystem system;
	fillSystem("system.txt", system);
	MyMainWindow window(&system);
	window.show();
	return app.exec();
}
