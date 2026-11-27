#include "pruebas.h"
#include "capitanN2.h"
#include "datos.h"
#include <vector>
using namespace std;

static int activos(const capitanN2& c)
{
    int n = 0;
    for (const proyectil& e : c.getEscudos()) if (e.estaActivo()) ++n;
    return n;
}

void pruebaCapitanN2()
{
    titulo("capitanN2");
    capitanN2 c(QPointF(100, 100));
    verificar(c.getVidasMax() == 4 && activos(c) == 0 && c.getEscudos().size() == 6, "4 vidas maximas y pool de 6 escudos libres");

    const bool encima = c.lanzarEscudo(QPointF(100.5, 100));
    const bool lanzado = c.lanzarEscudo(QPointF(200, 100));
    verificar(!encima && lanzado && casi(c.getEscudos()[0].getVelocidad().x(), 450.0), "lanza hacia el objetivo a 450 px/s, no encima de si mismo");

    const bool pronto = c.lanzarEscudo(QPointF(200, 100));
    for (int i = 0; i < 43; ++i) c.actualizar(datos::DT);
    verificar(!pronto && c.lanzarEscudo(QPointF(200, 100)), "recarga de 0.7 s entre lanzamientos");

    capitanN2 d(QPointF(100, 100));
    for (int i = 0; i < 6; ++i) { d.setTRecarga(0.0); d.lanzarEscudo(QPointF(200, 100)); }
    d.setTRecarga(0.0);
    verificar(activos(d) == 6 && !d.lanzarEscudo(QPointF(200, 100)), "con el pool lleno no lanza otro");

    capitanN2 e(QPointF(0, 0));
    e.setDireccionX(1); e.setDireccionY(1);
    for (int i = 0; i < 600; ++i) e.actualizar(datos::DT);
    verificar(casi(std::hypot(e.getVelocidad().x(), e.getVelocidad().y()), 250.0, 0.01), "en diagonal tambien va a 250 px/s");

    vector<obstaculo> paredes;
    paredes.emplace_back(QPointF(0, 0), 50.0, 600.0);
    paredes.emplace_back(QPointF(500, 0), 50.0, 600.0);
    capitanN2 f(QPointF(300, 300));
    f.lanzarEscudo(QPointF(400, 300));
    for (int i = 0; i < 180; ++i) { f.actualizar(datos::DT); f.resolverParedes(paredes); }   // 3.0 s
    const bool vivo = activos(f) == 1 && f.getEscudos()[0].getRebotesMax() == 0;
    for (int i = 0; i < 30; ++i) { f.actualizar(datos::DT); f.resolverParedes(paredes); }    // 3.5 s
    verificar(vivo && activos(f) == 0, "el escudo aguanta 3 rebotes y el 4o lo apaga");

    capitanN2 g;
    const bool base = g.getDanioEscudo() == 10;
    g.activarBufoDanio(1.0);
    const bool fuerte = g.getDanioEscudo() == 15;
    for (int i = 0; i < 61; ++i) g.actualizar(datos::DT);
    verificar(base && fuerte && !g.getFortachon() && g.getDanioEscudo() == 10, "bufo fortachon: sube el dano y expira");
    resumen("capitanN2");
}
