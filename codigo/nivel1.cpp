#include "nivel1.h"
#include <stdexcept>
#include "datos.h"
#include <cmath>
#include <algorithm>
using namespace std;

namespace
{
const double ANCHO_NIVEL1 = 4000.0;        // << ya existia
const double Y_SUELO      = 550.0;         // << ya existia

// ---- Proyectiles (placeholders: ajustar jugando) ----
const double DX_DISPARO     = 450.0;   // donde nacen respecto al capitan: mas de media pantalla (400), fuera de vista
const double VEL_PROYECTIL  = 300.0;   // px/s hacia la izquierda; igual para balas y lacrimogenas
const double MASA_PROYECTIL = 1.0;     // la base exige > 0; no interviene en la fisica actual
const double RADIO_PROYECTIL = 6.0;    // px; tambien decide si "nace dentro de un obstaculo"
const int    LACRIMOGENA_REBOTES = 3;  // rebotes que sobrevive; se fija midiendo (e^N * vn1 >= UMBRAL_REPOSO)
const double LACRIMOGENA_E       = 0.9;// restitucion: choque inelastico (pierde energia)
const int    N_BALAS         = 10;     // tamano del pool = maximo de balas vivas a la vez
const int    N_LACRIMOGENAS  = 5;      // idem

// ---- Ataques ----
const int    N_ALTURAS          = 10;    // lugares desde donde se puede disparar
const double ALTURA_MIN         = 110.0; // y del lugar mas alto
const double PASO_ALTURA        = 45.0;  // separacion entre lugares (el mas bajo queda en 515)
const int    ALTURAS_POR_ATAQUE = 5;     // cuantos lugares usa cada ataque (los mas cercanos al jugador)
const int    BALAS_POR_ATAQUE   = 5;     // debe ser <= N_BALAS
const int    LACRIS_POR_ATAQUE  = 2;     // debe ser <= N_LACRIMOGENAS
const double INTERVALO_DISPARO  = 0.2;   // s entre disparos de un mismo ataque
const double ESPERA_ATAQUE      = 3.0;   // s entre el ultimo disparo y el ataque siguiente (tambien el primero)

const double ANCHO_NUBE    = 150.0;
const double ALTO_NUBE     = 100.0;
const double DURACION_NUBE = 3.0;
}

nivel1::nivel1()
    : personaje(QPointF(100.0, 0.0)),
    primeraAltura(0), disparosRestantes(0), tReloj(ESPERA_ATAQUE),
    anchoMapa(ANCHO_NIVEL1)
{
    construirMapa();
    construirProy();                                   // NUEVO
    const QPointF inicio(100.0, Y_SUELO - personaje.getRadio());
    personaje.setPosicion(inicio);
    personaje.setPosicionAnterior(inicio);
}

//mapa
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
    lanzarProyectiles(dt);
    for (efecto& e : nubes)
    {
        e.actualizar(dt);
        e.aplicar(personaje);
    }
    personaje.actualizar(dt);                      // 1. mueve (la entrada ya esta aplicada)
    personaje.resolverPlataformas(plataformas);    // 2. corrige choques con el mapa
    actualizarProyectiles(dt);
}

//ataques proyectiles

void nivel1::construirProy()
{
    if(BALAS_POR_ATAQUE>N_BALAS||LACRIS_POR_ATAQUE>N_LACRIMOGENAS)
        throw invalid_argument("Un ataque no puede tener mas disparos q proyectiles");
    balas.reserve(N_BALAS);
    for (int i = 0; i < N_BALAS; ++i)
    {
        // bala: sin gravedad, 0 rebotes, e = 1
        balas.emplace_back(QPointF(), QPointF(), MASA_PROYECTIL, RADIO_PROYECTIL, false, 0, 1.0);
        balas.back().setActivo(false);
    }

    lacrimogenas.reserve(N_LACRIMOGENAS);
    for (int i = 0; i < N_LACRIMOGENAS; ++i)
    {
        lacrimogenas.emplace_back(QPointF(), QPointF(), MASA_PROYECTIL, RADIO_PROYECTIL, true,
                                  LACRIMOGENA_REBOTES, LACRIMOGENA_E);
        lacrimogenas.back().setActivo(false);
    }
    //todos sin velocidad ni posicion poque no sabemos donde se dispararan
    nubes.reserve(N_LACRIMOGENAS);
    for (int i = 0; i < N_LACRIMOGENAS; ++i)
    {
        nubes.emplace_back(QPointF(), ANCHO_NUBE, ALTO_NUBE, efecto::gas, DURACION_NUBE);
        nubes.back().setactivo(false);
    }
}

bool nivel1::lanzarEn(vector<proyectil>& proy, const QPointF& pos,
                      const QPointF& vel, int rebotes)
{
    //revisa que haya proyectiles disponibles para atacar
    for (proyectil& p : proy)
    {
        if (!p.estaActivo())
        {
            p.reiniciar(pos, vel, rebotes);
            return true;
        }
    }
    return false;
}

bool nivel1::lanzarBala(const QPointF& pos, const QPointF& vel)
{ return lanzarEn(balas, pos, vel, 0); }

bool nivel1::lanzarLacrimogena(const QPointF& pos, const QPointF& vel)
{ return lanzarEn(lacrimogenas, pos, vel, LACRIMOGENA_REBOTES); }

bool nivel1::puntoLibre(double x,double y,double r)const
{
    const QPointF c(x, y);
    for (const obstaculo& o : plataformas)
    {
        if (!o.getAtravesable() && o.tocaCirculo(c, r))
            return false;
    }
    return true;
}

void nivel1::chocarCon(proyectil& p) const
{
    for(const obstaculo& o:plataformas)
    {
        if(!p.estaActivo())
            return;
        if(o.getAtravesable())
            continue;
        if(o.colisionar(p,p.getCoefRestitucion()))
            p.registrarRebote();
    }
}

void nivel1:: elegirAlturas()
{
    const double yp=personaje.getPosicion().y();
    //(yp-ALTURA...)/PASO_AL.. es pa saber cuanta distancia hay entre el jugador y primer lugar
    //lround redonde dea con yp=530 ..=9
    //los min y max impiden que se calcule fuera de las zonas de disparo
    const int cercana = max(0, min(N_ALTURAS - 1,
                        int(lround((yp - ALTURA_MIN) / PASO_ALTURA))));
    //es cercana-"2" con el min evitamos que se salga por abajo
    //y el max para que no se pase de la posicion 0
    primeraAltura = max(0, min(cercana - ALTURAS_POR_ATAQUE / 2,
                    N_ALTURAS - ALTURAS_POR_ATAQUE));
}

void nivel1::dispararSiguiente()
{
    const int total=BALAS_POR_ATAQUE+LACRIS_POR_ATAQUE;
    const int hechos=total-disparosRestantes;
    const bool bala=disparosRestantes>LACRIS_POR_ATAQUE;
    --disparosRestantes;

    const double x=personaje.getPosicion().x()+DX_DISPARO;
    //hechos%ALTURAS... es que posicion toca ahora
    //primera...+... (0 a 9)
    //ALTURA_MIN+(...)*PASO_... convierte ese numero en pixeles
    const double y = ALTURA_MIN + (primeraAltura + hechos % ALTURAS_POR_ATAQUE) * PASO_ALTURA;

    if (x >= ANCHO_NIVEL1 || !puntoLibre(x, y, RADIO_PROYECTIL))
        return; //nace en punto muerto pailas

    const QPointF vel(-VEL_PROYECTIL, 0.0);
    if (bala) lanzarBala(QPointF(x, y), vel);
    else      lanzarLacrimogena(QPointF(x, y), vel);
}

void nivel1::lanzarProyectiles(double dt)
{
    tReloj-=dt;
    if(tReloj>0.0)
        return;

    if(disparosRestantes==0)
    {
        disparosRestantes=BALAS_POR_ATAQUE+LACRIS_POR_ATAQUE;
        elegirAlturas();
    }
    dispararSiguiente();

    tReloj+=(disparosRestantes==0)?ESPERA_ATAQUE:INTERVALO_DISPARO;
}

void nivel1::actualizarProyectiles(double dt)
{
    for(proyectil&b:balas)
    {
        if(!b.estaActivo())
            continue;
        b.actualizar(dt);
        chocarCon(b);

        if(b.estaActivo()&&personaje.esGolpeado(b))
        {
            personaje.recibirDanio();
            b.setActivo(false);
        }
        //eliminamos el proy si esta muy lejos del jugador
        if (b.estaActivo() && b.getPosicion().x() < personaje.getPosicion().x() - DX_DISPARO)
            b.setActivo(false);
    }
    for (proyectil& l : lacrimogenas)
    {
        if (!l.estaActivo())
            continue;
        l.actualizar(dt);
        chocarCon(l);                                  // rebota y pierde energia hasta quedarse sin rebotes

        if (l.estaActivo() && personaje.esGolpeado(l))
            l.setActivo(false);

        if (!l.estaActivo())
        {
            lanzarNube(l.getPosicion());
        }

    }
}

void nivel1::lanzarNube(const QPointF& centro)
{
    for (efecto& e : nubes)
    {
        if (!e.getactivo())
        {
            e.activar(QPointF(centro.x()-e.getAncho()/2.0, centro.y()-e.getAlto()/2.0), DURACION_NUBE);
            return;
        }
    }
}
//acceso
//obstaculos y personaje
const vector<obstaculo>& nivel1::getPlataformas() const
{ return plataformas; }

const capitanN1& nivel1::getPersonaje() const
{ return personaje; }

double nivel1::getAnchoMapa() const
{ return anchoMapa; }

void nivel1::setPlataformas(const std::vector<obstaculo>& p)
{ plataformas = p; }

void nivel1::setPersonaje(const capitanN1& p)
{ personaje = p; }

void nivel1::setAnchoMapa(double a) {
    if (a <= 0.0)
        throw invalid_argument("El ancho del mapa debe ser mayor que cero");
    anchoMapa = a;
    plataformas.clear();
    construirMapa();
}

//proyectiles

const vector<proyectil>& nivel1::getBalas() const
{ return balas; }

const vector<proyectil>& nivel1::getLacrimogenas() const
{ return lacrimogenas; }

void nivel1::setBalas(const vector<proyectil>& b)
{
    if (b.size() != balas.size())
        throw invalid_argument("El pool de balas conserva su tamano fijo");
    balas = b;
}

void nivel1::setLacrimogenas(const std::vector<proyectil>& l) {
    if (l.size() != lacrimogenas.size())
        throw std::invalid_argument("El pool de lacrimogenas conserva su tamano fijo");
    lacrimogenas = l;
}

int    nivel1::getPrimeraAltura() const
{ return primeraAltura; }

int    nivel1::getDisparosRestantes() const
{ return disparosRestantes; }

double nivel1::getTReloj() const
{ return tReloj; }

void nivel1::setPrimeraAltura(int i) {
    if (i < 0 || i > N_ALTURAS - ALTURAS_POR_ATAQUE)
        throw out_of_range("La ventana de alturas se sale de las alturas disponibles");
    primeraAltura = i;
}

void nivel1::setDisparosRestantes(int n) {
    if (n < 0)
        throw invalid_argument("Los disparos restantes no pueden ser negativos");
    disparosRestantes = n;
}

void nivel1::setTReloj(double t)
{ tReloj = t;}

//atajos
void nivel1::setDireccionPersonaje(int d)
{ personaje.setDireccion(d); }

bool nivel1::saltar()
{ return personaje.saltar(); }

bool nivel1::bajarPlataforma()
{ return personaje.bajarPlataforma(); }


const vector<efecto>& nivel1::getNubes() const
{ return nubes; }

void nivel1::setNubes(const vector<efecto>& n)
{
    if (n.size() != nubes.size())
        throw invalid_argument("El pool de nubes conserva su tamano fijo");
    nubes = n;
}


//operadores
nivel1& nivel1::operator=(const nivel1& o) {
    if (this != &o) {
        plataformas = o.plataformas;
        personaje   = o.personaje;
        anchoMapa   = o.anchoMapa;
        balas = o.balas;
        lacrimogenas = o.lacrimogenas;
        primeraAltura = o.primeraAltura;
        disparosRestantes = o.disparosRestantes;
        tReloj = o.tReloj;
        nubes = o.nubes;
    }
    return *this;
}

bool nivel1::operator==(const nivel1& o) const
{
    return plataformas == o.plataformas
            && personaje == o.personaje
            && balas == o.balas
            && lacrimogenas == o.lacrimogenas
            && primeraAltura == o.primeraAltura
            && disparosRestantes == o.disparosRestantes
            && abs(tReloj - o.tReloj) <= datos::EPSILON
            && abs(anchoMapa - o.anchoMapa) <= datos::EPSILON
            && nubes == o.nubes;
}



