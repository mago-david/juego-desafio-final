#include "efecto.h"
#include "datos.h"
#include "capitanN2.h"
#include <stdexcept>
#include <cmath>

efecto::efecto()
    : zona(QPointF(0.0, 0.0), 1.0, 1.0), tipo(gas), duracion(3.0),
    fuerzarebote(250.0), aviso(0.0), activo(false)
{}

efecto::efecto(const QPointF& esquina_, double ancho_, double alto_, tipoefecto tipo_,
               double duracion_, double fuerzarebote_)
    : zona(esquina_, ancho_, alto_), tipo(tipo_), duracion(1.0),
    fuerzarebote(0.0), aviso(0.0), activo(true)
{
    setduracion(duracion_);
    setfuerzarebote(fuerzarebote_);
}

efecto::efecto(const efecto& o)
    : zona(o), tipo(o.tipo), duracion(o.duracion), fuerzarebote(o.fuerzarebote),
    aviso(o.aviso), activo(o.activo)
{}

efecto::~efecto() {}

bool efecto::esTemporal() const
{
    return tipo == gas || tipo == senal;
}

void efecto::actualizar(double dt)
{
   if (!activo || !esTemporal())
        return;
    duracion-=dt;
    if(duracion<=0.0)
    {
        duracion=0.0;
        activo=false;
    }
}

void efecto::activar(const QPointF& esquina_, double duracion_)
{
    setEsquina(esquina_);
    setduracion(duracion_);
    aviso = 0.0;
    activo = true;
}

void efecto::activar(const QPointF& esquina_, double ancho_, double alto_,
                     tipoefecto tipo_, double aviso_, double duracion_)
{
    setEsquina(esquina_);
    setAncho(ancho_);
    setAlto(alto_);
    tipo = tipo_;
    setaviso(aviso_);
    setduracion(duracion_);
    activo = true;
}

bool efecto::aplicar(capitan& c)
{
    if (!activo || aviso > 0.0 || !contiene(c))
        return false;
    switch (tipo) {
    case gas:
        c.setEnNube(true);
        return true;
    case bufo: {
        c.ganarVida();
        capitanN2* capN2 = dynamic_cast<capitanN2*>(&c);
        if (capN2 != nullptr) {
            capN2->activarBufoDanio(duracion);
        } else {
            c.setT_veloz(duracion);
        }
        activo = false;
        return true;
    }
    case danio: {
        if (!c.recibirDanio())
            return false;
        const double centroX = getIzquierda() + getAncho() / 2.0;
        const double lado = (c.getPosicion().x() < centroX) ? -1.0 : 1.0;
        c.setVelocidad(QPointF(lado * fuerzarebote, -fuerzarebote));
        return true;
    }
    case senal:
        return false;
    }
    return false;
}

efecto::tipoefecto efecto::gettipo() const { return tipo; }
double efecto::getduracion() const { return duracion; }
double efecto::getfuerzarebote() const { return fuerzarebote; }
double efecto::getaviso() const { return aviso; }
bool efecto::getactivo() const { return activo; }
void efecto::settipo(tipoefecto t) { tipo = t; }
void efecto::setactivo(bool a) { activo = a; }
void efecto::setduracion(double d)
{
    if (d <= 0.0 && esTemporal())
        throw std::invalid_argument("pana si me vez es porque quieres colocar un efecto que dure 0 segundos todo bien?");
    duracion = d;
}

void efecto::setfuerzarebote(double f)
{
    if (f < 0.0)
        throw std::invalid_argument("La fuerza de rebote no puede ser negativa");
    fuerzarebote = f;
}

void efecto::setaviso(double a)
{
    if (a < 0.0)
        throw std::invalid_argument("El aviso no puede ser negativo");
    aviso = a;
}

efecto& efecto::operator=(const efecto& o)
{
    if (this != &o) {
        zona::operator=(o);
        tipo         = o.tipo;
        duracion     = o.duracion;
        fuerzarebote = o.fuerzarebote;
        aviso        = o.aviso;
        activo       = o.activo;
    }
    return *this;
}

bool efecto::operator==(const efecto& o) const
{
    return zona::operator==(o)
    && tipo == o.tipo
                      && std::abs(duracion - o.duracion) <= datos::EPSILON
                      && std::abs(fuerzarebote - o.fuerzarebote) <= datos::EPSILON
                      && std::abs(aviso - o.aviso) <= datos::EPSILON
                      && activo == o.activo;
}