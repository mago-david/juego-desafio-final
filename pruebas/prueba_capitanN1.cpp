#include "pruebas.h"
#include "capitanN1.h"
#include "datos.h"
#include <vector>
#include <algorithm>
using namespace std;

void pruebaCapitanN1()
{
    titulo("capitanN1");
    capitanN1 a(QPointF(0, 0));
    const bool s1 = a.saltar(), s2 = a.saltar(), s3 = a.saltar();
    verificar(s1 && s2 && !s3, "doble salto: el tercero no se permite");

    capitanN1 d(QPointF(0, 0));
    d.setDireccion(1);
    for (int i = 0; i < 600; ++i) d.actualizar(datos::DT);
    const bool normal = casi(d.getVelocidad().x(), 250.0, 0.01);
    d.setEnNube(true);
    for (int i = 0; i < 600; ++i) d.actualizar(datos::DT);
    verificar(normal && casi(d.getVelocidad().x(), 100.0, 0.01), "corre a 250 px/s y en la nube a 100 px/s");

    vector<obstaculo> suelo;
    suelo.emplace_back(QPointF(0, 550), 1000.0, 50.0);
    capitanN1 e(QPointF(100, 500));
    e.setSaltosRestantes(0);
    for (int i = 0; i < 120; ++i) { e.actualizar(datos::DT); e.resolverPlataformas(suelo); }
    verificar(e.getEnSuelo() && casi(e.getPosicion().y(), 530.0, 1.0) && e.getSaltosRestantes() == 2, "cae al suelo, queda apoyado y recupera los 2 saltos");

    vector<obstaculo> plat;
    plat.emplace_back(QPointF(0, 400), 200.0, 20.0, true);
    capitanN1 f(QPointF(100, 500));
    f.saltar();
    double minimo = 500.0;
    for (int i = 0; i < 180; ++i) {
        f.actualizar(datos::DT); f.resolverPlataformas(plat);
        minimo = min(minimo, f.getPosicion().y());
    }
    verificar(minimo < 390.0 && f.getEnSuelo() && casi(f.getPosicion().y(), 380.0, 1.0), "plataforma atravesable: sube a traves y aterriza encima");

    const bool bajo = f.bajarPlataforma();
    for (int i = 0; i < 60; ++i) { f.actualizar(datos::DT); f.resolverPlataformas(plat); }
    verificar(bajo && f.getPosicion().y() > 420.0, "bajarPlataforma la atraviesa hacia abajo");

    vector<obstaculo> techo;
    techo.emplace_back(QPointF(0, 300), 200.0, 50.0);
    capitanN1 t(QPointF(100, 390));
    t.saltar();
    t.resolverPlataformas(techo);
    verificar(casi(t.getPosicion().y(), 394.0) && casi(t.getVelocidad().y(), 0.0), "la cabeza choca con el techo y deja de subir");

    capitanN1 g(QPointF(100, 300));
    objetoCircular cabeza(QPointF(100, 251), QPointF(), 1.0, 6.0), lejos(QPointF(100, 240), QPointF(), 1.0, 6.0);
    verificar(g.esGolpeado(cabeza) && !g.esGolpeado(lejos), "hitbox: incluye 24 px extra de cabeza");
    resumen("capitanN1");
}
