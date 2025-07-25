#include "cframe.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    cframe w;
    w.showMaximized();
    return a.exec();
}
