#include "mainwindow.h"
#include "vistaN1.h"
#include "datos.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), vista(nullptr)
{
    vista = new vistaN1(this);
    setCentralWidget(vista);
    setFixedSize(datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setWindowTitle("Nivel 1");
    vista->iniciar();
}