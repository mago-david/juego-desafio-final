#include "pruebas.h"
#include "jefe.h"
#include <vector>
using namespace std;

static void matar(vector<esbirro>& v) { for (esbirro& s : v) s.recibirGolpe(1000); }

void pruebaJefe()
{
    titulo("jefe");
    verificar(lanza([] { jefe j(0); }), "vida maxima 0 lanza excepcion");

    jefe a(100);
    verificar(a.percibir(QPointF(500, 400)) == jefe::cuadrante::sureste
              && a.percibir(QPointF(300, 200)) == jefe::cuadrante::noroeste, "percibe el cuadrante del jugador");

    const bool nulo = a.recibirDanio(0);
    const bool normal = a.recibirDanio(10) && a.getVida() == 90;
    a.recibirDanio(500);
    verificar(!nulo && normal && a.getVida() == 0 && !a.estaVivo() && !a.recibirDanio(10), "recibe dano sin bajar de 0");

    jefe d(100, true);
    vector<efecto> efectos(16);
    vector<proyectil> balas;
    vector<esbirro> esbirros(4);
    d.setVida(50);
    d.actualizar(0.01, QPointF(400, 480), QPointF(), efectos, balas, esbirros);
    int vivos = 0;
    for (const esbirro& s : esbirros) if (s.estaVivo()) ++vivos;
    verificar(d.estaCurando() && vivos == 4 && !d.recibirDanio(10), "dificil a media vida: cura, lanza esbirros y es inmune");

    matar(esbirros);
    d.actualizar(0.01, QPointF(400, 480), QPointF(), efectos, balas, esbirros);   // segunda tanda
    matar(esbirros);
    d.actualizar(0.01, QPointF(400, 480), QPointF(), efectos, balas, esbirros);
    verificar(!d.estaCurando() && d.getEsbirrosLanzados() == 8, "la cura termina al eliminar las dos tandas");

    jefe f(100);
    vector<efecto> reserva(16);
    vector<esbirro> ninguno;
    f.actualizar(1.7, QPointF(400, 480), QPointF(), reserva, balas, ninguno);
    const bool aviso = f.getFase() == jefe::fase::aviso;
    f.actualizar(0.7, QPointF(400, 480), QPointF(), reserva, balas, ninguno);
    const bool disparo = f.getFase() == jefe::fase::disparo;
    f.actualizar(1.6, QPointF(400, 480), QPointF(), reserva, balas, ninguno);
    verificar(aviso && disparo && f.getFase() == jefe::fase::recarga, "ciclo recarga, aviso, disparo, recarga");

    jefe g(100);
    vector<efecto> chorros(16);
    g.setFase(jefe::fase::aviso);
    g.setAtaqueActual(jefe::ataque::chorros);
    g.setModoAtaque(jefe::modo::cruz);
    g.setObjetivoBalas(QPointF(500, 400));
    g.actuar(chorros, balas);
    int senales = 0, horizontales = 0;
    for (const efecto& x : chorros)
        if (x.getactivo() && x.gettipo() == efecto::senal) {
            ++senales;
            if (x.getAncho() > x.getAlto()) ++horizontales;
        }
    verificar(senales == 2 && horizontales == 1, "ataque en cruz: una senal horizontal y una vertical");
    resumen("jefe");
}
