#ifndef VISTANIVEL_H
#define VISTANIVEL_H

#include <QGraphicsView>
#include <QElapsedTimer>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QPixmap>
#include <QSoundEffect>
#include <QUrl>
#include <QThread>
#include "proyectil.h"

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QPushButton;
class QTimer;
class QKeyEvent;
class QResizeEvent;

class vistaNivel : public QGraphicsView
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
    bool esUltimoNivel;
    QGraphicsPixmapItem* spriteVictoria;
    QPushButton* btnSiguienteNivel;

    void crearUI_Final();
    void mostrarPantallaMuerte();
    void posicionarBotonesFinales();
    QPixmap recortarSprite(const QString& ruta, double altoDeseado);

    // Sonidos
    QSoundEffect* sonidoGolpe1;
    QSoundEffect* sonidoGolpe2;
    QSoundEffect* sonidoMuerte;
    QSoundEffect* sonidoBufo;
    QSoundEffect* sonidoGolpeJefe;
    int turnoGolpe;
    QThread* hiloAudio;
    void tocar(QSoundEffect* s);
private slots:
    void avanzar();
    void actualizarFundidoMuerte();

protected:
    QGraphicsScene* escena;

    explicit vistaNivel(QWidget* parent = nullptr);
    virtual void avanzarLogica(double dt) = 0;
    virtual void dibujar(double alfa) = 0;
    virtual void alPulsar(int tecla) = 0;
    virtual void alSoltar(int tecla) = 0;
    virtual void alPerderFoco();

    void setUltimoNivel(bool ultimo);
    void terminar(bool gano);

    // Controles
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;

    // Redimensionado
    void resizeEvent(QResizeEvent* e) override;

    // Proyectiles
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsEllipseItem*>& items, const QColor& color);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsEllipseItem*>& items);
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsPixmapItem*>& items, const QPixmap& sprite);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsPixmapItem*>& items, bool rotar = false);

    // HUD y frames compartidos
    QPixmap corazonLleno;
    QPixmap corazonVacio;
    QPixmap corazonExtra;
    QVector<QGraphicsPixmapItem*> corazones;
    QPixmap prepararFrame(const QString& ruta, double escala);
    QVector<QPixmap> cargarFrames(const QString& carpeta, int desde, int hasta, double escala);
    void dibujarHUD(int vidasMax);
    void actualizarHUD(int vidas);

    // Sonidos
    void reproducirDanio();
    void reproducirMuerte();
    void reproducirBufo();
    void reproducirGolpeJefe();

public:
    ~vistaNivel();
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