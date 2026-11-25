#include "muroPerseguidor.h"
#include "datos.h"
#include <stdexcept>
#include <cmath>
muroPerseguidor::muroPerseguidor()
    : zona(QPointF(0.0, 0.0), 1.0, 1.0), velocidad(60.0), activo(false) {}
muroPerseguidor::muroPerseguidor(const QPointF& esquina_, double ancho_, double alto_,
                                 double velocidad_)
    : zona(esquina_, ancho_, alto_), velocidad(0.0), activo(false)
{
    setvelocidad(velocidad_);
}
muroPerseguidor::muroPerseguidor(const muroPerseguidor& o)
    : zona(o), velocidad(o.velocidad), activo(o.activo) {}
muroPerseguidor::~muroPerseguidor() {}
void muroPerseguidor::actualizar(double dt)
{
    if (!activo)
        return;
    setEsquina(QPointF(getEsquina().x()+velocidad*dt,getEsquina().y()));
}
bool muroPerseguidor::aplicar(capitan& c) const
{
    if (!activo || !c.estaVivo())
        return false;
    if (c.getPosicion().x() > getDerecha())
        return false;
    c.setVidas(0);
    return true;
}
double muroPerseguidor::getvelocidad()const{return velocidad;}
bool muroPerseguidor::getactivo()const{return activo;}
void muroPerseguidor::setactivo(bool a){activo = a;}
void muroPerseguidor::setvelocidad(double v)
{
    if (v<0.0)
        throw std::invalid_argument("para que el muro te persiga tiene que avanzar no retroceder");
    velocidad=v;
}
muroPerseguidor& muroPerseguidor::operator=(const muroPerseguidor& o)
{
    if (this !=&o) {
        zona::operator=(o);
        velocidad=o.velocidad;
        activo=o.activo;
    }
    return *this;
}
bool muroPerseguidor::operator==(const muroPerseguidor& o) const
{
    return zona::operator==(o)
    && std::abs(velocidad - o.velocidad) <= datos::EPSILON
                && activo == o.activo;
}


