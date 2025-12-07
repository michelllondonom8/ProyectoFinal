#include "portada.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Portada w;
    w.show();
    return a.exec();
}
