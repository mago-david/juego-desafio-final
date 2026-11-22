#include "vistaN2.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QBrush>
#include <QPen>

namespace
{
// colores de prueba: se cambian por sprites cuando existan
const QColor COLOR_SENAL(255, 220, 0, 110);    // amarillo translucido: "!"
const QColor COLOR_CHORRO(60, 140, 255, 200);  // agua
const QColor COLOR_GAS(160, 160, 160, 120);    // nube
const QColor COLOR_OTRO(80, 200, 80, 150);     // velocidad / vida (por si aparecen)
const QColor COLOR_JEFE(150, 20, 20);
const QColor COLOR_CURA(40, 160, 60);          // jefe curandose (invulnerable)
const QColor COLOR_ESBIRRO(140, 60, 170);
}

vistaN2::vistaN2(QWidget* parent)
    : vistaNivel(parent),
    nivel(), dibujoPersonaje(nullptr), dibujoJefe(nullptr), textoVidas(nullptr),
    teclaW(false), teclaA(false), teclaS(false), teclaD(false)
{
    escena->setSceneRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setBackgroundBrush(Qt::black);                 // lo que queda fuera de la arena
    dibujarArena();
    dibujarJefe();
    dibujarPersonaje();
    crearDibujos(nivel.getPersonaje().getEscudos(), dibujoEscudos, QColor(200, 200, 220));
    crearDibujos(nivel.getBalas(), dibujoBalas, QColor(255, 80, 40));
    crearEfectos();
    crearEsbirros();

    textoVidas = escena->addSimpleText(QString());
    textoVidas->setBrush(Qt::white);
    textoVidas->setPos(10.0, 10.0);
    textoVidas->setZValue(10);
}

void vistaN2::dibujarArena()
{
    const QPen borde(Qt::black);
    const QBrush solida(QColor(90, 90, 90));

    QRectF area;                                   // rectangulo que abarcan las 4 paredes
    for (const obstaculo& o : nivel.getParedes()) {
        const QRectF r(o.getIzquierda(), o.getArriba(), o.getAncho(), o.getAlto());
        escena->addRect(r, borde, solida);
        area = area.united(r);
    }
    // el piso va debajo de las paredes
    QGraphicsRectItem* piso = escena->addRect(area, QPen(Qt::NoPen), QBrush(QColor(55, 60, 75)));
    piso->setZValue(-10);
}

void vistaN2::dibujarPersonaje()
{
    const double r = nivel.getPersonaje().getRadio();
    // circulo de prueba: se cambia por el sprite cenital cuando exista
    dibujoPersonaje = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                         QPen(Qt::black), QBrush(QColor(40, 90, 200)));
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujoPersonaje->setZValue(5);
}

void vistaN2::dibujarJefe()
{
    // el jefe no se mueve: se dibuja una sola vez, con las medidas de su cuerpo
    const obstaculo& c = nivel.getJefe().getCuerpo();
    dibujoJefe = escena->addRect(c.getIzquierda(), c.getArriba(), c.getAncho(), c.getAlto(),
                                 QPen(Qt::black), QBrush(COLOR_JEFE));
    dibujoJefe->setZValue(3);
}

void vistaN2::crearEfectos()
{
    // un rectangulo por efecto, creados una sola vez y reutilizados (como los pools)
    const int n = int(nivel.getEfectos().size());
    dibujoEfectos.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        QGraphicsRectItem* it = escena->addRect(QRectF(), QPen(Qt::NoPen), QBrush(COLOR_OTRO));
        it->setVisible(false);
        dibujoEfectos.append(it);
    }
}

void vistaN2::dibujarEfectos()
{
    const std::vector<efecto>& efectos = nivel.getEfectos();
    for (int i = 0; i < int(efectos.size()); ++i)
    {
        const efecto& e = efectos[i];
        QGraphicsRectItem* it = dibujoEfectos[i];
        it->setVisible(e.getactivo());
        if (!e.getactivo())
            continue;
        it->setRect(e.getIzquierda(), e.getArriba(), e.getAncho(), e.getAlto());
        switch (e.gettipo()) {
        case efecto::senal: it->setBrush(COLOR_SENAL);  it->setZValue(6); break;   // encima de todo
        case efecto::danio: it->setBrush(COLOR_CHORRO); it->setZValue(2); break;
        case efecto::gas:   it->setBrush(COLOR_GAS);    it->setZValue(2); break;
        default:            it->setBrush(COLOR_OTRO);   it->setZValue(2); break;
        }
    }
}

void vistaN2::crearEsbirros()
{
    // un circulo por hueco del pool, creados una sola vez
    const std::vector<esbirro>& esbirros = nivel.getEsbirros();
    dibujoEsbirros.reserve(int(esbirros.size()));
    for (const esbirro& s : esbirros)
    {
        const double r = s.getRadio();
        QGraphicsEllipseItem* it = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                                      QPen(Qt::black), QBrush(COLOR_ESBIRRO));
        it->setZValue(4);
        it->setVisible(false);
        dibujoEsbirros.append(it);
    }
}

void vistaN2::dibujarEsbirros()
{
    const std::vector<esbirro>& esbirros = nivel.getEsbirros();
    for (int i = 0; i < int(esbirros.size()); ++i)
    {
        const bool vivo = esbirros[i].estaVivo();   // vida 0 = hueco libre
        dibujoEsbirros[i]->setVisible(vivo);
        if (vivo)
            dibujoEsbirros[i]->setPos(esbirros[i].getPosicion());
    }
}

void vistaN2::avanzarLogica(double dt)
{
    nivel.actualizar(dt);
    if (!nivel.getJefe().estaVivo())
        terminar(true);
    else if (!nivel.getPersonaje().estaVivo())
        terminar(false);                           // pausa y avisa a la ventana
}

void vistaN2::dibujar(double)
{
    // sin interpolar por ahora: capitanN2 no guarda la posicion anterior
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujarPool(nivel.getPersonaje().getEscudos(), dibujoEscudos);
    dibujarPool(nivel.getBalas(), dibujoBalas);
    dibujarEfectos();
    dibujarEsbirros();

    // el color del jefe solo se toca cuando cambia de estado
    const bool curando = nivel.getJefe().estaCurando();
    const QColor& color = curando ? COLOR_CURA : COLOR_JEFE;
    if (dibujoJefe->brush().color() != color)
        dibujoJefe->setBrush(color);

    textoVidas->setText(QString("Vidas: %1     Jefe: %2 / %3%4")
                            .arg(nivel.getPersonaje().getVidas())
                            .arg(nivel.getJefe().getVida())
                            .arg(nivel.getJefe().getVidaMax())
                            .arg(curando ? "   CURANDO" : ""));
}

void vistaN2::actualizarDireccion()
{
    // en Qt y crece hacia abajo: S = +1, W = -1
    nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0),
                                (teclaS ? 1 : 0) - (teclaW ? 1 : 0));
}

void vistaN2::alPulsar(int tecla)
{
    switch (tecla) {
    case Qt::Key_W: teclaW = true; actualizarDireccion(); break;
    case Qt::Key_A: teclaA = true; actualizarDireccion(); break;
    case Qt::Key_S: teclaS = true; actualizarDireccion(); break;
    case Qt::Key_D: teclaD = true; actualizarDireccion(); break;
    default: break;
    }
}

void vistaN2::alSoltar(int tecla)
{
    switch (tecla) {
    case Qt::Key_W: teclaW = false; actualizarDireccion(); break;
    case Qt::Key_A: teclaA = false; actualizarDireccion(); break;
    case Qt::Key_S: teclaS = false; actualizarDireccion(); break;
    case Qt::Key_D: teclaD = false; actualizarDireccion(); break;
    default: break;
    }
}

void vistaN2::alPerderFoco()
{
    teclaW = teclaA = teclaS = teclaD = false;
    actualizarDireccion();
}

void vistaN2::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        nivel.lanzarEscudo(mapToScene(e->pos()));      // posicion del click en coordenadas de la escena
    else
        vistaNivel::mousePressEvent(e);
}

const nivel2& vistaN2::getNivel() const
{ return nivel; }
bool vistaN2::getTeclaW() const
{ return teclaW; }
bool vistaN2::getTeclaA() const
{ return teclaA; }
bool vistaN2::getTeclaS() const
{ return teclaS; }
bool vistaN2::getTeclaD() const
{ return teclaD; }