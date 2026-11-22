#ifndef JEFE_H
#define JEFE_H
#include <array>
#include <QPointF>
#include <vector>
#include "efecto.h"
#include "proyectil.h"
#include "datos.h"
#include "obstaculo.h"
#include "esbirro.h"
class jefe
{
public:
    enum class cuadrante { noroeste, noreste, suroeste, sureste };
    enum class ataque { chorros, balas, lluvia };
    enum class modo   { horizontal, vertical, cruz };
    enum class fase { recarga, aviso, disparo };
private:
    int vida;
    int vidaMax;

    fase faseActual;
    double T_fase; //tiempo para cambiar

    ataque ataqueActual;
    modo modoAtaque;

    int balasLanzadas;// señales (aviso) o balas (disparo) ya emitidas en la fase actual
    QPointF objetivo;

    std::array<int, datos::NUM_BOMBAS> bombas;

    double T_esquina;
    cuadrante esquinaActual;

    //para la dificultad
    bool faseEsbirros;
    int curaRestante;
    int esbirrosLanzados;

    void construirChorros(std::vector<efecto>& reserva, double x0, double y0,
                          efecto::tipoefecto tipo, double duracion) const;
    void atacarConChorros(std::vector<efecto>& efectos) const;
    void atacarConBalas(std::vector<efecto>& efectos, std::vector<proyectil>& balas);
    void atacarConLluvia(std::vector<efecto>& efectos)const;
    void atacarDesdeEsquina(double dt,const QPointF& poscapi,std::vector<proyectil>&balas);
    //donde nace la bala
    QPointF puntoBala(int i) const;
    //fase dificil
    void iniciarCura(std::vector<esbirro>& esbirros);
    void curar(double dt,std::vector<esbirro>& esbirros);
    void terminarCura();
    void lanzarEsbirro(esbirro& s, int i);
public:
    explicit jefe(int vidaMax_, bool faseEsbirros_ = false);
    jefe(const jefe& o) = default;

    //con el jugador
    cuadrante percibir(const QPointF& posCapi)const;
    void razonar(); //elije el ataque,modo y zona
    void actuar(std::vector<efecto>& efectos, std::vector<proyectil>& ba);
    //ciclo de recarga->aviso->atque
    bool actualizar(double dt, const QPointF& posJugador, std::vector<efecto>& efectos,
                    std::vector<proyectil>& balas,std::vector<esbirro>& esbirros);
    bool estaVivo() const;
    bool recibirDanio(int d);
    bool estaCurando() const;
    //este no es un get de atributo es un "cuerpo calculado" para rebibir danio
    const obstaculo& getCuerpo()const;
    //acceso

    int getVida() const;
    int getVidaMax() const;
    fase getFase() const;
    double getT_fase() const;
    ataque getAtaqueActual() const;
    modo getModoAtaque() const;
    int getBalasLanzadas() const;
    const QPointF& getObjetivoBalas() const;
    void setVida(int v);
    void setVidaMax(int m);
    void setFase(fase f);
    void setT_fase(double t);
    void setAtaqueActual(ataque a);
    void setModoAtaque(modo m);
    void setBalasLanzadas(int n);
    void setObjetivoBalas(const QPointF& p);
    const std::array<int, datos::NUM_BOMBAS>& getBombas() const;
    void setBombas(const std::array<int, datos::NUM_BOMBAS>& b);
    double getT_esquina() const;
    cuadrante getEsquinaActual() const;
    void setT_esquina(double t);
    void setEsquinaActual(cuadrante c);
    bool getFaseEsbirros() const;
    int getCuraRestante() const;
    int getEsbirrosLanzados() const;
    void setFaseEsbirros(bool f);
    void setCuraRestante(int c);
    void setEsbirrosLanzados(int n);

    //operadores
    jefe& operator=(const jefe& o);
    bool operator==(const jefe& o) const;
};

#endif // JEFE_H
