#include <QApplication>
#include "mainwindow.h"
#include <QMessageBox>
#include <stdexcept>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try {
        MainWindow w;
        w.show();
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", e.what());
        return 1;
    }
}