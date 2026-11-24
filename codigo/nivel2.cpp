#include "nivel2.h"
#include "datos.h"
#include <stdexcept>

namespace
{
const double DY_INICIO  = 180.0;   // el capi empieza debajo del jefe
const int    VIDA_JEFE  = 525;     // escala x10: el escudo hace 10 (15 fortachon)
const int    N_EFECTOS  = 16;      // peor caso: 7 nubes vivas + 7 señales de otra lluvia
const int    N_BALAS    = 10;
const double MASA_BALA  = 1.0;     // la base exige > 0
const double RADIO_BALA = 6.0;     // <= MARGEN_BALA de jefe.cpp
const int    N_ESBIRROS = 4;       // tanda viva a la vez
const bool   DIFICIL    = true;
}

nivel2::nivel2(bool dificil, bool fortachon)
    : personaje(QPointF(datos::CENTRO_X, datos::CENTRO_Y + DY_INICIO), datos::VIDAS_INICIALES, fortachon),
    jefeFinal(VIDA_JEFE, dificil), efectos(N_EFECTOS), balas(), esbirros(N_ESBIRROS)
{
    construirArena();
    construirBalas();
}

nivel2::nivel2() : nivel2(DIFICIL, false)
{}

nivel2::~nivel2() {}

void nivel2::construirArena()
{
    paredes.reserve(5);
    const double g = datos::GROSOR_PARED;
    paredes.emplace_back(QPointF(datos::X_ARENA, 0.0), datos::LADO_ARENA, g);                            // arriba
    paredes.emplace_back(QPointF(datos::X_ARENA, datos::LADO_ARENA - g), datos::LADO_ARENA, g);                 // abajo
    paredes.emplace_back(QPointF(datos::X_ARENA, g), g, datos::LADO_ARENA - 2.0 * g);                    // izquierda
    paredes.emplace_back(QPointF(datos::X_ARENA + datos::LADO_ARENA - g, g), g, datos::LADO_ARENA - 2.0 * g);   // derech
    paredes.emplace_back(jefeFinal.getCuerpo());
}

void nivel2::construirBalas()
{
    balas.reserve(N_BALAS);
    for (int i = 0; i < N_BALAS; ++i)
    {
        // sin gravedad, 0 rebotes, e = 1
        balas.emplace_back(QPointF(), QPointF(), MASA_BALA, RADIO_BALA, false, 0, 1.0);
        balas.back().setActivo(false);
    }
}

void nivel2::actualizar(double dt)
{
    personaje.actualizar(dt);
    const int golpes = personaje.escudosGolpean(jefeFinal.getCuerpo());
    if (golpes > 0)
        jefeFinal.recibirDanio(golpes * personaje.getDanioEscudo());

    personaje.resolverParedes(paredes);

    jefeFinal.actualizar(dt,personaje.getPosicion(),personaje.getVelocidad(),
                         efectos,balas,esbirros);

    actualizarBalas(dt);
    actualizarEfectos(dt);
    actualizarEsbirros(dt);
}

void nivel2::actualizarBalas(double dt)
{
    for(proyectil& b: balas)
    {
        if(!b.estaActivo())
            continue;
        b.actualizar(dt);
        for(const obstaculo& o: paredes)
        {
            if(!b.estaActivo())
                break;
            if(o.colisionar(b,b.getCoefRestitucion()))
                b.registrarRebote();
        }
        if(b.estaActivo()&&personaje.circuloVScirculo(b))
        {
            personaje.recibirDanio();
            b.setActivo(false);
        }
    }
}

void nivel2::actualizarEfectos(double dt)
{
    personaje.setEnNube(false);
    for(efecto& e:efectos)
    {
        e.actualizar(dt);
        e.aplicar(personaje);
    }
}

void nivel2::actualizarEsbirros(double dt)
{
    const int danio=personaje.getDanioEscudo();
    for(esbirro& s:esbirros)
    {
        if(!s.estaVivo())
            continue;
        s.perseguir(personaje.getPosicion());
        s.actualizar(dt);

        for(const obstaculo& o:paredes)
        {
            o.colisionar(s,0.0);
        }
        const int g=personaje.escudosGolpean(s);
        if(g>0)
            s.recibirGolpe(g*danio);
        if(s.estaVivo() && personaje.circuloVScirculo(s))
            personaje.recibirDanio();
    }
}
//atajos

void nivel2::setDireccionPersonaje(int dx, int dy)
{
    personaje.setDireccionX(dx);
    personaje.setDireccionY(dy);
}

bool nivel2::lanzarEscudo(const QPointF& objetivo)
{ return personaje.lanzarEscudo(objetivo); }

// acceso
const std::vector<obstaculo>& nivel2::getParedes() const
{ return paredes; }

const capitanN2& nivel2::getPersonaje() const
{ return personaje; }

const jefe& nivel2::getJefe() const
{ return jefeFinal; }

const std::vector<efecto>& nivel2::getEfectos() const
{ return efectos; }

const std::vector<proyectil>& nivel2::getBalas() const
{ return balas; }

void nivel2::setParedes(const std::vector<obstaculo>& p)
{ paredes = p; }

void nivel2::setPersonaje(const capitanN2& p)
{ personaje = p; }

void nivel2::setJefe(const jefe& j)
{ jefeFinal = j; }

void nivel2::setEfectos(const std::vector<efecto>& e)
{
    if (e.size() != efectos.size())
        throw std::invalid_argument("La reserva de efectos conserva su tamano fijo");
    efectos = e;
}

void nivel2::setBalas(const std::vector<proyectil>& b)
{
    if (b.size() != balas.size())
        throw std::invalid_argument("El pool de balas conserva su tamano fijo");
    balas = b;
}

const std::vector<esbirro>& nivel2::getEsbirros() const
{ return esbirros; }

void nivel2::setEsbirros(const std::vector<esbirro>& e)
{
    if (e.size() != esbirros.size())
        throw std::invalid_argument("El pool de esbirros conserva su tamano fijo");
    esbirros = e;
}

// operadores
nivel2& nivel2::operator=(const nivel2& o)
{
    if (this != &o)
    {
        paredes   = o.paredes;
        personaje = o.personaje;
        jefeFinal = o.jefeFinal;
        efectos   = o.efectos;
        balas     = o.balas;
        esbirros=o.esbirros;
    }
    return *this;
}

bool nivel2::operator==(const nivel2& o) const
{
    return paredes == o.paredes
           && personaje == o.personaje
           && jefeFinal == o.jefeFinal
           && efectos == o.efectos
           && balas == o.balas
        &&esbirros==o.esbirros;
}
















