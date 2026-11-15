#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class vistaNivel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    vistaNivel* vista;
public:
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif // MAINWINDOW_H