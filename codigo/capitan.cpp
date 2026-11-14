#include "capitan.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
using namespace std;

const double TIEMPO_INVULNERABLE = 1.5;
const double FACTOR_VELOZ        = 1.5;
capitan::capitan(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
                 bool usaGravedad_, int vidas_, int vidasMaximas_)
    : objetoCircular(pos, vel, masa_, radio_, usaGravedad_),vidas(0),vidasMax(1),T_invulnerable(0.0),
    T_veloz(0.0),enNube(false)
{
    setVidasMax(vidasMaximas_);
    setVidas(vidas_);
}
//movimiento
void capitan::actualizar(double dt)
{
    objetoCircular::actualizar(dt);
    if(T_invulnerable>0.0)
        {T_invulnerable=max(0.0,T_invulnerable-dt);}
    if(T_veloz>0.0)
        {T_veloz=max(0.0,T_veloz-dt);}
}
//acceso
int capitan::getVidas() const
{ return vidas; }

int capitan::getVidasMax() const
{ return vidasMax; }

double capitan::getT_invulnerable() const
{ return T_invulnerable; }

double capitan::getT_veloz() const
{ return T_veloz; }

bool capitan::getEnNube()const
{return enNube;}

void capitan::setVidas(int v) {
    if (v < 0 || v > vidasMax)
        throw invalid_argument("Las vidas deben estar entre 0 y las vidas maximas");
    vidas = v;
}

void capitan::setVidasMax(int m) {
    if (m < 1)
        throw invalid_argument("Las vidas maximas deben ser al menos 1");
    vidasMax = m;
    if (vidas > vidasMax)
        vidas = vidasMax;
}

void capitan::setT_invulnerable(double t) {
    if (t < 0.0)
        throw invalid_argument("El tiempo de invulnerabilidad no puede ser negativo");
    T_invulnerable = t;
}

void capitan::setT_veloz(double t) {
    if (t < 0.0)
        throw invalid_argument("El tiempo de veloz no puede ser negativo");
    T_veloz = t;
}

void capitan::setEnNube(bool n)
{enNube=n;}

//operadores
capitan& capitan::operator=(const capitan& o)
{
    if (this != &o) {
        objetoCircular::operator=(o);
        vidas          = o.vidas;
        vidasMax       = o.vidasMax;
        T_invulnerable = o.T_invulnerable;
        T_veloz        = o.T_veloz;
        enNube         = o.enNube;
    }
    return *this;
}

bool capitan::operator==(const capitan& o) const
{
    return objetoCircular::operator==(o)
           && vidas == o.vidas
           && vidasMax == o.vidasMax
           && abs(T_invulnerable - o.T_invulnerable) <= datos::EPSILON
           && abs(T_veloz - o.T_veloz) <= datos::EPSILON
           && enNube == o.enNube;
}

//jugabilidad
bool capitan::recibirDanio()
{
    if(!estaVivo() || T_invulnerable>0.0)
    {
        return false;
    }
    --vidas;
    T_invulnerable=TIEMPO_INVULNERABLE;
    return true;
}

bool capitan::ganarVida()
{
    if(vidas>=vidasMax)
        return false;
    vidas++;
    return true;
}

bool capitan::estaVivo()const
{return vidas>0;}

double capitan::getLimiteVel()const
{
    return (enNube)? datos::COEF_FRICCION: datos::LIMITE_VEL;
}

double capitan::getBufoVelocidad()const
{
    return (T_veloz>0.0)? FACTOR_VELOZ: 1.0;
}
