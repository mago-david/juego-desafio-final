#include "pruebas.h"
#include "proyectil.h"
#include "obstaculo.h"
#include "datos.h"

void pruebaProyectil()
{
    titulo("proyectil");
    verificar(lanza([] { proyectil p(QPointF(), QPointF(), 1.0, 1.0, false, -1, 1.0); }), "rebotes negativos lanzan excepcion");
    verificar(lanza([] { proyectil p(QPointF(), QPointF(), 1.0, 1.0, false, 0, 1.5); }), "restitucion fuera de [0,1] lanza excepcion");

    // rebotesMax es una cuenta regresiva
    proyectil p(QPointF(), QPointF(), 1.0, 1.0, false, 3, 1.0);
    p.registrarRebote(); p.registrarRebote(); p.registrarRebote();
    verificar(p.estaActivo() && p.getRebotesMax() == 0, "sobrevive 3 rebotes");
    p.registrarRebote();
    verificar(!p.estaActivo(), "el rebote 4 lo desactiva");

    p.reiniciar(QPointF(5, 6), QPointF(7, 8), 2);
    verificar(p.estaActivo() && p.getRebotesMax() == 2 && casi(p.getVelocidad().y(), 8), "reiniciar lo reactiva para reutilizarlo");

    obstaculo suelo(QPointF(0, 550), 4000.0, 50.0);
    proyectil l(QPointF(3000, 110), QPointF(-300, 0), 1.0, 6.0, true, 3, 0.9);
    int impactos = 0;
    for (int i = 0; i < 1200 && l.estaActivo(); ++i) {
        l.actualizar(datos::DT);
        if (suelo.colisionar(l, l.getCoefRestitucion())) { ++impactos; l.registrarRebote(); }
    }
    verificar(impactos == 4 && !l.estaActivo(), "con gravedad: se apaga en el 4o impacto real");

    proyectil q;
    q = p;
    verificar(q == p, "operator= y operator==");
    resumen("proyectil");
}
