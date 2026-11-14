#include "vistaN1.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>

vistaN1::vistaN1(QWidget* parent)
    : vistaNivel(parent),
    nivel(),dibujoPersonaje(nullptr),
    teclaA(false),teclaD(false)
{
    escena->setSceneRect(0.0, 0.0, nivel.getAnchoMapa(), datos::ALTO_ESCENA);
    dibujarMapa();
    dibujarPersonaje();
}

void vistaN1::dibujarMapa()
{
    const QPen borde(Qt::black);
    const QBrush solida(QColor(90, 90, 90));
    const QBrush atravesable(QColor(120, 170, 255));

    for (const obstaculo& o : nivel.getPlataformas())
        escena->addRect(o.getIzquierda(), o.getArriba(), o.getAncho(), o.getAlto(),
                        borde, o.getAtravesable() ? atravesable : solida);
}

void vistaN1::dibujarPersonaje()
{
    const double r = nivel.getPersonaje().getRadio();
    // El circulo se dibuja alrededor de (0,0) y se coloca con setPos en el centro del capitan.
    dibujoPersonaje = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                         QPen(Qt::black), QBrush(QColor(200, 30, 30)));
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    //ver como seria la hit box enrealidad
    auto* sprite = new QGraphicsRectItem(-r, r - 64.0, 2.0 * r, 64.0, dibujoPersonaje);
    sprite->setPen(QPen(Qt::blue));
}

void vistaN1::avanzarLogica(double dt)
{
    nivel.actualizar(dt);
}

void vistaN1::dibujar(double a)
{
    //el a es para evitar que la lgica y la pantalla tengan diferencia de frames
    const capitanN1& c = nivel.getPersonaje();
    const QPointF pos = c.getPosicionAnterior() + (c.getPosicion() - c.getPosicionAnterior()) * a;

    dibujoPersonaje->setPos(pos);
    centerOn(pos);
}

void vistaN1::actualizarDireccion()
{nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0));}

void vistaN1::alPulsar(int tecla)
{
    switch (tecla) {
    case Qt::Key_A:
        teclaA = true;
        actualizarDireccion();
        break;
    case Qt::Key_D:
        teclaD = true;
        actualizarDireccion();
        break;
    case Qt::Key_Space:
        nivel.saltar();
        break;
    case Qt::Key_Shift:
        nivel.bajarPlataforma();
        break;
    default: break;
    }
}
void vistaN1::alSoltar(int tecla) {
    switch (tecla) {
    case Qt::Key_A:
        teclaA = false;
        actualizarDireccion(); break;
    case Qt::Key_D:
        teclaD = false;
        actualizarDireccion(); break;
    default: break;
    }
}

void vistaN1::alPerderFoco() {
    teclaA = false;
    teclaD = false;
    actualizarDireccion();
}

const nivel1& vistaN1::getNivel() const
{ return nivel; }
bool vistaN1::getTeclaA() const
{ return teclaA; }
bool vistaN1::getTeclaD() const
{ return teclaD; }






















