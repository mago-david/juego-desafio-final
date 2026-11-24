#include "mainwindow.h"
#include "vistaN1.h"
#include "vistaN2.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    pilaVistas(new QStackedWidget(this)),
    paginaMenu(nullptr),
    paginaTutorialN1(nullptr),
    paginaTutorialN2(nullptr),
    vista(nullptr),
    nivelActual(1),
    dificultadActual(Normal)
{//texto tutoriales
    setCentralWidget(pilaVistas);

    paginaMenu=crearMenuPrincipal();
    paginaTutorialN1=crearPantallaTutorial(
        "Tutorial - Nivel 1",
        "* Muévete y salta por las plataformas evitando caer o ser alcanzado por el muro.\n"
        "* Esquiva disparos, granadas lacrimógenas y nubes de gas.\n"
        "* Recoge los bufos en las plataformas para obtener ventaja.");
    paginaTutorialN2=crearPantallaTutorial(
        "Tutorial - Nivel 2",
        "* Muévete con W, A, S, D y apunta/lanza tu escudo con clic izquierdo.\n"
        "* Golpea al jefe, esquiva sus proyectiles y elimina los esbirros.\n"
        "* Recoge el bufo 'Fortachón' para aumentar el daño de tu escudo y recuperar vida."
        );
    pilaVistas->addWidget(paginaMenu);
    pilaVistas->addWidget(paginaTutorialN1);
    pilaVistas->addWidget(paginaTutorialN2);
    pilaVistas->setCurrentWidget(paginaMenu);
    setWindowTitle("Menú Principal");
}
QWidget* MainWindow::crearMenuPrincipal()
{//menu principal creacion completa
    QWidget* panel=new QWidget(this);
    QVBoxLayout* layout=new QVBoxLayout(panel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(14);

    QLabel* titulo=new QLabel("DESAFÍO FINAL", panel);
    titulo->setStyleSheet("font-size: 34px; font-weight: bold; color: white;");//tipo letra y color
    titulo->setAlignment(Qt::AlignCenter);
    QHBoxLayout* filaDif = new QHBoxLayout();
    filaDif->setAlignment(Qt::AlignCenter);
    QLabel* lblDif = new QLabel("Dificultad:", panel);//panel de dificultad <<falta hacer que tificultad cambie algo>>
    lblDif->setStyleSheet("color: white; font-size: 16px;");
    QComboBox* comboDif = new QComboBox(panel);
    comboDif->addItem("Normal", Normal);
    comboDif->addItem("Difícil", Dificil);
    comboDif->setStyleSheet("font-size: 15px; padding: 4px 10px;");
    connect(comboDif, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this, comboDif](int idx) {
                dificultadActual = static_cast<Dificultad>(comboDif->itemData(idx).toInt());
            });
    filaDif->addWidget(lblDif);
    filaDif->addWidget(comboDif);//definicion de botones y funcion
    QPushButton* btnIniciar      = new QPushButton("Iniciar", panel);
    QPushButton* btnTutorialN1   = new QPushButton("Tutorial Nivel 1", panel);
    QPushButton* btnTutorialN2   = new QPushButton("Tutorial Nivel 2", panel);
    QPushButton* btnSelectorNiv  = new QPushButton("Selector de niveles", panel);
    const QString estiloBtn =
        "QPushButton {"
        "  min-width: 250px;"
        "  min-height: 44px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  background-color: #242630;"
        "  color: white;"
        "  border: 2px solid #7c859e;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #363a4a; border-color: white; }";
    btnIniciar->setStyleSheet(estiloBtn);
    btnTutorialN1->setStyleSheet(estiloBtn);
    btnTutorialN2->setStyleSheet(estiloBtn);
    btnSelectorNiv->setStyleSheet(estiloBtn);
    connect(btnIniciar,&QPushButton::clicked, this, &MainWindow::iniciarJuego);
    connect(btnTutorialN1,&QPushButton::clicked, this, &MainWindow::abrirTutorialN1);
    connect(btnTutorialN2,&QPushButton::clicked, this, &MainWindow::abrirTutorialN2);
    connect(btnSelectorNiv,&QPushButton::clicked, this, &MainWindow::abrirSelectorNiveles);
    layout->addWidget(titulo);
    layout->addLayout(filaDif);
    layout->addWidget(btnIniciar);
    layout->addWidget(btnTutorialN1);
    layout->addWidget(btnTutorialN2);
    layout->addWidget(btnSelectorNiv);
    panel->setStyleSheet("background-color: #121318;");
    return panel;
}
QWidget* MainWindow::crearPantallaTutorial(const QString& titulo, const QString& contenido)
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(18);
    QLabel* lblTitulo = new QLabel(titulo, panel);
    lblTitulo->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    lblTitulo->setAlignment(Qt::AlignCenter);
    QLabel* lblTexto = new QLabel(contenido, panel);
    lblTexto->setStyleSheet("font-size: 17px; color: #e2e6f0; line-height: 1.4;");
    lblTexto->setAlignment(Qt::AlignLeft);
    QPushButton* btnVolver = new QPushButton("Volver al menú", panel);
    btnVolver->setStyleSheet(
        "QPushButton {"
        "  min-width: 220px;"
        "  min-height: 42px;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "  background-color: #242630;"
        "  color: white;"
        "  border: 2px solid #7c859e;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #363a4a; border-color: white; }"
        );
    connect(btnVolver, &QPushButton::clicked, this, &MainWindow::abrirMenu);
    layout->addWidget(lblTitulo);
    layout->addWidget(lblTexto);
    layout->addWidget(btnVolver, 0, Qt::AlignCenter);
    panel->setStyleSheet("background-color: #121318;");
    return panel;
}
void MainWindow::iniciarJuego()
{
    cargarNivel(1);
}
void MainWindow::abrirTutorialN1()
{
    setWindowTitle("Tutorial - Nivel 1");
    pilaVistas->setCurrentWidget(paginaTutorialN1);
}
void MainWindow::abrirTutorialN2()
{
    setWindowTitle("Tutorial - Nivel 2");
    pilaVistas->setCurrentWidget(paginaTutorialN2);
}
void MainWindow::abrirSelectorNiveles()
{
    QMessageBox selector(this);
    selector.setWindowTitle("Selector de niveles");
    selector.setText("Selecciona un nivel:");
    QPushButton* btnN1 = selector.addButton("Nivel 1", QMessageBox::AcceptRole);
    QPushButton* btnN2 = selector.addButton("Nivel 2", QMessageBox::AcceptRole);
    selector.addButton("Cancelar", QMessageBox::RejectRole);
    selector.exec();
    if (selector.clickedButton() == btnN1) {
        cargarNivel(1);
    } else if (selector.clickedButton() == btnN2) {
        cargarNivel(2);
    }
}
void MainWindow::cargarNivel(int numeroNivel)
{
    if (vista) {
        vista->pausar();
        pilaVistas->removeWidget(vista);
        vista->deleteLater();
        vista = nullptr;
    }
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
    pilaVistas->addWidget(vista);
    pilaVistas->setCurrentWidget(vista);
    vista->iniciar();
}
void MainWindow::reiniciarNivel()
{
    cargarNivel(nivelActual);
}
void MainWindow::abrirMenu()
{
    if (vista) {
        vista->pausar();
        pilaVistas->removeWidget(vista);
        vista->deleteLater();
        vista = nullptr;
    }
    setWindowTitle("Menú Principal");
    pilaVistas->setCurrentWidget(paginaMenu);
}
void MainWindow::siguienteNivel()
{
    cargarNivel(nivelActual + 1);
}