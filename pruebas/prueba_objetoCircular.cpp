#include "pruebas.h"
#include "objetoCircular.h"
#include "datos.h"

void pruebaObjetoCircular()
{
    titulo("objetoCircular");
    verificar(lanza([] { objetoCircular x(QPointF(), QPointF(), 0.0, 1.0); }), "masa 0 lanza excepcion");
    verificar(lanza([] { objetoCircular x(QPointF(), QPointF(), 1.0, -1.0); }), "radio negativo lanza excepcion");

    objetoCircular caida(QPointF(0, 0), QPointF(0, 0), 1.0, 1.0, true);
    for (int i = 0; i < 60; ++i) caida.actualizar(datos::DT);
    verificar(casi(caida.getPosicion().y(), 250.0) && casi(caida.getVelocidad().y(), 500.0), "caida libre 1 s: y = 250 px, vy = 500 px/s");

    objetoCircular salto(QPointF(0, 0), QPointF(0, -350), 1.0, 1.0, true);
    for (int i = 0; i < 42; ++i) salto.actualizar(datos::DT);
    verificar(casi(salto.getPosicion().y(), -122.5), "lanzamiento vertical: altura maxima 122.5 px");

    objetoCircular recto(QPointF(0, 0), QPointF(60, 0), 1.0, 1.0, false);
    for (int i = 0; i < 60; ++i) recto.actualizar(datos::DT);
    verificar(casi(recto.getPosicion().x(), 60.0) && casi(recto.getPosicion().y(), 0.0), "sin gravedad: movimiento rectilineo");

    objetoCircular a(QPointF(0, 0), QPointF(), 1.0, 5.0), b(QPointF(10, 0), QPointF(), 1.0, 5.0);
    const bool tocan = a.circuloVScirculo(b);
    b.setPosicion(QPointF(10.01, 0));
    verificar(tocan && !a.circuloVScirculo(b), "colision entre circulos en el limite");

    objetoCircular copia;
    copia = a;
    const bool iguales = (copia == a);
    copia.setRadio(9.0);
    verificar(iguales && !(copia == a), "operator= copia y operator== detecta diferencias");
    resumen("objetoCircular");
}
