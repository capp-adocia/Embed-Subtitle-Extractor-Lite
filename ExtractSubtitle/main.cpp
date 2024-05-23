#include "subtitle.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Subtitle w;
    w.show();
    return a.exec();
}
