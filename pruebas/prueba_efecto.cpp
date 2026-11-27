#include "pruebas.h"
#include "efecto.h"
#include "capitanN2.h"
#include "datos.h"

void pruebaEfecto()
{
    titulo("efecto");
    verificar(lanza([] { efecto e; e.setduracion(-1.0); }), "duracion negativa lanza excepcion");

    efecto gas(QPointF(0, 0), 100.0, 100.0, efecto::gas, 3.0);
    capitan a(QPointF(50, 50), QPointF(), 1.0, 20.0, false);
    const bool marca = gas.aplicar(a) && a.getEnNube();
    for (int i = 0; i < 186; ++i) gas.actualizar(datos::DT);
    verificar(marca && !gas.getactivo(), "gas: marca al capitan y desaparece a los 3 s");

    efecto bufo1(QPointF(0, 0), 100.0, 100.0, efecto::bufo, 5.0);
    capitan b(QPointF(50, 50), QPointF(), 1.0, 20.0, false, 2, 3);
    verificar(bufo1.aplicar(b) && b.getVidas() == 3 && casi(b.getT_veloz(), 5.0) && !bufo1.getactivo(), "bufo en nivel 1: vida, velocidad y se consume");

    efecto bufo2(QPointF(0, 0), 100.0, 100.0, efecto::bufo, 5.0);
    capitanN2 c(QPointF(50, 50));
    verificar(bufo2.aplicar(c) && c.getVidas() == 4 && c.getFortachon() && c.getDanioEscudo() == 15 && casi(c.getT_veloz(), 0.0), "bufo en nivel 2: vida y fortachon");

    efecto golpe(QPointF(0, 0), 100.0, 100.0, efecto::danio, 0.0);
    capitan d(QPointF(20, 50), QPointF(), 1.0, 20.0, false);
    const bool primero = golpe.aplicar(d);
    const bool segundo = golpe.aplicar(d);
    verificar(primero && !segundo && d.getVidas() == 2 && casi(d.getVelocidad().x(), -250.0) && casi(d.getVelocidad().y(), -250.0), "dano: quita una vida, empuja y respeta invulnerabilidad");

    for (int i = 0; i < 600; ++i) golpe.actualizar(datos::DT);
    verificar(golpe.getactivo(), "duracion 0 es permanente");

    efecto senal(QPointF(0, 0), 100.0, 100.0, efecto::senal, 0.6);
    capitan f(QPointF(50, 50), QPointF(), 1.0, 20.0, false);
    const bool inofensiva = !senal.aplicar(f) && f.getVidas() == 3;
    for (int i = 0; i < 40; ++i) senal.actualizar(datos::DT);
    verificar(inofensiva && !senal.getactivo(), "senal: no hace dano y expira");
    resumen("efecto");
}
