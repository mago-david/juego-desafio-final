#include "nivel1.h"
#include <stdexcept>
#include "datos.h"
#include <cmath>
#include <algorithm>
using namespace std;

namespace
{
const double ANCHO_NIVEL1 = 6800.0;        // ~60 s de recorrido
const double VEL_MURO     = 95.0;          // px/s
const double Y_SUELO      = 550.0;
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
const int    BALAS_POR_ATAQUE   = 4;     // debe ser <= N_BALAS
const int    LACRIS_POR_ATAQUE  = 3;     // debe ser <= N_LACRIMOGENAS
const double INTERVALO_DISPARO  = 0.55;   // s entre disparos de un mismo ataque
const double ESPERA_ATAQUE      = 3.0;   // s entre el ultimo disparo y el ataque siguiente (tambien el primero)

const double ANCHO_NUBE    = 150.0;
const double ALTO_NUBE     = 100.0;
const double DURACION_NUBE = 3.0;
}

nivel1::nivel1()
    : personaje(QPointF(100.0, 0.0)),
    primeraAltura(0), disparosRestantes(0), tReloj(ESPERA_ATAQUE),
    anchoMapa(ANCHO_NIVEL1), muro(QPointF(0.0, 0.0), 40.0, Y_SUELO, VEL_MURO),
    muroactivo(false)
{
    construirMapa();
    construirProy();
    construirBufos();
    // NUEVO
    const QPointF inicio(100.0, Y_SUELO - personaje.getRadio());
    personaje.setPosicion(inicio);
    personaje.setPosicionAnterior(inicio);
}

//mapa
void nivel1::construirMapa()
{
    plataformas.clear();
    manifestantes.clear();
    const double s = Y_SUELO;

    auto agregar = [this](double x, double y, double ancho, double alto,
                          bool atravesable = false, int sprite = 0)
    {
        plataformas.emplace_back(QPointF(x, y), ancho, alto, atravesable, sprite);
    };

    auto agregarManifestantes = [this, s](double x, double ancho = 160.0, double alto = 90.0)
    {
        manifestantes.emplace_back(QPointF(x, s - alto), ancho, alto, efecto::multitud, 1.0);
    };

    // Límites mapa
    agregar(0, s, anchoMapa, 50, false, 7);          // suelo
    agregar(0, 0, 40, s, false, 0);                  // pared izquierda
    agregar(anchoMapa - 40, 0, 40, s, false, 0);     // pared derecha
    agregar(0, -50, anchoMapa, 50, false, 0);        // techo

    // parte 1
    agregar(280,  s - 105, 250, 105, false, 2);      // bus verde (2.png)
    agregarManifestantes(560, 160, 90);              // manifestantes 1 (8.png)
    agregarManifestantes(730, 160, 90);              // manifestantes 2 (8.png)
    agregar(910,  s - 200, 90,  200, false, 7);      // primer muro ladrillo (7.png)
    agregarManifestantes(1030, 180, 90);             // manifestantes suelo 1
    agregarManifestantes(1230, 180, 90);             // manifestantes suelo 2
    agregarManifestantes(1430, 180, 90);             // manifestantes suelo 3
    agregar(1060, s - 165, 140, 22,  true,  4);      // plataforma media izq (4.png)
    agregar(1210, s - 285, 220, 22,  true,  5);      // plataforma alta con bufo (5.png)
    agregar(1440, s - 165, 140, 22,  true,  4);      // plataforma media der (4.png)
    agregar(1620, s - 235, 90,  235, false, 7);      // segundo muro ladrillo (7.png)
    // parte 2
    agregar(1840, s - 95,  190, 95,  false, 3);      // camión grafiti (3.png)
    agregarManifestantes(2080, 160, 90);             // manifestantes 1
    agregarManifestantes(2250, 160, 90);             // manifestantes 2
    agregarManifestantes(2420, 160, 90);             // manifestantes 3
    agregar(2430, s - 175, 140, 22,  true,  4);      // plataforma corta (4.png)
    agregar(2640, s - 285, 95,  200, false, 7);      // muro colgante (túnel abajo)
    agregarManifestantes(2780, 160, 90);             // manifestantes salida túnel
    agregar(2790, s - 115, 220, 22,  true,  5);      // plataforma larga sobre multitud (5.png)
    // parte 3 final
    agregar(3180, s - 110, 360, 110, false, 6);      // TransMilenio rojo (6.png)
    agregar(3460, s - 220, 150, 22,  true,  4);      // plataforma subida (4.png)
    agregar(3640, s - 240, 90,  240, false, 7);      // muro alto (7.png)
    agregarManifestantes(3780, 220, 90);             // multitud larga
    agregar(4040, s - 95,  190, 95,  false, 1);      // camión refugio (1.png)
    agregarManifestantes(4270, 220, 90);             // multitud larga
    agregar(4280, s - 175, 220, 22,  true,  5);      // plataforma alta (5.png)
    agregar(4680, s - 110, 360, 110, false, 6);      // TransMilenio rojo (6.png)
    agregar(5100, s - 205, 140, 22,  true,  4);      // plataforma aérea (4.png)
    agregarManifestantes(5280, 250, 90);             // última multitud
    agregar(5580, s - 105, 250, 105, false, 2);      // bus verde (2.png)
    agregar(5920, s - 180, 220, 22,  true,  5);      // plataforma final (5.png)
    agregar(6220, s - 95,  190, 95,  false, 1);      // último camión (1.png)
}

void nivel1::actualizar(double dt) {
    if (muroactivo) {
    muro.actualizar(dt);
    }
    if (muro.aplicar(personaje))
        return; // tocó el borde izquierdo, muere al instante

    const double limiteIzq = muro.getDerecha();

    lanzarProyectiles(dt);
    personaje.setEnNube(false);
    for (efecto& n : nubes) {
        if (!n.getactivo())
            continue;
        if (n.getDerecha() < limiteIzq) {
            n.setactivo(false); // salió de pantalla: se apaga
            continue;
        }
        n.actualizar(dt);
        n.aplicar(personaje);
    }
    for (efecto& b : bufos) {
        if (!b.getactivo())
            continue;
        if (b.getDerecha() < limiteIzq) {
            b.setactivo(false); // quedó atrás: se desactiva
            continue;
        }
        b.aplicar(personaje);
    }
    for (efecto& m : manifestantes) {
        if (!m.getactivo())
            continue;
        if (m.getDerecha() < limiteIzq) {
            m.setactivo(false);
            continue;
        }
        m.aplicar(personaje);
    }
    personaje.actualizar(dt);
    personaje.resolverPlataformas(plataformas);
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
void nivel1::construirBufos()
{
    const double s = Y_SUELO;
    const double tam = 45.0;
    bufos.clear();
    bufos.emplace_back(QPointF(1300.0, s - 285.0 - tam), tam, tam, efecto::bufo, 5.0);
    bufos.emplace_back(QPointF(2665.0, s - tam), tam, tam, efecto::bufo, 5.0);
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
    const double limiteIzq = muro.getDerecha();
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
        //eliminamos el proy si sale por el borde izquierdo de la pantalla
        if (b.estaActivo() && b.getPosicion().x() + b.getRadio() < limiteIzq)
            b.setActivo(false);
    }
    for (proyectil& l : lacrimogenas)
    {
        if (!l.estaActivo())
            continue;
        l.actualizar(dt);
        chocarCon(l); // rebota y pierde energia hasta quedarse sin rebotes
        if (l.estaActivo() && personaje.esGolpeado(l))
            l.setActivo(false);
        // Si sale de pantalla sin haber explotado, se apaga sin lanzar nube
        if (l.estaActivo() && l.getPosicion().x() + l.getRadio() < limiteIzq) {
            l.setActivo(false);
            continue;
        }
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

const vector<efecto>& nivel1::getManifestantes() const
{ return manifestantes; }

const vector<efecto>& nivel1::getBufos() const
{ return bufos; }

const muroPerseguidor& nivel1::getMuro() const
{ return muro; }

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
        bufos = o.bufos;
        muro = o.muro;
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
           && nubes == o.nubes
           && bufos == o.bufos
           && muro == o.muro;
}
void nivel1::setMuroActivo(bool activo)
{
    muroactivo = activo;
}