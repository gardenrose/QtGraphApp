#include "mainwindow.h"
#include "conversion.h"
#include <QApplication>
#include <QtDataVisualization>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
