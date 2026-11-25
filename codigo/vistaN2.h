#ifndef VISTAN2_H
#define VISTAN2_H

#include <QVector>
#include <QPixmap>
#include <QSize>
#include "vistaNivel.h"
#include "nivel2.h"

class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QGraphicsColorizeEffect;
class QMouseEvent;
class QMediaPlayer;
class QAudioOutput;

class vistaN2 : public vistaNivel
{
    Q_OBJECT
private:
    nivel2 nivel;
    QGraphicsPixmapItem* dibujoPersonaje;
    QVector<QGraphicsPixmapItem*> dibujoEscudos;
    QGraphicsPixmapItem* dibujoJefe;
    QVector<QGraphicsPixmapItem*> dibujoBalas;
    QVector<QGraphicsRectItem*> dibujoEfectos;
    QVector<QGraphicsPixmapItem*> dibujoBufos;
    QVector<QGraphicsPixmapItem*> dibujoHumos;
    QVector<QGraphicsPixmapItem*> dibujoAguas;
    QVector<QGraphicsPixmapItem*> dibujoEsbirros;
    QGraphicsRectItem* barraJefeFondo;
    QGraphicsRectItem* barraJefeRelleno;
    QGraphicsRectItem* barraJefeBorde;
    QGraphicsColorizeEffect* efectoCuracionJefe;
    QMediaPlayer* reproductorJefe;
    QAudioOutput* salidaAudioJefe;
    // Sprites Capitán N2
    QPixmap spriteAbajoQuieto;
    QVector<QPixmap> framesAbajoCaminar;
    QVector<QPixmap> framesAbajoSinEscudo;
    QVector<QPixmap> framesAbajoLanzar;

    QPixmap spriteArribaQuieto;
    QVector<QPixmap> framesArribaCaminar;

    QPixmap spriteDerechaQuieto;
    QVector<QPixmap> framesDerechaCaminar;
    QVector<QPixmap> framesDerechaLanzar;

    QPixmap spriteIzquierdaQuieto;
    QVector<QPixmap> framesIzquierdaCaminar;
    QVector<QPixmap> framesIzquierdaLanzar;

    QPixmap spriteEscudo;
    QPixmap spriteBala;
    QPixmap spriteBufo;
    QVector<QPixmap> framesHumo;
    QVector<QPixmap> framesAgua;

    // Caché para evitar escalar/tilear en cada frame
    QVector<QPixmap> cacheHumos;
    QSize tamCacheHumo;
    QVector<QPixmap> cacheAguas;
    QSize tamCacheAgua;

    // Sprites Arena / Jefe / Esbirros (capitan2/jefe/1..5.png)
    QPixmap spritePiso;
    QPixmap spriteMuroHorizontal;
    QPixmap spriteMuroVertical;
    QPixmap spriteEsbirro;
    QPixmap spriteJefe;

    // Estado visual / animación
    int ultimaDirX;
    int ultimaDirY;
    double tiempoAnimacion;
    double tiempoLanzando;
    int vidasPrevias;
    bool jefeEstabaVivo;
    int vidaJefePrevia;
    bool fortachonPrevio;

    bool teclaW;
    bool teclaA;
    bool teclaS;
    bool teclaD;

    QVector<QPixmap> cargarListaFrames(const QString& carpeta, const QVector<int>& indices, double escala);
    QVector<QPixmap> voltearListaHorizontal(const QVector<QPixmap>& orig);
    void cargarSprites();
    void dibujarArena();
    void dibujarPersonaje();
    void dibujarJefe();
    void crearEfectos();
    void dibujarEfectos();
    void crearBufos();
    void dibujarBufos();
    void crearHumos();
    void dibujarHumos();
    void crearAguas();
    void dibujarAguas();
    void crearEsbirros();
    void dibujarEsbirros();
    void actualizarDireccion();
    void crearBarraJefe();
    void actualizarBarraJefe();
    const QPixmap& elegirFrameCapitan() const;

protected:
    void avanzarLogica(double dt) override;
    void dibujar(double alfa) override;
    void alPulsar(int tecla) override;
    void alSoltar(int tecla) override;
    void alPerderFoco() override;
    void mousePressEvent(QMouseEvent* e) override;

public:
    explicit vistaN2(bool dificultad, QWidget* parent = nullptr);
    const nivel2& getNivel() const;
    bool getTeclaW() const;
    bool getTeclaA() const;
    bool getTeclaS() const;
    bool getTeclaD() const;
};

#endif // VISTAN2_H
