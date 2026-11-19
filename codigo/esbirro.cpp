#include "esbirro.h"
#include <stdexcept>
#include <cmath>
using namespace std;

namespace
{
const double VEL_ESBIRRO   = 60.0;
const double MASA_ESBIRRO  = 1.0;
const double RADIO_ESBIRRO = 16.0;
}

esbirro::esbirro()
    : objetoCircular(QPointF(0.0, 0.0), QPointF(0.0, 0.0), MASA_ESBIRRO, RADIO_ESBIRRO, false),
    vida(0)
{}

void esbirro::reiniciar(const QPointF& pos,int vida)
{
    setPosicion(pos);
    setVelocidad(QPointF(0.0,0.0));
    setVida(vida);
}

void esbirro::perseguir(const QPointF& objetivo)
{
    QPointF d=objetivo-posicion;
    double n=hypot(d.x(),d.y());
    if(n<1.0)
        velocidad=QPointF(0.0,0.0);
    else
        velocidad=d*(VEL_ESBIRRO/n);
}

void esbirro::recibirGolpe()
{
    if(vida>0)
        --vida;
}

bool esbirro::estaVivo() const
{return vida>0;}

//acceso
int esbirro::getVida() const
{ return vida; }

void esbirro::setVida(int v)
{
    if (v < 0) throw invalid_argument("La vida del esbirro no puede ser negativa");
    vida = v;
}

//operadores
esbirro& esbirro::operator=(const esbirro& o)
{
    if (this != &o) {
        objetoCircular::operator=(o);
        vida = o.vida;
    }
    return *this;
}

bool esbirro::operator==(const esbirro& o) const
{
    return objetoCircular::operator==(o) && vida == o.vida;
}

















