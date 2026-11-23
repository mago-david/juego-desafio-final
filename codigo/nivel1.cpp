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
const int    LACRIMOGENA_REBOTES = 3;  // rebotes que sobrevive
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
const double INTERVALO_DISPARO  = 0.55;  // s entre disparos de un mismo ataque
const double ESPERA_ATAQUE      = 3.0;   // s entre el ultimo disparo y el ataque siguiente (tambien el primero)

const double ANCHO_NUBE    = 150.0;
const double ALTO_NUBE     = 100.0;
const double DURACION_NUBE = 3.0;
}

nivel1::nivel1(int vidas_)
    : personaje(QPointF(100.0, 0.0), vidas_),
    primeraAltura(0), disparosRestantes(0), tReloj(ESPERA_ATAQUE),
    anchoMapa(ANCHO_NIVEL1), muro(QPointF(0.0, 0.0), 40.0, Y_SUELO, VEL_MURO)
{
    construirMapa();
    construirProy();
    construirBufos();
    const QPointF inicio(100.0, Y_SUELO - personaje.getRadio());
    personaje.setPosicion(inicio);
    personaje.setPosicionAnterior(inicio);
}

nivel1::nivel1() : nivel1(datos::VIDAS_INICIALES) {}

nivel1::~nivel1() {}

//mapa
void nivel1::construirMapa()
{
    plataformas.clear();
    manifestantes.clear();
    const double s = Y_SUELO;

    // Limites del mapa
    plataformas.emplace_back(QPointF(0.0, s),          anchoMapa, 60.0,     false, 7); // suelo
    plataformas.emplace_back(QPointF(-30.0, 0.0),      30.0,      s + 60.0, false, 0); // pared izquierda
    plataformas.emplace_back(QPointF(anchoMapa, 0.0),  30.0,      s + 60.0, false, 0); // pared derecha

    // parte 1
    plataformas.emplace_back(QPointF(150.0, s - 95.0),  180.0, 95.0,  false, 1); // bloque gris inicial
    plataformas.emplace_back(QPointF(230.0, s - 220.0), 140.0, 22.0,  true,  4); // plataforma azul 1
    manifestantes.emplace_back(QPointF(330.0, s - 90.0), 320.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(430.0, s - 320.0), 190.0, 22.0,  true,  5); // plataforma media-alta
    plataformas.emplace_back(QPointF(650.0, s - 230.0), 60.0,  230.0, false, 7); // 1er muro vertical

    // parte 2
    manifestantes.emplace_back(QPointF(710.0, s - 90.0), 640.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(740.0,  s - 150.0), 130.0, 22.0,  true,  4); // ruta baja 1
    plataformas.emplace_back(QPointF(760.0,  s - 410.0), 140.0, 22.0,  true,  4); // ruta aerea alta
    plataformas.emplace_back(QPointF(930.0,  s - 270.0), 200.0, 22.0,  true,  5); // central (1er bufo)
    plataformas.emplace_back(QPointF(1080.0, s - 150.0), 130.0, 22.0,  true,  4); // ruta baja 2
    plataformas.emplace_back(QPointF(1160.0, s - 360.0), 200.0, 22.0,  true,  5); // alta hacia 2do muro
    plataformas.emplace_back(QPointF(1350.0, s - 230.0), 60.0,  230.0, false, 7); // 2do muro vertical

    // parte 3
    plataformas.emplace_back(QPointF(1500.0, s - 290.0), 190.0, 22.0,  true,  5);
    plataformas.emplace_back(QPointF(1620.0, s - 110.0), 240.0, 110.0, false, 2); // bus verde
    plataformas.emplace_back(QPointF(1800.0, s - 440.0), 150.0, 22.0,  true,  4); // techo alto
    manifestantes.emplace_back(QPointF(1860.0, s - 90.0), 250.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(1920.0, s - 210.0), 130.0, 22.0,  true,  4); // escalon al muro torre

    // parte 4
    plataformas.emplace_back(QPointF(2110.0, s - 310.0), 75.0,  310.0, false, 7); // 3er muro torre
    plataformas.emplace_back(QPointF(2180.0, s - 410.0), 210.0, 22.0,  true,  5); // pasarela alta
    plataformas.emplace_back(QPointF(2260.0, s - 160.0), 140.0, 22.0,  true,  4);
    manifestantes.emplace_back(QPointF(2280.0, s - 90.0), 240.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(2540.0, s - 105.0), 200.0, 105.0, false, 1); // vehiculo gris
    plataformas.emplace_back(QPointF(2640.0, s - 260.0), 160.0, 22.0,  true,  4); // salto medio
    plataformas.emplace_back(QPointF(2820.0, s - 390.0), 190.0, 22.0,  true,  5); // plataforma alta

    // parte 5
    plataformas.emplace_back(QPointF(3030.0, s - 210.0), 55.0,  210.0, false, 7); // muro medio
    manifestantes.emplace_back(QPointF(3085.0, s - 90.0), 340.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(3120.0, s - 310.0), 150.0, 22.0,  true,  4); // puente sobre manifestantes
    plataformas.emplace_back(QPointF(3310.0, s - 450.0), 170.0, 22.0,  true,  5); // cima alta (2do bufo arriba)
    plataformas.emplace_back(QPointF(3425.0, s - 115.0), 250.0, 115.0, false, 2); // bus verde 2
    plataformas.emplace_back(QPointF(3690.0, s - 250.0), 140.0, 22.0,  true,  4); // salto salida convoy
    plataformas.emplace_back(QPointF(3830.0, s - 340.0), 65.0,  340.0, false, 7); // 4to muro torre

    // parte 6
    plataformas.emplace_back(QPointF(3930.0, s - 170.0), 130.0, 22.0,  true,  4); // plataforma baja
    plataformas.emplace_back(QPointF(3960.0, s - 460.0), 180.0, 22.0,  true,  5); // terraza muy alta
    manifestantes.emplace_back(QPointF(4060.0, s - 90.0), 540.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(4140.0, s - 290.0), 170.0, 22.0,  true,  4); // isla central
    plataformas.emplace_back(QPointF(4260.0, s - 430.0), 190.0, 22.0,  true,  5); // isla aerea alta
    plataformas.emplace_back(QPointF(4390.0, s - 180.0), 150.0, 22.0,  true,  4); // escape bajo
    plataformas.emplace_back(QPointF(4600.0, s - 100.0), 210.0, 100.0, false, 1); // bloque gris aterrizaje

    // parte 7
    plataformas.emplace_back(QPointF(4870.0, s - 230.0), 140.0, 22.0,  true,  4);
    plataformas.emplace_back(QPointF(5030.0, s - 370.0), 160.0, 22.0,  true,  5);
    manifestantes.emplace_back(QPointF(5050.0, s - 90.0), 420.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(5230.0, s - 470.0), 180.0, 22.0,  true,  4); // punto mas alto del nivel (3er bufo)
    plataformas.emplace_back(QPointF(5260.0, s - 190.0), 140.0, 22.0,  true,  4); // nivel medio alternativo
    plataformas.emplace_back(QPointF(5470.0, s - 260.0), 60.0,  260.0, false, 7); // 5to muro vertical
    plataformas.emplace_back(QPointF(5570.0, s - 115.0), 240.0, 115.0, false, 2); // bus verde 3

    // parte 8
    plataformas.emplace_back(QPointF(5840.0, s - 270.0), 150.0, 22.0,  true,  4); // subida 1
    manifestantes.emplace_back(QPointF(5920.0, s - 90.0), 480.0, 90.0, efecto::danio, 0.0);
    plataformas.emplace_back(QPointF(6040.0, s - 400.0), 190.0, 22.0,  true,  5); // cima final
    plataformas.emplace_back(QPointF(6120.0, s - 170.0), 140.0, 22.0,  true,  4); // plataforma baja (4to bufo)
    plataformas.emplace_back(QPointF(6300.0, s - 300.0), 160.0, 22.0,  true,  4); // ultimo salto aereo
    plataformas.emplace_back(QPointF(6420.0, s - 220.0), 65.0,  220.0, false, 7); // ultimo muro antes de meta
    plataformas.emplace_back(QPointF(6520.0, s - 100.0), 180.0, 100.0, false, 1); // vehiculo final de llegada
}

void nivel1::construirBufos()
{
    bufos.clear();
    const double s = Y_SUELO;
    bufos.emplace_back(QPointF(1015.0, s - 315.0), 30.0, 30.0, efecto::bufo, 5.0); // parte 2 en plataforma central
    bufos.emplace_back(QPointF(3380.0, s - 495.0), 30.0, 30.0, efecto::bufo, 5.0); // parte 5 en cima alta
    bufos.emplace_back(QPointF(5305.0, s - 515.0), 30.0, 30.0, efecto::bufo, 5.0); // parte 7 en cumbre maxima
    bufos.emplace_back(QPointF(6175.0, s - 215.0), 30.0, 30.0, efecto::bufo, 5.0); // parte 8 impulso final
}

void nivel1::construirProy()
{
    if (BALAS_POR_ATAQUE > N_BALAS || LACRIS_POR_ATAQUE > N_LACRIMOGENAS)
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
    //nubes sin posicion: no sabemos donde se dispararan
    nubes.reserve(N_LACRIMOGENAS);
    for (int i = 0; i < N_LACRIMOGENAS; ++i)
    {
        nubes.emplace_back(QPointF(), ANCHO_NUBE, ALTO_NUBE, efecto::gas, DURACION_NUBE);
        nubes.back().setactivo(false);
    }
}

void nivel1::actualizar(double dt)
{
    muro.actualizar(dt);              // no hace nada mientras este inactivo
    if (muro.aplicar(personaje))
        return;                       // toco el borde izquierdo, muere al instante

    const double limiteIzq = muro.getDerecha();

    lanzarProyectiles(dt);
    personaje.setEnNube(false);
    for (efecto& n : nubes) {
        if (!n.getactivo())
            continue;
        if (n.getDerecha() < limiteIzq) {
            n.setactivo(false);       // salio de pantalla: se apaga
            continue;
        }
        n.actualizar(dt);
        n.aplicar(personaje);
    }
    for (efecto& b : bufos) {
        if (!b.getactivo())
            continue;
        if (b.getDerecha() < limiteIzq) {
            b.setactivo(false);       // quedo atras: se desactiva
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

bool nivel1::puntoLibre(double x, double y, double r) const
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
    for (const obstaculo& o : plataformas)
    {
        if (!p.estaActivo())
            return;
        if (o.getAtravesable())
            continue;
        if (o.colisionar(p, p.getCoefRestitucion()))
            p.registrarRebote();
    }
}

void nivel1::elegirAlturas()
{
    const double yp = personaje.getPosicion().y();
    //(yp-ALTURA_MIN)/PASO_ALTURA = que lugar queda mas cerca del jugador
    //min y max impiden que se calcule fuera de las zonas de disparo
    const int cercana = max(0, min(N_ALTURAS - 1,
                                   int(lround((yp - ALTURA_MIN) / PASO_ALTURA))));
    //se centra la ventana de lugares en la cercana sin salirse de 0..N_ALTURAS
    primeraAltura = max(0, min(cercana - ALTURAS_POR_ATAQUE / 2,
                               N_ALTURAS - ALTURAS_POR_ATAQUE));
}

void nivel1::dispararSiguiente()
{
    const int total = BALAS_POR_ATAQUE + LACRIS_POR_ATAQUE;
    const int hechos = total - disparosRestantes;
    const bool bala = disparosRestantes > LACRIS_POR_ATAQUE;
    --disparosRestantes;

    const double x = personaje.getPosicion().x() + DX_DISPARO;
    //hechos % ALTURAS_POR_ATAQUE = que lugar de la ventana toca ahora
    const double y = ALTURA_MIN + (primeraAltura + hechos % ALTURAS_POR_ATAQUE) * PASO_ALTURA;

    if (x >= anchoMapa || !puntoLibre(x, y, RADIO_PROYECTIL))
        return; //nace en punto muerto

    const QPointF vel(-VEL_PROYECTIL, 0.0);
    if (bala) lanzarBala(QPointF(x, y), vel);
    else      lanzarLacrimogena(QPointF(x, y), vel);
}

void nivel1::lanzarProyectiles(double dt)
{
    tReloj -= dt;
    if (tReloj > 0.0)
        return;

    if (disparosRestantes == 0)
    {
        disparosRestantes = BALAS_POR_ATAQUE + LACRIS_POR_ATAQUE;
        elegirAlturas();
    }
    dispararSiguiente();

    tReloj += (disparosRestantes == 0) ? ESPERA_ATAQUE : INTERVALO_DISPARO;
}

void nivel1::actualizarProyectiles(double dt)
{
    const double limiteIzq = muro.getDerecha();
    for (proyectil& b : balas)
    {
        if (!b.estaActivo())
            continue;
        b.actualizar(dt);
        chocarCon(b);
        if (b.estaActivo() && personaje.esGolpeado(b))
        {
            personaje.recibirDanio();
            b.setActivo(false);
        }
        //se elimina si sale por el borde izquierdo de la pantalla
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
        // si sale de pantalla sin haber explotado, se apaga sin lanzar nube
        if (l.estaActivo() && l.getPosicion().x() + l.getRadio() < limiteIzq) {
            l.setActivo(false);
            continue;
        }
        if (!l.estaActivo())
            lanzarNube(l.getPosicion());
    }
}

void nivel1::lanzarNube(const QPointF& centro)
{
    for (efecto& e : nubes)
    {
        if (!e.getactivo())
        {
            e.activar(QPointF(centro.x() - e.getAncho() / 2.0, centro.y() - e.getAlto() / 2.0), DURACION_NUBE);
            return;
        }
    }
}

//acceso
const vector<obstaculo>& nivel1::getPlataformas() const
{ return plataformas; }

const capitanN1& nivel1::getPersonaje() const
{ return personaje; }

const vector<efecto>& nivel1::getManifestantes() const
{ return manifestantes; }

const vector<efecto>& nivel1::getBufos() const
{ return bufos; }

const vector<proyectil>& nivel1::getBalas() const
{ return balas; }

const vector<proyectil>& nivel1::getLacrimogenas() const
{ return lacrimogenas; }

const vector<efecto>& nivel1::getNubes() const
{ return nubes; }

int nivel1::getPrimeraAltura() const
{ return primeraAltura; }

int nivel1::getDisparosRestantes() const
{ return disparosRestantes; }

double nivel1::getTReloj() const
{ return tReloj; }

double nivel1::getAnchoMapa() const
{ return anchoMapa; }

const muroPerseguidor& nivel1::getMuro() const
{ return muro; }

void nivel1::setPlataformas(const vector<obstaculo>& p)
{ plataformas = p; }

void nivel1::setPersonaje(const capitanN1& p)
{ personaje = p; }

void nivel1::setManifestantes(const vector<efecto>& m)
{
    if (m.size() != manifestantes.size())
        throw invalid_argument("La lista de manifestantes conserva su tamano fijo");
    manifestantes = m;
}

void nivel1::setBufos(const vector<efecto>& b)
{
    if (b.size() != bufos.size())
        throw invalid_argument("La lista de bufos conserva su tamano fijo");
    bufos = b;
}

void nivel1::setBalas(const vector<proyectil>& b)
{
    if (b.size() != balas.size())
        throw invalid_argument("El pool de balas conserva su tamano fijo");
    balas = b;
}

void nivel1::setLacrimogenas(const vector<proyectil>& l)
{
    if (l.size() != lacrimogenas.size())
        throw invalid_argument("El pool de lacrimogenas conserva su tamano fijo");
    lacrimogenas = l;
}

void nivel1::setNubes(const vector<efecto>& n)
{
    if (n.size() != nubes.size())
        throw invalid_argument("El pool de nubes conserva su tamano fijo");
    nubes = n;
}

void nivel1::setPrimeraAltura(int i)
{
    if (i < 0 || i > N_ALTURAS - ALTURAS_POR_ATAQUE)
        throw out_of_range("La ventana de alturas se sale de las alturas disponibles");
    primeraAltura = i;
}

void nivel1::setDisparosRestantes(int n)
{
    if (n < 0)
        throw invalid_argument("Los disparos restantes no pueden ser negativos");
    disparosRestantes = n;
}

void nivel1::setTReloj(double t)
{ tReloj = t; }

void nivel1::setAnchoMapa(double a)
{
    if (a <= 0.0)
        throw invalid_argument("El ancho del mapa debe ser mayor que cero");
    anchoMapa = a;
    construirMapa();
}

void nivel1::setMuro(const muroPerseguidor& m)
{ muro = m; }

//atajos para la vista
void nivel1::setMuroActivo(bool activo)
{ muro.setactivo(activo); }

void nivel1::setDireccionPersonaje(int d)
{ personaje.setDireccion(d); }

bool nivel1::saltar()
{ return personaje.saltar(); }

bool nivel1::bajarPlataforma()
{ return personaje.bajarPlataforma(); }

//operadores
nivel1& nivel1::operator=(const nivel1& o)
{
    if (this != &o) {
        plataformas       = o.plataformas;
        personaje         = o.personaje;
        manifestantes     = o.manifestantes;
        bufos             = o.bufos;
        balas             = o.balas;
        lacrimogenas      = o.lacrimogenas;
        nubes             = o.nubes;
        primeraAltura     = o.primeraAltura;
        disparosRestantes = o.disparosRestantes;
        tReloj            = o.tReloj;
        anchoMapa         = o.anchoMapa;
        muro              = o.muro;
    }
    return *this;
}

bool nivel1::operator==(const nivel1& o) const
{
    return plataformas == o.plataformas
           && personaje == o.personaje
           && manifestantes == o.manifestantes
           && bufos == o.bufos
           && balas == o.balas
           && lacrimogenas == o.lacrimogenas
           && nubes == o.nubes
           && primeraAltura == o.primeraAltura
           && disparosRestantes == o.disparosRestantes
           && abs(tReloj - o.tReloj) <= datos::EPSILON
           && abs(anchoMapa - o.anchoMapa) <= datos::EPSILON
           && muro == o.muro;
}