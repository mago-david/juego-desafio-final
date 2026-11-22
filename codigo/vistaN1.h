#ifndef VISTAN1_H
#define VISTAN1_H
#include "vistaNivel.h"
#include "nivel1.h"
#include <QPixmap>
#include <QVector>
#include <QString>
class QGraphicsEllipseItem; //hit box capitan
class QGraphicsPixmapItem;// sprites personajes fondos objetos y tales
class QGraphicsColorizeEffect;// efectos de color
class vistaN1:public vistaNivel
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
    QVector<QPixmap> animaciones[N_ESTADOS];//estado del personaje para animar
    Estado estadoAnim;
    double tAnim;
    bool mirandoIzq;
    QVector<QGraphicsPixmapItem*> capasItem;   // imagen de cada capa del fondo
    QVector<double> capasFactor;               // velocidad de cada capa (0.2, 0.5, 0.8)
    void dibujarFondo();
    void dibujarMapa();
    void dibujarPersonaje();
    void actualizarDireccion();
    void cargarSprites();
    void actualizarSprite();
    void agregarCapa(const QString& ruta, double factor, int z);
    void actualizarFondo(const QPointF& pos);
    //proyectiles
    QVector<QGraphicsPixmapItem*> dibujoBalas;
    QVector<QGraphicsPixmapItem*> dibujoLacrimogenas;
    QVector<QGraphicsPixmapItem*> dibujoNubes;
    QVector<QPixmap> framesGas;
    void crearNubes();
    void actualizarNubes();
    //bufo
    QVector<QGraphicsPixmapItem*> dibujoBufos;
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
