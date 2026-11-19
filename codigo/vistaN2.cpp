#include "vistaN2.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QBrush>
#include <QPen>

vistaN2::vistaN2(QWidget* parent)
    : vistaNivel(parent),
    nivel(), dibujoPersonaje(nullptr),
    teclaW(false), teclaA(false), teclaS(false), teclaD(false)
{
    escena->setSceneRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setBackgroundBrush(Qt::black);                 // lo que queda fuera de la arena
    dibujarArena();
    dibujarPersonaje();
    crearDibujos(nivel.getPersonaje().getEscudos(), dibujoEscudos, QColor(200, 200, 220));
}

void vistaN2::dibujarArena()
{
    const QPen borde(Qt::black);
    const QBrush solida(QColor(90, 90, 90));

    QRectF area;                                   // rectangulo que abarcan las 4 paredes
    for (const obstaculo& o : nivel.getParedes()) {
        const QRectF r(o.getIzquierda(), o.getArriba(), o.getAncho(), o.getAlto());
        escena->addRect(r, borde, solida);
        area = area.united(r);
    }
    // el piso va debajo de las paredes
    QGraphicsRectItem* piso = escena->addRect(area, QPen(Qt::NoPen), QBrush(QColor(55, 60, 75)));
    piso->setZValue(-10);
}

void vistaN2::dibujarPersonaje()
{
    const double r = nivel.getPersonaje().getRadio();
    // circulo de prueba: se cambia por el sprite cenital cuando exista
    dibujoPersonaje = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                         QPen(Qt::black), QBrush(QColor(40, 90, 200)));
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujoPersonaje->setZValue(5);
}

void vistaN2::avanzarLogica(double dt)
{
    nivel.actualizar(dt);
}

void vistaN2::dibujar(double)
{
    // sin interpolar por ahora: capitanN2 no guarda la posicion anterior
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujarPool(nivel.getPersonaje().getEscudos(), dibujoEscudos);
}

void vistaN2::actualizarDireccion()
{
    // en Qt y crece hacia abajo: S = +1, W = -1
    nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0),
                                (teclaS ? 1 : 0) - (teclaW ? 1 : 0));
}

void vistaN2::alPulsar(int tecla)
{
    switch (tecla) {
    case Qt::Key_W: teclaW = true; actualizarDireccion(); break;
    case Qt::Key_A: teclaA = true; actualizarDireccion(); break;
    case Qt::Key_S: teclaS = true; actualizarDireccion(); break;
    case Qt::Key_D: teclaD = true; actualizarDireccion(); break;
    default: break;
    }
}

void vistaN2::alSoltar(int tecla)
{
    switch (tecla) {
    case Qt::Key_W: teclaW = false; actualizarDireccion(); break;
    case Qt::Key_A: teclaA = false; actualizarDireccion(); break;
    case Qt::Key_S: teclaS = false; actualizarDireccion(); break;
    case Qt::Key_D: teclaD = false; actualizarDireccion(); break;
    default: break;
    }
}

void vistaN2::alPerderFoco()
{
    teclaW = teclaA = teclaS = teclaD = false;
    actualizarDireccion();
}

void vistaN2::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        nivel.lanzarEscudo(mapToScene(e->pos()));      // posicion del click en coordenadas de la escena
    else
        vistaNivel::mousePressEvent(e);
}

const nivel2& vistaN2::getNivel() const
{ return nivel; }
bool vistaN2::getTeclaW() const
{ return teclaW; }
bool vistaN2::getTeclaA() const
{ return teclaA; }
bool vistaN2::getTeclaS() const
{ return teclaS; }
bool vistaN2::getTeclaD() const
{ return teclaD; }
