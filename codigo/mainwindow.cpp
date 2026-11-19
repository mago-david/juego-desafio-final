#include "mainwindow.h"
#include "vistaN1.h"
#include "datos.h"
#include "vistaN2.h"
#include "datos.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), vista(nullptr)
{
    vista = new vistaN1(this);                 // para volver al Nivel 1: new vistaN1(this) y #include "vistaN1.h"
    setCentralWidget(vista);
    setFixedSize(datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setWindowTitle("Nivel 2");
    vista->iniciar();
}