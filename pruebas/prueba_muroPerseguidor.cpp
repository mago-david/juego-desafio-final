#include "pruebas.h"
#include "muroPerseguidor.h"
#include "datos.h"

void pruebaMuroPerseguidor()
{
    titulo("muroPerseguidor");
    muroPerseguidor m(QPointF(0, 0), 40.0, 550.0, 60.0);
    m.actualizar(1.0, 300.0);
    verificar(casi(m.getEsquina().x(), 0.0), "inactivo no avanza");
    verificar(lanza([&m] { m.setvelocidad(-1.0); }), "velocidad negativa lanza excepcion");

    capitan atras(QPointF(30, 100), QPointF(), 1.0, 20.0, false);
    verificar(!m.aplicar(atras) && atras.getVidas() == 3, "inactivo no hace dano");

    m.setactivo(true);
    muroPerseguidor lejos = m;
    for (int i = 0; i < 60; ++i) m.actualizar(datos::DT, 300.0);
    verificar(casi(m.getEsquina().x(), 60.0), "capitan cerca: avanza a su velocidad");
    for (int i = 0; i < 60; ++i) lejos.actualizar(datos::DT, 2000.0);
    verificar(casi(lejos.getEsquina().x(), 120.0), "capitan a mas de 500 px: avanza al doble");

    verificar(m.aplicar(atras) && !atras.estaVivo(), "alcanzar al capitan lo mata al instante");
    capitan delante(QPointF(500, 100), QPointF(), 1.0, 20.0, false);
    verificar(!m.aplicar(delante) && delante.getVidas() == 3, "capitan por delante no sufre dano");
    resumen("muroPerseguidor");
}
