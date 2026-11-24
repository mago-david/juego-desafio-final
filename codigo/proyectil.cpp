#include "proyectil.h"
#include <stdexcept>
#include "datos.h"
#include <cmath>
using namespace std;
proyectil::proyectil() : proyectil(QPointF(0,0), QPointF(0,0),
                1.0, 1.0, false, 0, 1.0)
{}
proyectil::proyectil(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
                     bool usaGravedad_, int rebotes_, double coefRestitucion_)
    : objetoCircular(pos, vel, masa_, radio_, usaGravedad_),
    rebotesMax(0), coefRestitucion(0.0), activo(true)
{
    setRebotesMax(rebotes_);
    setCoefRestitucion(coefRestitucion_);
}
proyectil::~proyectil() {}
//reutilizar
void proyectil::reiniciar(const QPointF& pos,const QPointF& vel, int rm)
{
    setRebotesMax(rm);
    setPosicion(pos);
    setVelocidad(vel);
    activo=true;
}

void proyectil::registrarRebote()
{
    if(rebotesMax>0)
        --rebotesMax;
    else
        activo=false;
}
//acceso
int proyectil::getRebotesMax() const
{ return rebotesMax; }

double proyectil::getCoefRestitucion() const
{ return coefRestitucion; }

bool proyectil::estaActivo() const
{ return activo; }

void proyectil::setRebotesMax(int r) {
    if (r < 0)
        throw invalid_argument("Los rebotes restantes no pueden ser negativos");
    rebotesMax = r;
}

void proyectil::setCoefRestitucion(double e) {
    if (e < 0.0 || e > 1.0)
        throw out_of_range("El coeficiente de restitucion debe estar entre 0 y 1");
    coefRestitucion = e;
}

void proyectil::setActivo(bool a)
{ activo = a; }

//operadores
proyectil& proyectil::operator=(const proyectil& o) {
    if (this != &o) {
        objetoCircular::operator=(o);       // la parte heredada la copia la base
        rebotesMax = o.rebotesMax;
        coefRestitucion  = o.coefRestitucion;
        activo           = o.activo;
    }
    return *this;
}

bool proyectil::operator==(const proyectil& o) const {
    return objetoCircular::operator==(o)
    && rebotesMax == o.rebotesMax
    && abs(coefRestitucion - o.coefRestitucion) <= datos::EPSILON
    && activo == o.activo;
}