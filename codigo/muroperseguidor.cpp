#include "muroperseguidor.h"
#include "datos.h"
#include <stdexcept>
#include <cmath>
muroperseguidor::muroperseguidor()
    : zona(QPointF(0.0, 0.0), 1.0, 1.0), velocidad(60.0), activo(true) {}
muroperseguidor::muroperseguidor(const QPointF& esquina_, double ancho_, double alto_,
                                 double velocidad_)
    : zona(esquina_, ancho_, alto_), velocidad(0.0), activo(true)
{
    setvelocidad(velocidad_);
}
muroperseguidor::muroperseguidor(const muroperseguidor& o)
    : zona(o), velocidad(o.velocidad), activo(o.activo) {}
muroperseguidor::~muroperseguidor() {}
void muroperseguidor::actualizar(double dt)
{
    if (!activo)
        return;
    setEsquina(QPointF(getEsquina().x()+velocidad*dt,getEsquina().y()));
}
bool muroperseguidor::aplicar(capitan& c) const
{
    if (!contiene(c))
        return false;
    c.setVidas(0);
    return true;
}
double muroperseguidor::getvelocidad()const{return velocidad;}
bool muroperseguidor::getactivo()const{return activo;}
void muroperseguidor::setactivo(bool a){activo = a;}
void muroperseguidor::setvelocidad(double v)
{
    if (v<0.0)
        throw std::invalid_argument("para que el muro te persiga tiene que avanzar no retroceder");
    velocidad=v;
}
muroperseguidor& muroperseguidor::operator=(const muroperseguidor& o)
{
    if (this !=&o) {
        zona::operator=(o);
        velocidad=o.velocidad;
        activo=o.activo;
    }
    return *this;
}
bool muroperseguidor::operator==(const muroperseguidor& o) const
{
    return zona::operator==(o)
    && std::abs(velocidad - o.velocidad) <= datos::EPSILON
                && activo == o.activo;
}

