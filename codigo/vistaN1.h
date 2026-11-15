#ifndef VISTAN1_H
#define VISTAN1_H
#include "vistaNivel.h"
#include "nivel1.h"
#include <QPixmap>
#include <QVector>
#include <QString>
class QGraphicsEllipseItem;
class QGraphicsPixmapItem;

class vistaN1:public vistaNivel
{
    Q_OBJECT
private:
    enum Estado { QUIETO, CAMINAR, CORRER, SALTO, AGACHAR, N_ESTADOS };
    nivel1 nivel;
    QGraphicsEllipseItem* dibujoPersonaje;
    QGraphicsPixmapItem* dibujoSprite;
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
    QPixmap prepararFrame(const QString& ruta, double escala);
    QVector<QPixmap> cargarFrames(const QString& carpeta, int desde, int hasta, double escala);
    void actualizarSprite();
    void agregarCapa(const QString& ruta, double factor, int z);
    void actualizarFondo(const QPointF& pos);
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
