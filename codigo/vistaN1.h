#ifndef VISTAN1_H
#define VISTAN1_H

#include "vistaNivel.h"
#include "nivel1.h"
#include <QPixmap>
#include <QVector>
#include <QString>

class QGraphicsEllipseItem;
class QGraphicsPixmapItem;
class QGraphicsColorizeEffect;
class QGraphicsRectItem;

class vistaN1 : public vistaNivel
{
    Q_OBJECT
private:
    enum Estado { QUIETO, CAMINAR, CORRER, SALTO, AGACHAR, N_ESTADOS };
    nivel1 nivel;
    QGraphicsEllipseItem* dibujoPersonaje;
    QGraphicsPixmapItem* dibujoSprite;
    QGraphicsColorizeEffect* efectoDanio;
    bool teclaA;
    bool teclaD;
    QVector<QPixmap> animaciones[N_ESTADOS];
    QVector<QPixmap> animacionesIzq[N_ESTADOS];
    Estado estadoAnim;
    double tAnim;
    bool mirandoIzq;
    QVector<QGraphicsPixmapItem*> capasItem;
    QVector<double> capasFactor;

    // Proyectiles y gas
    QVector<QGraphicsPixmapItem*> dibujoBalas;
    QVector<QGraphicsPixmapItem*> dibujoLacrimogenas;
    QVector<QGraphicsPixmapItem*> dibujoNubes;
    QVector<QPixmap> framesGas;

    // Bufos
    QVector<QGraphicsPixmapItem*> dibujoBufos;

    // Muro
    QGraphicsRectItem* dibujoMuro;

    void dibujarFondo();
    void dibujarMapa();
    void dibujarPersonaje();
    void actualizarDireccion();
    void cargarSprites();
    void actualizarSprite();
    void agregarCapa(const QString& ruta, double factor, int z);
    void actualizarFondo(const QPointF& pos);
    void crearNubes();
    void actualizarNubes();
    void crearBufos();
    void actualizarBufos();

protected:
    void avanzarLogica(double dt) override;
    void dibujar(double alfa) override;
    void alPulsar(int tecla) override;
    void alSoltar(int tecla) override;
    void alPerderFoco() override;

public:
    explicit vistaN1(QWidget* parent = nullptr);
    const nivel1& getNivel() const;
    bool getTeclaA() const;
    bool getTeclaD() const;
};

#endif // VISTAN1_H
