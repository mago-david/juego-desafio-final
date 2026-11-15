#include "nivel1.h"
#include <stdexcept>
#include "datos.h"
#include <cmath>
const double ANCHO_NIVEL1 = 4000.0;   // largo del escenario en px
const double Y_SUELO      = 550.0;
nivel1::nivel1():personaje(QPointF(100.0,0.0)),anchoMapa(ANCHO_NIVEL1)
{
    construirMapa();
    const QPointF inicio(100.0,Y_SUELO-personaje.getRadio());
    personaje.setPosicion(inicio);
    personaje.setPosicionAnterior(inicio);
}

void nivel1::construirMapa()
{
    plataformas.reserve(16);
    const double s=Y_SUELO;
    //mini funcion para mermar tanta escritura al crear las plataformas
    auto agregar = [this](double x, double y, double ancho, double alto,
                          bool atravesable = false)
    {
        plataformas.emplace_back(QPointF(x, y), ancho, alto, atravesable);
    };

    //los limites del mapa
    agregar(0,s,anchoMapa,50); //suelo
    agregar(0,0,40,s); //pared izquierda
    agregar(anchoMapa-40,0,40,s); //pared derecha
    agregar(0,-50,anchoMapa,50); //techo no se ve

    agregar(400, s - 80,  120, 80);
    agregar(520, s - 160, 120, 160);

    agregar(760,  s - 120, 160, 20, true);
    agregar(980,  s - 220, 160, 20, true);
    agregar(1200, s - 320, 160, 20, true);

    agregar(1500, s - 220, 80, 220);

    agregar(1800, s - 170, 200, 30);

    agregar(2200, s - 150, 300, 20, true);
}
void nivel1::actualizar(double dt) {
    personaje.actualizar(dt);                      // 1. mueve (la entrada ya esta aplicada)
    personaje.resolverPlataformas(plataformas);    // 2. corrige choques con el mapa
}

//acceso
const std::vector<obstaculo>& nivel1::getPlataformas() const { return plataformas; }
const capitanN1& nivel1::getPersonaje() const { return personaje; }
double nivel1::getAnchoMapa() const { return anchoMapa; }

void nivel1::setPlataformas(const std::vector<obstaculo>& p) { plataformas = p; }
void nivel1::setPersonaje(const capitanN1& p) { personaje = p; }

void nivel1::setAnchoMapa(double a) {
    if (a <= 0.0)
        throw std::invalid_argument("El ancho del mapa debe ser mayor que cero");
    anchoMapa = a;
    plataformas.clear();    // suelo, techo y pared derecha dependen del ancho: se reconstruye
    construirMapa();
}

void nivel1::setDireccionPersonaje(int d)
{ personaje.setDireccion(d); }

bool nivel1::saltar()
{ return personaje.saltar(); }

bool nivel1::bajarPlataforma()
{ return personaje.bajarPlataforma(); }

//operadores
nivel1& nivel1::operator=(const nivel1& o) {
    if (this != &o) {
        plataformas = o.plataformas;
        personaje   = o.personaje;
        anchoMapa   = o.anchoMapa;
    }
    return *this;
}

bool nivel1::operator==(const nivel1& o) const {
    return plataformas == o.plataformas
           && personaje == o.personaje
           && std::abs(anchoMapa - o.anchoMapa) <= datos::EPSILON;
}