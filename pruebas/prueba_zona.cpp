#include "pruebas.h"
#include "zona.h"

void pruebaZona()
{
    titulo("zona");
    zona z(QPointF(10, 20), 100.0, 50.0);
    verificar(casi(z.getIzquierda(), 10) && casi(z.getDerecha(), 110) && casi(z.getArriba(), 20) && casi(z.getAbajo(), 70), "los bordes salen de esquina, ancho y alto");
    verificar(lanza([] { zona x(QPointF(), 0.0, 5.0); }), "ancho 0 lanza excepcion");
    verificar(lanza([&z] { z.setAlto(-1.0); }) && casi(z.getAlto(), 50), "alto negativo lanza excepcion y no cambia el valor");

    objetoCircular dentro(QPointF(50, 45), QPointF(), 1.0, 5.0);
    verificar(z.contiene(dentro), "contiene un objeto en su interior");
    objetoCircular borde(QPointF(115, 45), QPointF(), 1.0, 5.0);
    verificar(z.contiene(borde), "contiene un objeto que solo roza el borde");
    objetoCircular fuera(QPointF(116, 45), QPointF(), 1.0, 5.0);
    verificar(!z.contiene(fuera), "no contiene un objeto separado del borde");

    zona copia;
    copia = z;
    const bool iguales = (copia == z);
    copia.setEsquina(QPointF(0, 0));
    verificar(iguales && !(copia == z) && casi(copia.getDerecha(), 100), "operator=, operator== y bordes tras mover");
    resumen("zona");
}
