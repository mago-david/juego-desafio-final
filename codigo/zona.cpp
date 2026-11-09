#include "zona.h"
//#include "datos.h"
#include <stdexcept>
#include <cmath>
using namespace std;
zona::zona(const vector2D& esquina_, double ancho_, double alto_)
    : esquina(esquina_), ancho(1.0), alto(1.0)
{
    setAncho(ancho_);
    setAlto(alto_);
}

//acceso
const vector2D& zona::getEsquina() const
{ return esquina; }

double zona::getAncho() const
{ return ancho; }

double zona::getAlto() const
{ return alto; }

void zona::setEsquina(const vector2D& e)
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
{ return esquina.getX(); }

double zona::getDerecha() const
{ return esquina.getX() + ancho; }

double zona::getArriba() const
{ return esquina.getY(); }

double zona::getAbajo() const
{ return esquina.getY() + alto; }

//adentro?
bool zona::contiene(const entidadFisica& e) const
{
    const vector2D& c = e.getPosicion();
    return c.getX() >= getIzquierda() && c.getX() <= getDerecha()
           && c.getY() >= getArriba()    && c.getY() <= getAbajo();
}







