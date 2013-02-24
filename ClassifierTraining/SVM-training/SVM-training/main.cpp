#include "SVMTrainer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SVMTrainer w;
	w.show();
	return a.exec();
}
