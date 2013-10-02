#include <QtWidgets/QApplication>
#include "window.hpp"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	PolynomSystem system;
	MyMainWindow window(&system);
	window.show();
	return app.exec();
}
