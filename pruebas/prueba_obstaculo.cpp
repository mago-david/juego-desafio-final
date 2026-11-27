#include "pruebas.h"
#include "obstaculo.h"

// Rectangulo en (0,0) de 100x50; la bola de radio 10 toca su borde superior.
static objetoCircular bola(double vy) { return objetoCircular(QPointF(50, -5), QPointF(0, vy), 1.0, 10.0, false); }

void pruebaObstaculo()
{
    titulo("obstaculo");
    obstaculo r(QPointF(0, 0), 100.0, 50.0);
    verificar(r.tocaCirculo(QPointF(104, 25), 5.0) && !r.tocaCirculo(QPointF(106, 25), 5.0), "tocaCirculo a 4 px si, a 6 px no (radio 5)");

    objetoCircular a = bola(100);
    verificar(r.colisionar(a, 0.5) && casi(a.getVelocidad().y(), -50.0), "e = 0.5: impacto real y rebota a la mitad");
    objetoCircular b = bola(100);
    verificar(r.colisionar(b, 0.0) && casi(b.getVelocidad().y(), 0.0) && casi(b.getPosicion().y(), -10.0), "e = 0: aterriza sin rebotar y sale del obstaculo");
    objetoCircular c = bola(10);
    verificar(!r.colisionar(c, 0.9) && casi(c.getVelocidad().y(), 0.0), "apoyado (velocidad < umbral): false y queda en reposo");
    objetoCircular d = bola(-100);
    verificar(!r.colisionar(d, 0.9), "si ya se aleja no hay impacto");
    objetoCircular e = bola(100);
    verificar(lanza([&] { r.colisionar(e, 1.5); }), "restitucion fuera de [0,1] lanza excepcion");

    obstaculo base(QPointF(0, 0), 10.0, 10.0, true, 3);
    obstaculo copia;
    copia = base;
    const bool iguales = (copia == base);
    copia.setSpriteId(4);
    verificar(iguales && !(copia == base), "operator= y operator== incluyen atravesable y spriteId");
    resumen("obstaculo");
}
