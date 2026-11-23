#ifndef VISTANIVEL_H
#define VISTANIVEL_H
#include <QGraphicsView>
#include <QElapsedTimer>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QPixmap>
#include<QSoundEffect>
#include <QUrl>
#include "proyectil.h"

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QPushButton;
class QTimer;
class QKeyEvent;
class QSoundEffect;
class QResizeEvent;
class vistaNivel:public QGraphicsView

{
    Q_OBJECT
private:
    QTimer* reloj;
    QElapsedTimer cronometro;
    double acumulado;
    bool pausado;
    QTimer* relojMuerte;
    double opacidadMuerte;
    QGraphicsRectItem* telonNegro;
    QGraphicsPixmapItem* spriteMoriste;
    QPushButton* btnReintentar;
    QPushButton* btnMenu;
    bool enPantallaFinal;
    bool ganoNivel;
    QGraphicsPixmapItem* spriteVictoria;
    QPushButton* btnSiguienteNivel;
    void crearUI_Victoria();
    void mostrarPantallaVictoria();
    void posicionarBotonesFinales();
    void crearUI_Muerte();
    void mostrarPantallaMuerte();
    QPixmap recortarSprite(const QString& ruta, double altoDeseado);
    //sonidos
    QSoundEffect* sonidoGolpe1;
    QSoundEffect* sonidoGolpe2;
    QSoundEffect* sonidoMuerte;
    QSoundEffect* sonidoBufo;
    QSoundEffect* sonidoGameOver;
    QSoundEffect* sonidoVictoria;
    int turnoGolpe;

private slots:
    void avanzar();
    void actualizarFundidoMuerte();
protected:
    //la escena la hacen las hijas
    QGraphicsScene* escena;

    explicit vistaNivel(QWidget* parent = nullptr);

    //ni idea como describir
    virtual void avanzarLogica(double dt) = 0;
    virtual void dibujar(double alfa) = 0;
    virtual void alPulsar(int tecla) = 0;
    virtual void alSoltar(int tecla) = 0;
    virtual void alPerderFoco();

    void terminar(bool gano);
    //los controles
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    //pantalla grande
    void resizeEvent(QResizeEvent* e) override;
    //cositas proyectiles
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsEllipseItem*>& items, const QColor& color);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsEllipseItem*>& items);
    //las de abajo son los proyectiles definitivos para n1 y seguramente para n2 de momento dejar las de arriba para evitar errores
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsPixmapItem*>& items, const QPixmap& sprite);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsPixmapItem*>& items, bool rotar = false);
    //cositas que van en ambos niveles
    QPixmap corazonLleno;
    QPixmap corazonVacio;
    QPixmap corazonExtra;
    QVector<QGraphicsPixmapItem*> corazones;
    QPixmap prepararFrame(const QString& ruta, double escala);
    QVector<QPixmap> cargarFrames(const QString& carpeta, int desde, int hasta, double escala);
    void dibujarHUD(int vidasMax);
    void actualizarHUD(int vidas);
    //sonidos
    void reproducirDanio();
    void reproducirMuerte();
    void reproducirBufo();
    void reproducirGameOver();
    void reproducirVictoria();
public:
    //"momentos"
    void iniciar();
    void pausar();
    double getAcumulado() const;
    bool getPausado() const;
signals:
    void terminado(bool gano);
    void solicitarReintento();
    void solicitarMenu();
    void solicitarSiguienteNivel();
};

#endif // VISTANIVEL_H
