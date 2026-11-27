#include "mainwindow.h"
#include "vistaN1.h"
#include "vistaN2.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    pilaVistas(new QStackedWidget(this)),
    paginaMenu(nullptr),
    paginaTutorialN1(nullptr),
    paginaTutorialN2(nullptr),
    paginaSelectorNiveles(nullptr),
    vista(nullptr),
    nivelActual(1),
    dificultadActual(Normal),
    musica(new QMediaPlayer(this)),
    salidaMusica(new QAudioOutput(this))
{
    musica->setAudioOutput(salidaMusica);   // aqui se paga la inicializacion, con el menu en pantalla
    salidaMusica->setVolume(0.45f);
    musica->setLoops(QMediaPlayer::Infinite);

    setCentralWidget(pilaVistas);
    pilaVistas->setObjectName("pilaMenuGlobal");
    pilaVistas->setStyleSheet(
        "#pilaMenuGlobal {"
        " border-image: url(:/capitan/fondo/0.png) 0 0 0 0 stretch stretch;"
        "}"
        );
    paginaMenu = crearMenuPrincipal();
    paginaTutorialN1 = crearPantallaTutorial(
        "Tutorial - Nivel 1",
        "• Muévete y salta por las plataformas evitando caer o ser alcanzado por el muro.\n"
        "• Esquiva disparos, granadas lacrimógenas y nubes de gas.\n"
        "• Recoge los bufos en las plataformas para obtener ventaja."
        );
    paginaTutorialN2 = crearPantallaTutorial(
        "Tutorial - Nivel 2",
        "• Muévete con W, A, S, D y apunta/lanza tu escudo con clic izquierdo.\n"
        "• Golpea al jefe, esquiva sus proyectiles y elimina los esbirros.\n"
        "• Recoge el bufo 'Fortachón' para aumentar el daño de tu escudo y recuperar vida."
        );
    paginaSelectorNiveles = crearPantallaSelectorNiveles();

    pilaVistas->addWidget(paginaMenu);
    pilaVistas->addWidget(paginaTutorialN1);
    pilaVistas->addWidget(paginaTutorialN2);
    pilaVistas->addWidget(paginaSelectorNiveles);
    pilaVistas->setCurrentWidget(paginaMenu);
    setStyleSheet("QMainWindow { background-color: black; }");
}

QWidget* MainWindow::crearMenuPrincipal()
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(14);

    QLabel* titulo = new QLabel("DESAFÍO FINAL", panel);
    titulo->setStyleSheet("font-size: 34px; font-weight: bold; color: white;");
    titulo->setAlignment(Qt::AlignCenter);

    QHBoxLayout* filaDif = new QHBoxLayout();
    filaDif->setAlignment(Qt::AlignCenter);
    QLabel* lblDif = new QLabel("Dificultad:", panel);
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
    filaDif->addWidget(comboDif);

    QPushButton* btnIniciar     = new QPushButton("Iniciar", panel);
    QPushButton* btnTutorialN1  = new QPushButton("Tutorial Nivel 1", panel);
    QPushButton* btnTutorialN2  = new QPushButton("Tutorial Nivel 2", panel);
    QPushButton* btnSelectorNiv = new QPushButton("Selector de niveles", panel);

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

    connect(btnIniciar,     &QPushButton::clicked, this, &MainWindow::iniciarJuego);
    connect(btnTutorialN1,  &QPushButton::clicked, this, &MainWindow::abrirTutorialN1);
    connect(btnTutorialN2,  &QPushButton::clicked, this, &MainWindow::abrirTutorialN2);
    connect(btnSelectorNiv, &QPushButton::clicked, this, &MainWindow::abrirSelectorNiveles);

    layout->addWidget(titulo);
    layout->addLayout(filaDif);
    layout->addWidget(btnIniciar);
    layout->addWidget(btnTutorialN1);
    layout->addWidget(btnTutorialN2);
    layout->addWidget(btnSelectorNiv);

    panel->setStyleSheet("background: transparent;");
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

    panel->setStyleSheet("background: transparent;");
    return panel;
}

QWidget* MainWindow::crearPantallaSelectorNiveles()
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(14);

    QLabel* lblTitulo = new QLabel("SELECCIONAR NIVEL", panel);
    lblTitulo->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    QPushButton* btnNivel1 = new QPushButton("Nivel 1", panel);
    QPushButton* btnNivel2 = new QPushButton("Nivel 2", panel);
    QPushButton* btnVolver = new QPushButton("Volver al menú", panel);

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

    btnNivel1->setStyleSheet(estiloBtn);
    btnNivel2->setStyleSheet(estiloBtn);
    btnVolver->setStyleSheet(estiloBtn);

    connect(btnNivel1, &QPushButton::clicked, this, [this]() { cargarNivel(1); });
    connect(btnNivel2, &QPushButton::clicked, this, [this]() { cargarNivel(2); });
    connect(btnVolver, &QPushButton::clicked, this, &MainWindow::abrirMenu);

    layout->addWidget(lblTitulo);
    layout->addWidget(btnNivel1);
    layout->addWidget(btnNivel2);
    layout->addWidget(btnVolver);

    panel->setStyleSheet("background: transparent;");
    return panel;
}

void MainWindow::limpiarVistaActual()
{
    if (!vista)
        return;
    vista->pausar();
    vista->disconnect(this);
    pilaVistas->removeWidget(vista);
    vista->deleteLater();
    vista = nullptr;
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
    setWindowTitle("Seleccionar nivel");
    pilaVistas->setCurrentWidget(paginaSelectorNiveles);
}

void MainWindow::cargarNivel(int numeroNivel)
{
    limpiarVistaActual();
    nivelActual = numeroNivel;
    if (nivelActual == 1) {
        setWindowTitle("Nivel 1");
        vista = new vistaN1(this);
    } else {
        setWindowTitle("Nivel 2");
        vista = new vistaN2(dificultadActual == Dificil, this);
    }
    connect(vista, &vistaNivel::solicitarReintento,      this, &MainWindow::reiniciarNivel);
    connect(vista, &vistaNivel::solicitarMenu,           this, &MainWindow::abrirMenu);
    connect(vista, &vistaNivel::solicitarSiguienteNivel, this, &MainWindow::siguienteNivel);
    pilaVistas->addWidget(vista);
    pilaVistas->setCurrentWidget(vista);
    vista->iniciar();
    //rutas de musica
    const QUrl cancion(nivelActual == 1 ? "qrc:/capitan/sonidos_fondo/1.mp3"
                                        : "qrc:/capitan/sonidos_fondo/2.mp3");
    if (musica->source() != cancion)
        musica->setSource(cancion);
    musica->setPosition(0);
    musica->play();
    connect(vista, &vistaNivel::terminado, this, [this](bool) { musica->stop(); });
}

void MainWindow::reiniciarNivel()
{
    cargarNivel(nivelActual);
}

void MainWindow::abrirMenu()
{
    musica->stop();
    limpiarVistaActual();
    setWindowTitle("Menú Principal");
    pilaVistas->setCurrentWidget(paginaMenu);
}

void MainWindow::siguienteNivel()
{
    cargarNivel(nivelActual + 1);
}
