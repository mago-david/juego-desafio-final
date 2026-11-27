#include "pruebas.h"
#include "nivel1.h"
#include "datos.h"
#include <vector>
using namespace std;

static int activos(const vector<proyectil>& v)
{
    int n = 0;
    for (const proyectil& p : v) if (p.estaActivo()) ++n;
    return n;
}

static void avanzar(nivel1& n, int pasos)
{
    for (int i = 0; i < pasos; ++i) n.actualizar(datos::DT);
}

void pruebaNivel1()
{
    titulo("nivel1");
    nivel1 w;
    w.setPersonaje(capitanN1(QPointF(500, 530)));
    avanzar(w, 1);
    const bool activado = w.getMuro().getactivo();
    w.setPersonaje(capitanN1(QPointF(0, 530)));
    avanzar(w, 1);
    verificar(activado && !w.getPersonaje().estaVivo(), "el muro se activa al avanzar y mata al alcanzar");

    nivel1 n;
    avanzar(n, 170);
    verificar(activos(n.getBalas()) + activos(n.getLacrimogenas()) == 0, "antes de los 3 s no se dispara nada");
    avanzar(n, 14);
    verificar(activos(n.getBalas()) >= 1 && n.getDisparosRestantes() == 8 && n.getPrimeraAltura() == 5, "a los 3 s empieza el ataque cerca de la altura del jugador");

    avanzar(n, 428);   // 10.2 s
    const bool termino = n.getDisparosRestantes() == 0;
    avanzar(n, 30);    // 10.7 s
    verificar(termino && n.getDisparosRestantes() > 0, "termina el ataque, espera y empieza otro");

    nivel1 m;
    m.setTReloj(1000.0);   // aplaza el ataque automatico
    bool todas = true;
    for (int i = 0; i < 10; ++i) todas = m.lanzarBala(QPointF(700, 300), QPointF(-300, 0)) && todas;
    verificar(todas && !m.lanzarBala(QPointF(700, 300), QPointF(-300, 0)), "con las 10 balas vivas no lanza otra");
    avanzar(m, 240);
    verificar(activos(m.getBalas()) == 0, "las balas se desactivan al chocar y liberan el pool");

    verificar(lanza([&n] { n.setPrimeraAltura(6); }) && lanza([&n] { n.setBalas(vector<proyectil>()); }), "rechaza altura fuera de rango y pools de otro tamano");
    resumen("nivel1");
}
