#include "capitanN1.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
using namespace std;

const double VEL_SALTO=350.0;
const double TOLERANCIA_SUELO = 1.0;

//capitanN1::capitanN1() {}
capitanN1::capitanN1(const QPointF& pos, int vidas_)
    : capitan(pos, QPointF(0.0, 0.0), 1.0, 20.0,
              true, vidas_, datos::VIDAS_INICIALES),
    direccion(0),
    saltosRestantes(2),
    enSuelo(false),
    posicionAnterior(pos),
    T_sinPlataformas(0.0) {}

void capitanN1::actualizar(double dt)
{
    posicionAnterior=posicion;
    if (T_sinPlataformas > 0.0)
        T_sinPlataformas = max(0.0, T_sinPlataformas - dt);
    const double a=datos::VEL_CORRER*datos::LIMITE_VEL*getBufoVelocidad();
    //vx'=(dir*a-b*vx) vel max=a/b
    velocidad.setX( velocidad.x() + (direccion * a - getLimiteVel() * velocidad.x()) * dt );
    capitan::actualizar(dt);
}

//aceso
int capitanN1::getDireccion() const
{ return direccion; }

void capitanN1::setDireccion(int d)
{
    if (d < -1 || d > 1)
        throw invalid_argument("La direccion de entrada debe ser -1, 0 o 1");
    direccion = d;
}
int capitanN1::getSaltosRestantes() const
{ return saltosRestantes; }

void capitanN1::setSaltosRestantes(int s) {
    if (s < 0 || s > 2)
        throw invalid_argument("Saltos fuera de rango");
    saltosRestantes = s;
}
bool capitanN1::getEnSuelo() const
{ return enSuelo; }

void capitanN1::setEnSuelo(bool e)
{ enSuelo = e; }

const QPointF& capitanN1::getPosicionAnterior() const
{ return posicionAnterior; }

void capitanN1::setPosicionAnterior(const QPointF& p)
{ posicionAnterior = p; }

double capitanN1::getT_sinPlataformas() const
{ return T_sinPlataformas; }

void capitanN1::setT_sinPlataformas(double t) {
    if (t < 0.0)
        throw invalid_argument("El tiempo sin plataformas no puede ser negativo");
    T_sinPlataformas = t;
}

//operadores
capitanN1& capitanN1::operator=(const capitanN1& o) {
    if (this != &o) {
        capitan::operator=(o);         // la parte heredada la copia la base
        direccion   = o.direccion;
        saltosRestantes    = o.saltosRestantes;
        enSuelo            = o.enSuelo;
        posicionAnterior   = o.posicionAnterior;
        T_sinPlataformas   = o.T_sinPlataformas;
    }
    return *this;
}

bool capitanN1::operator==(const capitanN1& o) const {
    return (capitan::operator==(o)
    && direccion == o.direccion
    && saltosRestantes == o.saltosRestantes
    && enSuelo == o.enSuelo
    && posicionAnterior == o.posicionAnterior
    && abs(T_sinPlataformas - o.T_sinPlataformas) <= datos::EPSILON);
}

//acciones

bool capitanN1::saltar()
{
    if(saltosRestantes>0)
    {
        velocidad.setY(-VEL_SALTO);
        --saltosRestantes;
        enSuelo=false;
        return true;
    }
    return false;
}

void capitanN1::resolverPlataformas(const vector<obstaculo>& plataformas)
{
    const double r=getRadio();
    enSuelo=false;
    for(const obstaculo& o:plataformas)
    {
        if(o.getAtravesable())
        {
            if(T_sinPlataformas>0.0)
                continue;
            if(velocidad.y()<=0.0)
                continue;
            if(posicionAnterior.y() + r >o.getArriba()+TOLERANCIA_SUELO)
                continue;
        }
        o.colisionar(*this,0.0);
        //en el primer verifica que el punto mas bajo este arriba de la plataforma
        //en los otros 2 que su x este entre los extremos del rec
        if(abs( (posicion.y()+r) - o.getArriba()) <= TOLERANCIA_SUELO
            && posicion.x()>=o.getIzquierda() && posicion.x()<=o.getDerecha() )
        {enSuelo=true;}
    }
    if(enSuelo)
        saltosRestantes=2;
    else if(saltosRestantes>1)
        saltosRestantes=1;
}


bool capitanN1:: bajarPlataforma()
{
    if(!enSuelo)
        return false;
    T_sinPlataformas=0.2;
    return true;
}















