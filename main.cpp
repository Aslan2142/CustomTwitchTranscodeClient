#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QString version = "0.9.1";

    QApplication a(argc, argv);
    MainWindow w(version);
    w.show();

    return a.exec();
}
