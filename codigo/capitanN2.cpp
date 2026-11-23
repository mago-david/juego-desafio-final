#include "capitanN2.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
using namespace std;
namespace
{
const int    VIDAS_MAX_N2   = 4;
const int    N_ESCUDOS      = 6;
const double VEL_ESCUDO     = 450.0;
const double RADIO_ESCUDO   = 12.0;
const int    REBOTES_ESCUDO = 3;
const double RETARDO_ESCUDO = 0.7;
const int DANIO_ESCUDO    = 10;
const int DANIO_FORTACHON = 15;
}

capitanN2::capitanN2(const QPointF& pos, int vidas_,bool forta_)
    : capitan(pos, QPointF(0.0, 0.0), 1.0, 20.0,
              false, vidas_, VIDAS_MAX_N2),
    direccionX(0), direccionY(0), tRecarga(0.0),fortachon(forta_)
{
    escudos.reserve(N_ESCUDOS);
    for (int i = 0; i < N_ESCUDOS; ++i) {
        // escudo: sin gravedad, rebota elastico (e = 1: conserva la energia)
        escudos.emplace_back(QPointF(), QPointF(), 1.0, RADIO_ESCUDO, false, REBOTES_ESCUDO, 1.0);
        escudos.back().setActivo(false);
    }
}

void capitanN2::actualizar(double dt)
{
    //normalizamos aceleracion para que no vayas mas rapido en diagonal
    const double n=hypot(direccionX,direccionY);
    const double a=datos::VEL_CORRER*datos::LIMITE_VEL*getBufoVelocidad();
    const double ax=(n>0.0)?a*direccionX/n:0.0;
    const double ay=(n>0.0)?a*direccionY/n:0.0;
    //v'=a-b*v(bizcoso)
    const double b=getLimiteVel();
    velocidad.setX(velocidad.x() + (ax - b * velocidad.x()) * dt);
    velocidad.setY(velocidad.y() + (ay - b * velocidad.y()) * dt);
    capitan::actualizar(dt);

    //reutilizacion ataque
    if (tRecarga > 0.0)
        tRecarga = max(0.0, tRecarga - dt);

    for (proyectil& e : escudos)
    {
        if (e.estaActivo())
            e.actualizar(dt);
    }
}

//accion
bool capitanN2:: lanzarEscudo(const QPointF& objetivo)
{
    if(tRecarga > 0.0)
        return false;
    const QPointF d=objetivo-getPosicion();
    const double dist=hypot(d.x(),d.y());

    if(dist<1.0)
        return false;
    for(proyectil& e: escudos)
    {
        if(!e.estaActivo())
        {
            e.reiniciar(getPosicion(),d*(VEL_ESCUDO/dist),REBOTES_ESCUDO);
            tRecarga=RETARDO_ESCUDO;
            return true;
        }
    }
    return false;
}

void capitanN2::resolverParedes(const vector<obstaculo>& paredes)
{
    for(const obstaculo& o:paredes)
    {
        o.colisionar(*this,0.0);
        for(proyectil& e:escudos)
        {
            if(e.estaActivo()&&o.colisionar(e,e.getCoefRestitucion()))
                e.registrarRebote();
        }
    }
}
//version para el jefe
int capitanN2::escudosGolpean(const obstaculo& blanco)
{
    int golpes=0;
    for(proyectil& e: escudos)
    {
        if(e.estaActivo() && blanco.tocado(e))
        {
            e.setActivo(false);
            ++golpes;
        }
    }
    return golpes;
}
//version para los esbirros
int capitanN2::escudosGolpean(const objetoCircular& blanco)
{
    int golpes = 0;
    for (proyectil& e : escudos)
    {
        if (e.estaActivo() && e.circuloVScirculo(blanco))
        {
            e.setActivo(false);
            ++golpes;
        }
    }
    return golpes;
}
// Acceso
int capitanN2::getDireccionX() const
{ return direccionX; }

int capitanN2::getDireccionY() const
{ return direccionY; }

double capitanN2::getTRecarga() const
{ return tRecarga; }

const vector<proyectil>& capitanN2::getEscudos() const
{ return escudos; }

void capitanN2::setDireccionX(int d) {
    if (d < -1 || d > 1)
        throw invalid_argument("La direccion X debe ser -1, 0 o 1");
    direccionX = d;
}

void capitanN2::setDireccionY(int d) {
    if (d < -1 || d > 1)
        throw invalid_argument("La direccion Y debe ser -1, 0 o 1");
    direccionY = d;
}

void capitanN2::setTRecarga(double t) {
    if (t < 0.0)
        throw invalid_argument("La recarga no puede ser negativa");
    tRecarga = t;
}

void capitanN2::setEscudos(const vector<proyectil>& e) {
    if (e.size() != escudos.size())
        throw invalid_argument("El pool de escudos conserva su tamano fijo");
    escudos = e;
}

bool capitanN2::getFortachon() const
{ return fortachon; }

void capitanN2::setFortachon(bool f)
{ fortachon = f; }


int capitanN2::getDanioEscudo() const
{ return fortachon ? DANIO_FORTACHON : DANIO_ESCUDO; }

// Operadores
capitanN2& capitanN2::operator=(const capitanN2& o) {
    if (this != &o) {
        capitan::operator=(o);
        direccionX = o.direccionX;
        direccionY = o.direccionY;
        tRecarga   = o.tRecarga;
        escudos    = o.escudos;
        fortachon= o.fortachon;
    }
    return *this;
}

bool capitanN2::operator==(const capitanN2& o) const {
    return capitan::operator==(o)
    && direccionX == o.direccionX
    && direccionY == o.direccionY
    && abs(tRecarga - o.tRecarga) <= datos::EPSILON
    && escudos == o.escudos
    && fortachon == o.fortachon;
}









