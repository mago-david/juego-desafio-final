#include <QApplication>
#include "mainwindow.h"
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Round);
    QApplication a(argc, argv);
    try {
        MainWindow w;
        w.showFullScreen();
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", e.what());
        return 1;
    }
}