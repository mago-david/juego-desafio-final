#include "zona.h"
#include "datos.h"
#include <stdexcept>
#include <cmath>
using namespace std;
zona::zona(const QPointF& esquina_, double ancho_, double alto_)
    : esquina(esquina_), ancho(1.0), alto(1.0)
{
    setAncho(ancho_);
    setAlto(alto_);
}

//acceso
const QPointF& zona::getEsquina() const
{ return esquina; }

double zona::getAncho() const
{ return ancho; }

double zona::getAlto() const
{ return alto; }

void zona::setEsquina(const QPointF& e)
{ esquina = e; }

void zona::setAncho(double a) {
    if (a <= 0.0)
        throw invalid_argument("El ancho de la zona debe ser mayor que cero");
    ancho = a;
}

void zona::setAlto(double a) {
    if (a <= 0.0)
        throw invalid_argument("El alto de la zona debe ser mayor que cero");
    alto = a;
}
//bordes
double zona::getIzquierda() const
{ return esquina.x(); }

double zona::getDerecha() const
{ return esquina.x() + ancho; }

double zona::getArriba() const
{ return esquina.y(); }

double zona::getAbajo() const
{ return esquina.y() + alto; }

//operadores
zona& zona::operator=(const zona& o) {
    if (this != &o) {
        esquina = o.esquina;
        ancho   = o.ancho;
        alto    = o.alto;
    }
    return *this;
}

bool zona::operator==(const zona& o) const {
    return esquina == o.esquina
           && abs(ancho - o.ancho) <= datos::EPSILON
           && abs(alto - o.alto) <= datos::EPSILON;
}

//adentro?
bool zona::contiene(const objetoCircular& e) const
{
    const QPointF& c = e.getPosicion();
    return c.x() >= getIzquierda() && c.x() <= getDerecha()
           && c.y() >= getArriba()    && c.y() <= getAbajo();
}
