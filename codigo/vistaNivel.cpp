#include "vistaNivel.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <algorithm>

vistaNivel::vistaNivel(QWidget* parent)
    : QGraphicsView(parent),
    reloj(new QTimer(this)),
    acumulado(0.0),
    pausado(true),
    escena(new QGraphicsScene(this))
{
    setScene(escena);
    //el metodo de arbol no nos sirve
    escena->setItemIndexMethod(QGraphicsScene::NoIndex);
    //no barra de desplazamiento
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //no marco
    setFrameShape(QFrame::NoFrame);
    //centramos en caso de pantalla mas grande
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //el focus para los controles
    setFocusPolicy(Qt::StrongFocus);
    //precise para evitar tirones
    reloj->setTimerType(Qt::PreciseTimer);
    connect(reloj, &QTimer::timeout, this, &vistaNivel::avanzar);
    //el tiempo incia en cada nivel osea cada vista
}

//"momentos"

void vistaNivel::iniciar()
{
    acumulado=0.0;
    cronometro.start();
    pausado=false;
    reloj->start(16);
    setFocus();
}

void vistaNivel::pausar()
{
    pausado=true;
    reloj->stop();
}

void vistaNivel::terminar(bool gano)
{
    pausar();
    emit terminado(gano);
}

//ejecucion
void vistaNivel::avanzar()
{
    //tiempo real
    //seguridad para no encadenar lageos
    acumulado+=std::min(cronometro.nsecsElapsed()*datos::EPSILON,0.1);
    cronometro.start();

    while(!pausado && acumulado>=datos::DT)
    {
        avanzarLogica(datos::DT);
        acumulado-=datos::DT;
    }
    //sin esto solo se dibuja en cada paso y da tirones
    if(!pausado)
        dibujar(acumulado/datos::DT);
}

//controles
void vistaNivel::keyPressEvent(QKeyEvent* e) {
    if (e->isAutoRepeat())
        return;
    alPulsar(e->key());
}

void vistaNivel::keyReleaseEvent(QKeyEvent* e) {
    if (e->isAutoRepeat())
        return;
    alSoltar(e->key());
}

void vistaNivel::focusOutEvent(QFocusEvent* e) {
    alPerderFoco();
    QGraphicsView::focusOutEvent(e);
}

void vistaNivel::alPerderFoco() {}

double vistaNivel::getAcumulado() const
{ return acumulado; }
bool vistaNivel::getPausado() const
{ return pausado; }



