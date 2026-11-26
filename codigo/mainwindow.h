#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
class QStackedWidget;
class QWidget;
class vistaNivel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Dificultad { Normal, Dificil };

private:
    QStackedWidget* pilaVistas;
    QWidget* paginaMenu;
    QWidget* paginaTutorialN1;
    QWidget* paginaTutorialN2;
    QWidget* paginaSelectorNiveles;
    vistaNivel* vista;
    int nivelActual;
    Dificultad dificultadActual;
    QMediaPlayer* musica;
    QAudioOutput* salidaMusica;

    QWidget* crearMenuPrincipal();
    QWidget* crearPantallaTutorial(const QString& titulo, const QString& contenido);
    QWidget* crearPantallaSelectorNiveles();
    void limpiarVistaActual();
    void cargarNivel(int numeroNivel);

private slots:
    void iniciarJuego();
    void abrirTutorialN1();
    void abrirTutorialN2();
    void abrirSelectorNiveles();
    void reiniciarNivel();
    void abrirMenu();
    void siguienteNivel();

public:
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif // MAINWINDOW_H