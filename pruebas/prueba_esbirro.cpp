#include "pruebas.h"
#include "esbirro.h"
#include "datos.h"

void pruebaEsbirro()
{
    titulo("esbirro");
    esbirro e;
    verificar(!e.estaVivo(), "un esbirro nuevo es un hueco libre (vida 0)");
    e.reiniciar(QPointF(0, 0), 30);
    verificar(e.estaVivo() && e.getVida() == 30 && casi(e.getVelocidad().x(), 0.0), "reiniciar lo activa quieto y con su vida");
    verificar(lanza([&e] { e.setVida(-1); }), "vida negativa lanza excepcion");

    for (int i = 0; i < 60; ++i) { e.perseguir(QPointF(400, 0)); e.actualizar(datos::DT); }
    verificar(casi(e.getPosicion().x(), 90.0), "persigue a 90 px/s");

    e.perseguir(e.getPosicion() + QPointF(0.5, 0));
    verificar(casi(e.getVelocidad().x(), 0.0) && casi(e.getVelocidad().y(), 0.0), "pegado al objetivo se detiene (sin dividir por cero)");

    e.recibirGolpe(10);
    e.recibirGolpe(-5);
    verificar(e.getVida() == 20 && e.estaVivo(), "recibe dano e ignora valores negativos");
    e.recibirGolpe(100);
    verificar(!e.estaVivo() && e.getVida() == 0, "muere y la vida no baja de 0");
    resumen("esbirro");
}
