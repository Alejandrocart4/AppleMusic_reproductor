#include "cframe.h"

#include <QApplication>
#include <QCoreApplication>
#include <QtGlobal>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);  // <— HiDPI pixmaps
    QApplication a(argc, argv);
    a.setStyle("fusion");
    cframe w;
    w.showMaximized();
    return a.exec();
}
