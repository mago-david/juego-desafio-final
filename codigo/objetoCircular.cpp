#include "objetoCircular.h"
#include <stdexcept>
using namespace std;
#include "datos.h"
#include <cmath>

objetoCircular::objetoCircular(const QPointF& pos, const QPointF& vel,
                               double masa_, double radio_, bool usaGravedad_)
    : velocidad(vel), posicion(pos), masa(1.0), radio(1.0), gravedad(usaGravedad_)
{
    setMasa(masa_);
    setRadio(radio_);
}

objetoCircular::~objetoCircular() {}


void objetoCircular::actualizar(double dt) {
    const QPointF a(0.0, gravedad ? datos::GRAVEDAD : 0.0);
    //r = r0 + v*dt + 1/2*a*dt^2
    posicion  += velocidad * dt + a * (0.5 * dt * dt);
    velocidad += a * dt;
}

// acceso
const QPointF& objetoCircular::getPosicion() const
{ return posicion; }

const QPointF& objetoCircular::getVelocidad() const
{ return velocidad; }

double objetoCircular::getMasa() const
{ return masa; }

double objetoCircular::getRadio() const
{ return radio; }

bool objetoCircular::getUsaGravedad() const
{ return gravedad; }

void objetoCircular::setPosicion(const QPointF& p)
{ posicion = p; }

void objetoCircular::setVelocidad(const QPointF& v)
{ velocidad = v; }

void objetoCircular::setUsaGravedad(bool g)
{ gravedad = g; }

void objetoCircular::setMasa(double m) {
    if (m <= 0.0)
        throw invalid_argument("La masa debe ser mayor que cero");
    masa = m;
}

void objetoCircular::setRadio(double r) {
    if (r <= 0.0)
        throw invalid_argument("El radio debe ser mayor que cero");
    radio = r;
}

// operadores
objetoCircular& objetoCircular::operator=(const objetoCircular& o) {
    if (this != &o) {
        posicion  = o.posicion;
        velocidad = o.velocidad;
        masa      = o.masa;
        radio     = o.radio;
        gravedad  = o.gravedad;
    }
    return *this;
}

bool objetoCircular::operator==(const objetoCircular& o) const {
    return posicion == o.posicion
           && velocidad == o.velocidad
           && abs(masa - o.masa) <= datos::EPSILON
           && abs(radio - o.radio) <= datos::EPSILON
           && gravedad == o.gravedad;
}
