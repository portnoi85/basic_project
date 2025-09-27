#include "crossword.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Crossword w;
	w.show();
	return a.exec();
}
