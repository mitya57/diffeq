#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include "window.hpp"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	QTranslator translator;
	translator.load(QLocale::system().name(), "locale");
	app.installTranslator(&translator);
	PolynomSystem system;
	MyMainWindow window(&system);
	window.show();
	return app.exec();
}
