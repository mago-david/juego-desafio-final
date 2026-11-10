#include "jugador.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
using namespace std;
jugador::jugador(const vector2D& pos, const vector2D& vel, double masa_, double radio_,
                 bool usaGravedad_, int vidas_, int vidasMaximas_)
    : entidadFisica(pos, vel, masa_, radio_, usaGravedad_),vidas(0),vidasMax(1),T_invulnerable(0.0),
    T_veloz(0.0),enNube(false)
{
    setVidasMax(vidasMaximas_);
    setVidas(vidas_);
}
//movimiento
void jugador::actualizar(double dt)
{
    entidadFisica::actualizar(dt);
    if(T_invulnerable>0.0)
        {T_invulnerable=max(0.0,T_invulnerable-dt);}
    if(T_veloz>0.0)
        {T_veloz=max(0.0,T_veloz-dt);}
}
//acceso
int jugador::getVidas() const
{ return vidas; }

int jugador::getVidasMax() const
{ return vidasMax; }

double jugador::getT_invulnerable() const
{ return T_invulnerable; }

double jugador::getT_veloz() const
{ return T_veloz; }

bool jugador::getEnNube()const
{return enNube;}

void jugador::setVidas(int v) {
    if (v < 0 || v > vidasMax)
        throw invalid_argument("Las vidas deben estar entre 0 y las vidas maximas");
    vidas = v;
}

void jugador::setVidasMax(int m) {
    if (m < 1)
        throw invalid_argument("Las vidas maximas deben ser al menos 1");
    vidasMax = m;
    if (vidas > vidasMax)
        vidas = vidasMax;
}

void jugador::setT_invulnerable(double t) {
    if (t < 0.0)
        throw invalid_argument("El tiempo de invulnerabilidad no puede ser negativo");
    T_invulnerable = t;
}

void jugador::setT_veloz(double t) {
    if (t < 0.0)
        throw invalid_argument("El tiempo de veloz no puede ser negativo");
    T_veloz = t;
}

void jugador::setEnNube(bool n)
{enNube=n;}

//jugabilidad
bool jugador::recibirDanio()
{
    if(!estaVivo() || T_invulnerable>0.0)
    {
        return false;
    }
    --vidas;
    T_invulnerable=datos::TIEMPO_INVULNERABLE;
    return true;
}

bool jugador::ganarVida()
{
    if(vidas>=vidasMax)
        return false;
    vidas++;
    return true;
}

bool jugador::estaVivo()const
{return vidas>0;}

double jugador::getLimiteVel()const
{
    return (enNube)? datos::COEF_FRICCION: datos::LIMITE_VEL;
}

