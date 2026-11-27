#include "pruebas.h"
#include "capitan.h"
#include "datos.h"

void pruebaCapitan()
{
    titulo("capitan");
    capitan c(QPointF(0, 0), QPointF(0, 0), 1.0, 20.0, false);
    verificar(lanza([&c] { c.setVidas(4); }), "vidas por encima del maximo lanza excepcion");

    const bool primero = c.recibirDanio();
    const bool segundo = c.recibirDanio();
    verificar(primero && !segundo && c.getVidas() == 2, "un golpe quita una vida y deja invulnerable");
    for (int i = 0; i < 91; ++i) c.actualizar(datos::DT);   // 1.5 s
    verificar(c.recibirDanio() && c.getVidas() == 1, "pasado 1.5 s vuelve a recibir dano");

    c.setVidas(0);
    c.setT_invulnerable(0.0);
    verificar(!c.estaVivo() && !c.recibirDanio() && c.getVidas() == 0, "sin vidas esta muerto y no baja de 0");

    capitan d(QPointF(0, 0), QPointF(0, 0), 1.0, 20.0, false, 2, 3);
    const bool sube = d.ganarVida();
    const bool pasa = d.ganarVida();
    verificar(sube && !pasa && d.getVidas() == 3, "ganarVida sube hasta el maximo y no pasa");

    d.setEnNube(true);
    d.setT_veloz(1.0);
    const bool conEfectos = casi(d.getLimiteVel(), datos::COEF_FRICCION) && casi(d.getBufoVelocidad(), 1.5);
    for (int i = 0; i < 61; ++i) d.actualizar(datos::DT);
    verificar(conEfectos && casi(d.getBufoVelocidad(), 1.0), "la nube frena y el bufo de velocidad expira");

    capitan e;
    e = d;
    verificar(e == d, "operator= y operator==");
    resumen("capitan");
}
