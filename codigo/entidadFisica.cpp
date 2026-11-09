#include "entidadFisica.h"
#include <stdexcept>
using namespace std;
#include "datos.h"
#include <cmath>

entidadFisica::entidadFisica(const vector2D& pos, const vector2D& vel,
                             double masa_, double radio_, bool usaGravedad_)
    : velocidad(vel), posicion(pos), masa(1.0), radio(1.0), gravedad(usaGravedad_)
{
    setMasa(masa_);
    setRadio(radio_);
}

entidadFisica::~entidadFisica() {}


void entidadFisica::actualizar(double dt) {
    const vector2D a(0.0, gravedad ? datos::GRAVEDAD : 0.0);
    posicion  += velocidad * dt + a * (0.5 * dt * dt);
    //r = r0 + v*dt + 1/2*a*dt^2
    velocidad += a * dt;
}

// acceso
const vector2D& entidadFisica::getPosicion() const
{ return posicion; }
const vector2D& entidadFisica::getVelocidad() const
{ return velocidad; }
double entidadFisica::getMasa() const
{ return masa; }
double entidadFisica::getRadio() const
{ return radio; }
bool entidadFisica::getUsaGravedad() const
{ return gravedad; }

void entidadFisica::setPosicion(const vector2D& p)
{ posicion = p; }
void entidadFisica::setVelocidad(const vector2D& v)
{ velocidad = v; }
void entidadFisica::setUsaGravedad(bool g)
{ gravedad = g; }

void entidadFisica::setMasa(double m) {
    if (m <= 0.0)
        throw invalid_argument("La masa debe ser mayor que cero");
    masa = m;
}

void entidadFisica::setRadio(double r) {
    if (r <= 0.0)
        throw invalid_argument("El radio debe ser mayor que cero");
    radio = r;
}

//magnitudes fisicas
vector2D entidadFisica::momento() const {
    return velocidad * masa;
}

double entidadFisica::energiaCinetica() const {
    return 0.5 * masa * velocidad.magnitudP2();
}

// operadores
entidadFisica& entidadFisica::operator=(const entidadFisica& o) {
    if (this != &o) {
        posicion    = o.posicion;
        velocidad   = o.velocidad;
        masa        = o.masa;
        radio       = o.radio;
        gravedad = o.gravedad;
    }
    return *this;
}

//bool entidadFisica::operator==(const entidadFisica& o) const {
    //return posicion == o.posicion
           //&& velocidad == o.velocidad
          // && abs(masa - o.masa) <= datos::EPSILON
           //&& abs(radio - o.radio) <= datos::EPSILON
          // && gravedad == o.gravedad;
//}

