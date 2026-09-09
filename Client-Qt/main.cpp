#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    a.setStyleSheet("QWidget { font-size: 14px; }");

    w.show();
    return a.exec();
}
