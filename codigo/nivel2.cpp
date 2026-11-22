#include "nivel2.h"
#include "datos.h"

namespace
{
const double GROSOR_PARED = 50.0;                  // las paredes deben ser solidas: mas gruesas que lo que avanza un escudo por paso (7.5 px)
const double LADO_ARENA   = datos::ALTO_ESCENA;    // arena cuadrada, tan alta como la ventana
const double X_ARENA      = (datos::ANCHO_ESCENA - LADO_ARENA) / 2.0;   // centrada en la escena
}
nivel2::nivel2()
    : personaje(QPointF(X_ARENA + LADO_ARENA / 2.0, LADO_ARENA / 2.0))
{
    construirArena();
}

void nivel2::construirArena()
{
    paredes.reserve(4);
    const double g = GROSOR_PARED;
    paredes.emplace_back(QPointF(X_ARENA, 0.0), LADO_ARENA, g);                            // arriba
    paredes.emplace_back(QPointF(X_ARENA, LADO_ARENA - g), LADO_ARENA, g);                 // abajo
    paredes.emplace_back(QPointF(X_ARENA, g), g, LADO_ARENA - 2.0 * g);                    // izquierda
    paredes.emplace_back(QPointF(X_ARENA + LADO_ARENA - g, g), g, LADO_ARENA - 2.0 * g);   // derecha
}

void nivel2::actualizar(double dt)
{
    personaje.actualizar(dt);
    personaje.resolverParedes(paredes);
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

void nivel2::setParedes(const std::vector<obstaculo>& p)
{ paredes = p; }

void nivel2::setPersonaje(const capitanN2& p)
{ personaje = p; }

// operadores
nivel2& nivel2::operator=(const nivel2& o)
{
    if (this != &o) {
        paredes   = o.paredes;
        personaje = o.personaje;
    }
    return *this;
}

bool nivel2::operator==(const nivel2& o) const
{
    return paredes == o.paredes && personaje == o.personaje;
}
















