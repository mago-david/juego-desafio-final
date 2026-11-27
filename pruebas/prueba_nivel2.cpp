#include "pruebas.h"
#include "nivel2.h"
#include "datos.h"
#include <vector>

static int activos(const nivel2& n)
{
    int c = 0;
    for (const proyectil& e : n.getPersonaje().getEscudos()) if (e.estaActivo()) ++c;
    return c;
}

// aplaza los ataques del jefe para que las pruebas no dependan del azar
static void calmar(nivel2& n)
{
    jefe j = n.getJefe();
    j.setT_fase(1000.0);
    j.setT_esquina(1000.0);
    n.setJefe(j);
}

static void avanzar(nivel2& n, int pasos)
{
    for (int i = 0; i < pasos; ++i) n.actualizar(datos::DT);
}

// Arena: el capitan (radio 20) se mueve entre x 170..630 y y 70..530; empieza en (400, 480)
void pruebaNivel2()
{
    titulo("nivel2");
    nivel2 n;
    verificar(n.getParedes().size() == 5, "4 paredes mas el cuerpo del jefe");

    calmar(n);
    n.setDireccionPersonaje(1, 0);
    avanzar(n, 180);
    verificar(casi(n.getPersonaje().getPosicion().x(), 630.0, 0.01), "caminando a la derecha se frena en la pared");

    nivel2 m;
    calmar(m);
    m.setDireccionPersonaje(-1, -1);
    avanzar(m, 300);
    verificar(casi(m.getPersonaje().getPosicion().x(), 170.0, 0.01) && casi(m.getPersonaje().getPosicion().y(), 70.0, 0.01), "en diagonal queda en la esquina sin atravesar paredes");

    nivel2 e;
    calmar(e);
    e.lanzarEscudo(QPointF(500, 480));
    avanzar(e, 210);   // 3.5 s
    const bool vivo = activos(e) == 1;
    avanzar(e, 24);    // 3.9 s
    verificar(vivo && activos(e) == 0, "el escudo se apaga tras agotar sus rebotes");

    nivel2 h;
    calmar(h);
    const int vidaAntes = h.getJefe().getVida();
    h.lanzarEscudo(QPointF(400, 85));
    avanzar(h, 60);
    verificar(h.getJefe().getVida() == vidaAntes - 10 && activos(h) == 0, "el escudo dana al jefe y desaparece");

    nivel2 b;
    calmar(b);
    std::vector<proyectil> balas = b.getBalas();
    balas[0].reiniciar(b.getPersonaje().getPosicion(), QPointF(), 0);
    b.setBalas(balas);
    avanzar(b, 1);
    verificar(b.getPersonaje().getVidas() == 2 && !b.getBalas()[0].estaActivo(), "una bala dana al capitan y se apaga");

    nivel2 facil(false), dificil(true);
    verificar(facil.getEsbirros().empty() && facil.getJefe().getVida() == 400
              && dificil.getEsbirros().size() == 4 && dificil.getJefe().getVida() == 525, "la dificultad cambia la vida del jefe y los esbirros");
    resumen("nivel2");
}
