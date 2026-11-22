#include "mainwindow.h"
#include "vistaN1.h"
#include "vistaN2.h"
#include "datos.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), vista(nullptr), nivelActual(1)
{
    setFixedSize(datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    cargarNivel(1);
}
void MainWindow::cargarNivel(int numeroNivel)
{
    nivelActual = numeroNivel;
    if (nivelActual == 1) {
        setWindowTitle("Nivel 1");
        vista = new vistaN1(this);
    } else {
        setWindowTitle("Nivel 2");
        vista = new vistaN2(this);
    }
    connect(vista, &vistaNivel::solicitarReintento,      this, &MainWindow::reiniciarNivel);
    connect(vista, &vistaNivel::solicitarMenu,           this, &MainWindow::abrirMenu);
    connect(vista, &vistaNivel::solicitarSiguienteNivel, this, &MainWindow::siguienteNivel);
    setCentralWidget(vista);
    vista->iniciar();
}
void MainWindow::reiniciarNivel()
{
    cargarNivel(nivelActual);
}
void MainWindow::abrirMenu()
{
    vista->pausar();
}
void MainWindow::siguienteNivel()
{
    cargarNivel(nivelActual + 1);
}