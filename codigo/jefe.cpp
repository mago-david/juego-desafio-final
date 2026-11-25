#include "jefe.h"
#include "datos.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <stdexcept>
#include <numeric>
using namespace std;

//generador unico del proyecto (declarado en datos.h): sin qt y se crea una sola vez
std::mt19937 datos::generador(static_cast<unsigned>(
    std::chrono::steady_clock::now().time_since_epoch().count()));

namespace
{
    std::mt19937& generador = datos::generador;
    const QPointF CENTRO(datos::CENTRO_X, datos::CENTRO_Y);

    const int NUM_MODOS = 3;
    const int NUM_ORIENTACIONES = 2;
    //tiempos
    const double T_RECARGA = 2.0;
    const double T_AVISO = 1.0;
    const double T_DISPARO = 1.5;
    const double T_GAS = 6.0;
    //chorros
    const double GROSOR_CHORRO = 35.0;
    const double FUERZA_CHORRO = 160.0;
    //lados para el cuadrante
    const double MITAD_ARENA = (datos::LADO_ARENA - 2.0 * datos::GROSOR_PARED) / 2.0;
    //balas
    const int    NUM_BALAS = 3;
    const double INTERVALO_BALA = 0.2;
    const double VEL_BALA = 380.0;
    const double SEPARACION_FLANCO = 90.0;
    const double LADO_SENAL = 30.0;
    const double MARGEN_BALA = 15.0;

    //datos pa las bombas/lluvias
    const double CERCA = MITAD_ARENA / 4.0;
    const double LEJOS = 3.0 * MITAD_ARENA / 4.0;
    //posibles ubicaciones de las nubes
    const std::array<QPointF, 14> POS_LLUVIA = {{
                                {-LEJOS, -LEJOS},{LEJOS, -LEJOS},{-LEJOS, -CERCA}, {-CERCA, -CERCA},
                                {CERCA, -CERCA}, {LEJOS, -CERCA}, {-LEJOS,  CERCA}, {-CERCA,  CERCA},
                                {CERCA,  CERCA}, {LEJOS,  CERCA},{-LEJOS,  LEJOS}, {-CERCA,  LEJOS},
                                {CERCA,  LEJOS}, {LEJOS,  LEJOS} }};
    const double LADO_NUBE = 70.0;

    //ataque esquinas
    const int    NUM_ESQUINAS = 4;
    const double INTERVALO_ESQUINA = 0.8;
    const double VEL_ESQUINA = 245.0;
    const double ESPERA_ESQUINA = 2.0;

    //cuerpo del jefe: el sprite entero es solido y recibe dano
    const obstaculo CUERPO(QPointF(datos::X_JEFE, datos::Y_JEFE), datos::ANCHO_JEFE, datos::ALTO_JEFE);

    //fase dificil
    const int    LIMITE_ESBIRROS = 8;
    const int    VIDA_ESBIRRO = 30;
    const double INTERVALO_CURA = 0.3;

    //aprendizaje
    const double ALFA = 0.3;
    const double H_OBS= 1.2;
    const double VEL_MIN_PRED = 20.0;
}
jefe::jefe() : jefe(1, false)
{}

jefe::jefe(int vidaMax_, bool faseEsbirros_)
    : vida(vidaMax_), vidaMax(vidaMax_),
    faseActual(fase::recarga), T_fase(T_RECARGA),
    ataqueActual(ataque::chorros), modoAtaque(modo::horizontal),
    balasLanzadas(0), objetivo(CENTRO), bombas{},
    T_esquina(ESPERA_ESQUINA), esquinaActual(cuadrante::noroeste),
    faseEsbirros(faseEsbirros_), curaRestante(0), esbirrosLanzados(0), k(0.0),
    posRef(CENTRO), velRef(0.0, 0.0), T_obs(H_OBS)
{
    if (vidaMax_ <= 0)
        throw invalid_argument("La vida maxima del jefe debe ser mayor que cero");
}

jefe::cuadrante jefe::percibir(const QPointF& poscapi)const
{
    const bool este = poscapi.x() >= CENTRO.x();
    const bool sur  = poscapi.y() >= CENTRO.y();
    if (sur)
        return este ? cuadrante::sureste : cuadrante::suroeste;

    return este ? cuadrante::noreste : cuadrante::noroeste;
}

jefe::~jefe() {}

//acciones
void jefe::razonar()
{
    const ataque anterior = ataqueActual;
    auto sortear = [](int n) { return uniform_int_distribution<int>(0, n - 1)(generador); };
    ataqueActual = static_cast<ataque>(sortear(datos::NUM_ATAQUES));
    if(ataqueActual==ataque::lluvia&& anterior==ataque::lluvia)
        ataqueActual = static_cast<ataque>(sortear(datos::NUM_ATAQUES - 1)); //chorro es 0 y balas 1

    if (ataqueActual == ataque::chorros)
        modoAtaque = static_cast<modo>(sortear(NUM_MODOS));
    else if(ataqueActual==ataque::balas)
        modoAtaque=static_cast<modo>(sortear(NUM_ORIENTACIONES));
    else
    {
        //nubes en posiciones alazar ya definidas sin repetir
        array<int,POS_LLUVIA.size()>orden;
        iota(orden.begin(),orden.end(),0);
        shuffle(orden.begin(),orden.end(),generador);
        copy_n(orden.begin(),datos::NUM_BOMBAS,bombas.begin());
    }
}

void jefe::actuar(std::vector<efecto>& efectos, std::vector<proyectil>& balas)
{
    switch (ataqueActual)
    {
    case ataque::chorros:
        atacarConChorros(efectos);
        break;
    case ataque::balas:
        atacarConBalas(efectos, balas);
        break;
    case ataque::lluvia:
        atacarConLluvia(efectos);
        break;                            // despues
    }
}

void jefe::aprender(double dt,const QPointF& poscapi,const QPointF& vel)
{
    T_obs -= dt;
    if (T_obs > 0.0)
        return;
    T_obs = H_OBS;

    // comprobar: se movio en la direccion que velRef decia?
    const double v2 = velRef.x() * velRef.x() + velRef.y() * velRef.y();
    if (v2 > VEL_MIN_PRED * VEL_MIN_PRED)           // si estaba quieto no hay nada que comprobar
    {
        const QPointF real = poscapi - posRef;
        const double razon = (real.x() * velRef.x() + real.y() * velRef.y()) / (H_OBS * v2);
        k += ALFA * (max(0.0, min(1.0, razon)) - k);
    }
    // nueva foto para la siguiente comprobacion
    posRef = poscapi;
    velRef = vel;
}

void jefe::actualizar(double dt, const QPointF& posJugador,const QPointF& vel, std::vector<efecto>& efectos,
                      std::vector<proyectil>& balas,vector<esbirro>& esbirros)
{
    aprender(dt,posJugador,vel);
    atacarDesdeEsquina(dt, posJugador,vel, balas);
    //dificil
    if(curaRestante>0)
    {
        curar(dt,esbirros);
        return;
    }
    if(faseEsbirros && esbirrosLanzados==0 && vida<=vidaMax/2)
    {
        iniciarCura(esbirros);
        return;
    }

    T_fase -= dt;
    const bool cambio = (T_fase <= 0.0);
    //recarga->aviso->disparo->recarga->...
    if (cambio)
    {
        switch (faseActual)
        {
        case fase::recarga:
            objetivo = posJugador;
            razonar();
            faseActual = fase::aviso;
            T_fase = T_AVISO;
            break;
        case fase::aviso:
            faseActual = fase::disparo;
            T_fase = T_DISPARO;
            break;
        case fase::disparo:
            faseActual = fase::recarga; T_fase = T_RECARGA;
            break;
        }
        balasLanzadas = 0;
    }
    // chorros actuan solo al cambiar de fase; las balas salen escalonadas en cada frame
    if (cambio || ataqueActual == ataque::balas)
        actuar(efectos, balas);
    return;                    // hubo cambio de fase
}

void jefe::construirChorros(std::vector<efecto>& reserva, double x0, double y0,
                            efecto::tipoefecto tipo, double duracion) const
{
    //una forma de resumir las lineas una mini funcion
    auto franja = [&](double x, double y, double ancho, double alto)
    {
        for (efecto& e : reserva)
        {
            if (e.getactivo())
                continue;
            e.settipo(tipo);
            e.setAncho(ancho);
            e.setAlto(alto);
            e.setfuerzarebote(FUERZA_CHORRO);
            e.activar(QPointF(x, y), duracion);
            return;
        }
    };

    const double s = GROSOR_CHORRO;
    const double hueco = (MITAD_ARENA - 2.0 * s) / 3.0;
    // esquina superior izquierda del interior de la arena y su lado completo
    const double ax = CENTRO.x() - MITAD_ARENA;
    const double ay = CENTRO.y() - MITAD_ARENA;
    const double largo = 2.0 * MITAD_ARENA;   // de pared a pared

    switch (modoAtaque)
    {
    case modo::vertical:      // columna del cuadrante, de la pared norte a la sur
        franja(x0 + hueco, ay, s, largo);
        franja(x0 + 2.0 * hueco + s, ay, s, largo);
        break;
    case modo::horizontal:    // fila del cuadrante, de la pared oeste a la este
        franja(ax, y0 + hueco, largo, s);
        franja(ax, y0 + 2.0 * hueco + s, largo, s);
        break;
    case modo::cruz:          // se cruzan en el centro del cuadrante
        franja(x0 + (MITAD_ARENA - s) / 2.0, ay, s, largo);
        franja(ax, y0 + (MITAD_ARENA - s) / 2.0, largo, s);
        break;
    }
}
//ataques
void jefe::atacarConChorros(std::vector<efecto>& efectos) const
{
    // esquina superior izquierda del cuadrante objetivo
    const cuadrante zonaActual=percibir(objetivo);
    double x0 = CENTRO.x(), y0 = CENTRO.y();
    switch (zonaActual)
    {
    case cuadrante::noroeste:
        x0 -= MITAD_ARENA;
        y0 -= MITAD_ARENA;
        break;
    case cuadrante::noreste:
        y0 -= MITAD_ARENA;
        break;
    case cuadrante::suroeste:
        x0 -= MITAD_ARENA;
        break;
    case cuadrante::sureste:
        break;
    }
    // las señales duran T_AVISO: expiran solas cuando empieza el disparo
    if (faseActual == fase::aviso)
        construirChorros(efectos, x0, y0, efecto::senal, T_AVISO);
    else if (faseActual == fase::disparo)
        construirChorros(efectos, x0, y0, efecto::danio, T_DISPARO);
}

//donde nacen balas
QPointF jefe::puntoBala(int i)const
{
    //si es la primera vala derecho a tu ubicacion
    //si es 2 a SEPARACIO... de ti a la derecha y la tercera misma distacia a la izquierda
    const double desfase=(i==0)?0.0:(i==1?SEPARACION_FLANCO:-SEPARACION_FLANCO);

    const double lim = MITAD_ARENA - MARGEN_BALA;
    const bool vertical = (modoAtaque == modo::vertical);
    const cuadrante zonaActual=percibir(objetivo);
    const bool norte = (zonaActual == cuadrante::noroeste || zonaActual == cuadrante::noreste);
    const bool oeste = (zonaActual == cuadrante::noroeste || zonaActual == cuadrante::suroeste);

    const double c = vertical ? objetivo.x() - CENTRO.x()
                              : objetivo.y() - CENTRO.y();
    //posicion + desface
    const double carril = std::max(-lim, std::min(lim, c + desfase));
    //arma la bala pegada a la pared
    if (vertical)
    {
        const double x = CENTRO.x() + carril;
        if (norte && x > datos::X_JEFE - MARGEN_BALA && x < datos::X_JEFE + datos::ANCHO_JEFE + MARGEN_BALA)
        {
            return QPointF(x, datos::Y_JEFE + datos::ALTO_JEFE + MARGEN_BALA);
        }
        return QPointF(x, CENTRO.y() + (norte ? -lim : lim));
    }
    return QPointF(CENTRO.x() + (oeste ? -lim : lim), CENTRO.y() + carril);
}

void jefe::atacarConBalas(vector<efecto>& efectos,vector<proyectil>& balas)
{
    if(faseActual==fase::recarga)
        return;
    //tiempo de la fase
    const double total=(faseActual == fase::aviso) ? T_AVISO : T_DISPARO;
    //cuantas deberian haber salido ya
    const int debidas = std::min(NUM_BALAS, int((total - T_fase) / INTERVALO_BALA) + 1);
    //direccion y velocidad
    const cuadrante zonaActual=percibir(objetivo);
    const bool norte = (zonaActual == cuadrante::noroeste || zonaActual == cuadrante::noreste);
    const bool oeste = (zonaActual == cuadrante::noroeste || zonaActual == cuadrante::suroeste);
    const QPointF vel = (modoAtaque == modo::vertical)
                            ? QPointF(0.0, norte ? VEL_BALA : -VEL_BALA)
                            : QPointF(oeste ? VEL_BALA : -VEL_BALA, 0.0);

    while (balasLanzadas < debidas)
    {
        const QPointF origen = puntoBala(balasLanzadas);
        //primero la señal
        if (faseActual == fase::aviso)
        {
            for (efecto& e : efectos)
            {
                if (e.getactivo())
                    continue;
                e.settipo(efecto::senal);
                e.setAncho(LADO_SENAL);
                e.setAlto(LADO_SENAL);
                e.activar(origen - QPointF(LADO_SENAL / 2.0, LADO_SENAL / 2.0), T_AVISO);
                break;   // dura T_AVISO: expira justo cuando sale su bala
            }
        }
        //despues el disparo
        else
        {
            for (proyectil& b : balas)
            {
                if (b.estaActivo())
                    continue;
                b.reiniciar(origen, vel, 0);
                break;
            }
        }
        ++balasLanzadas;
    }
}

void jefe::atacarConLluvia(vector<efecto>& efectos)const
{
    if(faseActual==fase::recarga)
        return;
    const bool aviso=(faseActual==fase::aviso);
    const efecto::tipoefecto tipo = aviso ? efecto::senal : efecto::gas;
    const double lado     = aviso ? LADO_SENAL : LADO_NUBE;
    const double duracion = aviso ? T_AVISO : T_GAS;

    for(int k:bombas)
    {
        const QPointF centro=CENTRO+POS_LLUVIA[k];
        for(efecto& e: efectos)
        {
            //no si ya se esta usando
            if(e.getactivo())
                continue;
            //las posiciones y tal
            e.settipo(tipo);
            e.setAncho(lado);
            e.setAlto(lado);
            e.activar(centro - QPointF(lado / 2.0, lado / 2.0), duracion);
            break;
        }
    }
}

void jefe::atacarDesdeEsquina(double dt,const QPointF& poscapi,const QPointF& vel,
                              vector<proyectil>& balas)
{
    T_esquina-=dt;
    if(T_esquina>0.0)
        return;
    T_esquina+=INTERVALO_ESQUINA;

    //la esquina del turno
    const double lim = MITAD_ARENA - MARGEN_BALA;
    const bool norte = (esquinaActual == cuadrante::noroeste || esquinaActual == cuadrante::noreste);
    const bool oeste = (esquinaActual == cuadrante::noroeste || esquinaActual == cuadrante::suroeste);
    const QPointF origen(CENTRO.x() + (oeste ? -lim : lim), CENTRO.y() + (norte ? -lim : lim));
    // pasa el turno a la siguiente esquina (noroeste, noreste, suroeste, sureste, ...)
    esquinaActual = static_cast<cuadrante>((static_cast<int>(esquinaActual) + 1) % NUM_ESQUINAS);

    //apunta al jugador
    QPointF d=poscapi-origen;
    double largo= hypot(d.x(),d.y());
    if(largo<1.0)
        return;

    QPointF meta=poscapi;

    if (hypot(vel.x(), vel.y()) > VEL_MIN_PRED)
        meta = poscapi + vel * (k * (largo / VEL_ESQUINA));   // adelanto aprendido

    d=meta-origen;
    largo=hypot(d.x(),d.y());
    if(largo<1.0)
        return;

    for(proyectil& b:balas)
    {
        if(b.estaActivo())
            continue;
        b.reiniciar(origen,d*(VEL_ESQUINA/largo),0);
        return;
    }
}

void jefe::lanzarEsbirro(esbirro& s,int i)
{
    //salen de debajo del jefe, repartidos en 4 puntos a lo ancho del cuerpo
    const double x = datos::X_JEFE + (i % 4 + 0.5) * datos::ANCHO_JEFE / 4.0;
    const double y = datos::Y_JEFE + datos::ALTO_JEFE + s.getRadio() + 2.0;
    s.reiniciar(QPointF(x, y), VIDA_ESBIRRO);
    ++esbirrosLanzados;
}

void jefe::iniciarCura(vector<esbirro>& esbirros)
{
    curaRestante=vidaMax/5;
    T_fase=INTERVALO_CURA;
    faseActual=fase::recarga;
    for(int i=0;i< int(esbirros.size()) && esbirrosLanzados<LIMITE_ESBIRROS;i++)
    {
        lanzarEsbirro(esbirros[i],i);
    }
}

void jefe::curar(double dt, vector<esbirro>& esbirros)
{
    bool quedan=false;
    for(int i=0;i<int(esbirros.size());i++)
    {
        if(!esbirros[i].estaVivo() && esbirrosLanzados<LIMITE_ESBIRROS)
            lanzarEsbirro(esbirros[i],i);
        quedan=quedan||esbirros[i].estaVivo();
    }
    if(!quedan)
    {
        terminarCura();
        return;
    }
    T_fase-=dt;
    while(T_fase<=0.0 && curaRestante>0)
    {
        T_fase +=INTERVALO_CURA;
        vida=min(vidaMax,vida+1);
        --curaRestante;
    }
    if(curaRestante==0)
        terminarCura();
}

void jefe::terminarCura()
{
    curaRestante=0;
    faseActual=fase::recarga;
    T_fase=T_RECARGA;
}

bool jefe::estaVivo() const
{ return vida > 0; }

bool jefe::recibirDanio(int d)
{
    if(d<=0||!estaVivo()||estaCurando())
        return false;
    vida=max(0,vida-d);
    return true;
}

bool jefe::estaCurando() const
{ return curaRestante > 0; }

const obstaculo& jefe:: getCuerpo()const
{return CUERPO;}

//acceso

int jefe::getVida() const
{ return vida; }

int jefe::getVidaMax() const
{ return vidaMax; }

jefe::fase jefe::getFase() const
{ return faseActual; }

double jefe::getT_fase() const
{ return T_fase; }

jefe::ataque jefe::getAtaqueActual() const
{ return ataqueActual; }

jefe::modo jefe::getModoAtaque() const
{ return modoAtaque; }


void jefe::setVida(int v)
{ vida = max(0, min(v, vidaMax)); }   // nunca pasa del maximo

void jefe::setVidaMax(int m)
{
    if (m <= 0)
        throw invalid_argument("La vida maxima del jefe debe ser mayor que cero");
    vidaMax = m;
    if (vida > vidaMax) vida = vidaMax;
}
void jefe::setFase(fase f)
{ faseActual = f; }

void jefe::setT_fase(double t)
{ T_fase = (t < 0.0) ? 0.0 : t; }

void jefe::setAtaqueActual(ataque a)
{ ataqueActual = a; }

void jefe::setModoAtaque(modo m)
{ modoAtaque = m; }

const std::array<int, datos::NUM_BOMBAS>& jefe::getBombas() const { return bombas; }

void jefe::setBombas(const std::array<int, datos::NUM_BOMBAS>& b)
{
    for (int k : b)
        if (k < 0 || k >= int(POS_LLUVIA.size()))
            throw out_of_range("Posicion de lluvia inexistente");
    bombas = b;
}

double jefe::getT_esquina() const
{ return T_esquina; }

jefe::cuadrante jefe::getEsquinaActual() const
{ return esquinaActual; }

void jefe::setT_esquina(double t)
{ T_esquina = std::max(0.0, t); }

void jefe::setEsquinaActual(cuadrante c)
{ esquinaActual = c; }


int jefe::getBalasLanzadas() const
{ return balasLanzadas; }

const QPointF& jefe::getObjetivoBalas() const
{ return objetivo; }

void jefe::setBalasLanzadas(int n)
{
    if (n < 0 || n > NUM_BALAS)
        throw out_of_range("Las balas lanzadas deben estar entre 0 y las de la rafaga");
    balasLanzadas = n;
}

void jefe::setObjetivoBalas(const QPointF& p)
{ objetivo = p; }

bool jefe::getFaseEsbirros() const
{ return faseEsbirros; }

int jefe::getCuraRestante() const
{ return curaRestante; }

int jefe::getEsbirrosLanzados() const
{ return esbirrosLanzados; }

void jefe::setFaseEsbirros(bool f)
{ faseEsbirros = f; }

void jefe::setCuraRestante(int c)
{
    if (c < 0)
        throw invalid_argument("La cura restante no puede ser negativa");
    curaRestante = c;
}

void jefe::setEsbirrosLanzados(int n)
{
    if (n < 0 || n > LIMITE_ESBIRROS)
        throw out_of_range("Los esbirros lanzados deben estar entre 0 y el limite");
    esbirrosLanzados = n;
}

double jefe::getK() const
{ return k; }

void jefe::setK(double v)
{ k = max(0.0, min(1.0, v)); }

const QPointF& jefe::getPosRef() const
{ return posRef; }

const QPointF& jefe::getVelRef() const
{ return velRef; }

double jefe::getT_obs() const
{ return T_obs; }

void jefe::setPosRef(const QPointF& p)
{ posRef = p; }

void jefe::setVelRef(const QPointF& v)
{ velRef = v; }

void jefe::setT_obs(double t)
{ T_obs = t; }

//operadores

jefe& jefe::operator=(const jefe& o)
{
    if (this != &o) {
        vida = o.vida;
        vidaMax = o.vidaMax;
        faseActual = o.faseActual;
        T_fase = o.T_fase;
        ataqueActual = o.ataqueActual;
        modoAtaque = o.modoAtaque;
        bombas = o.bombas;
        balasLanzadas = o.balasLanzadas;
        objetivo      = o.objetivo;
        T_esquina     = o.T_esquina;
        esquinaActual = o.esquinaActual;
        faseEsbirros = o.faseEsbirros;
        curaRestante = o.curaRestante;
        esbirrosLanzados = o.esbirrosLanzados;
        k = o.k;
        posRef = o.posRef;
        velRef=o.velRef;
        T_obs=o.T_obs;

    }
    return *this;
}

bool jefe::operator==(const jefe& o) const
{
    return  vida == o.vida
           && vidaMax == o.vidaMax
           && faseActual == o.faseActual
           && std::abs(T_fase - o.T_fase) <= datos::EPSILON
           && ataqueActual == o.ataqueActual
           && modoAtaque == o.modoAtaque
           && bombas == o.bombas
           && balasLanzadas == o.balasLanzadas
           && objetivo == o.objetivo
           && std::abs(T_esquina - o.T_esquina) <= datos::EPSILON
           && esquinaActual == o.esquinaActual
           && faseEsbirros == o.faseEsbirros
           && curaRestante == o.curaRestante
           && esbirrosLanzados == o.esbirrosLanzados
           && std::abs(k - o.k) <= datos::EPSILON
           && posRef == o.posRef
           && velRef == o.velRef
           && std::abs(T_obs - o.T_obs) <= datos::EPSILON;
}
