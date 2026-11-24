#include "vistaN2.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <algorithm>
#include <cmath>
#include "efecto.h"
#include "esbirro.h"
#include "jefe.h"
#include "proyectil.h"

namespace
{
const QColor COLOR_SENAL(255, 220, 0, 110);
const QColor COLOR_CHORRO(60, 140, 255, 200);
const QColor COLOR_GAS(160, 160, 160, 120);
const QColor COLOR_OTRO(80, 200, 80, 150);
const double DURACION_ANIM_LANZAR = 0.25;
const double FPS_ANIMACION        = 10.0;
}

vistaN2::vistaN2(QWidget* parent)
    : vistaNivel(parent),
    nivel(), dibujoPersonaje(nullptr), dibujoJefe(nullptr), textoVidas(nullptr),
    ultimaDirX(0), ultimaDirY(1), tiempoAnimacion(0.0), tiempoLanzando(0.0),
    vidasPrevias(nivel.getPersonaje().getVidas()),
    jefeEstabaVivo(nivel.getJefe().estaVivo()),
    fortachonPrevio(nivel.getPersonaje().getFortachon() && nivel.getPersonaje().getTFortachon() > 0.0)
{
    escena->setSceneRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setBackgroundBrush(Qt::black);
    cargarSprites();
    dibujarArena();
    dibujarJefe();
    dibujarPersonaje();
    crearDibujos(nivel.getPersonaje().getEscudos(), dibujoEscudos, spriteEscudo);
    crearDibujos(nivel.getBalas(), dibujoBalas, spriteBala);
    crearEfectos();
    crearBufos();
    crearEsbirros();
    textoVidas = escena->addSimpleText(QString());
    textoVidas->setBrush(Qt::white);
    textoVidas->setPos(10.0, 10.0);
    textoVidas->setZValue(10);
}

QVector<QPixmap> vistaN2::cargarListaFrames(const QString& carpeta, const QVector<int>& indices, double escala)
{
    QVector<QPixmap> out;
    out.reserve(indices.size());
    for (int i = 0; i < indices.size(); ++i) {
        const int idx = indices[i];
        QPixmap p = prepararFrame(QString(":/capitan2/%1/%2.png").arg(carpeta).arg(idx), escala);
        if (!p.isNull())
            out.append(p);
    }
    return out;
}

QVector<QPixmap> vistaN2::voltearListaHorizontal(const QVector<QPixmap>& orig)
{
    QVector<QPixmap> flip;
    flip.reserve(orig.size());
    const QTransform t = QTransform().scale(-1.0, 1.0);
    for (int i = 0; i < orig.size(); ++i) {
        flip.append(orig[i].transformed(t, Qt::SmoothTransformation));
    }
    return flip;
}

void vistaN2::cargarSprites()
{
    // Escala base relativa al diametro del Capitan (radio = 20)
    const double altoCapitan = nivel.getPersonaje().getRadio() * 2.3;
    QPixmap ref = prepararFrame(":/capitan2/abajo/1.png", 1.0);
    const double escalaCap = (!ref.isNull() && ref.height() > 0) ? (altoCapitan / double(ref.height())) : 1.0;

    QVector<int> idxAbajoCaminar;
    idxAbajoCaminar << 2 << 3 << 5 << 8 << 11 << 12;
    QVector<int> idxAbajoLanzar;
    idxAbajoLanzar << 4 << 6 << 7 << 9 << 10 << 13;
    QVector<int> idxAbajoSinEscudo;
    idxAbajoSinEscudo << 14 << 15 << 17;
    QVector<int> idxArribaCaminar;
    idxArribaCaminar << 1 << 2 << 3 << 4 << 5;
    QVector<int> idxDerechaCaminar;
    idxDerechaCaminar << 1 << 2 << 4 << 6 << 8 << 11;
    QVector<int> idxDerechaLanzar;
    idxDerechaLanzar << 5 << 7 << 9 << 10;

    spriteAbajoQuieto    = prepararFrame(":/capitan2/abajo/1.png", escalaCap);
    framesAbajoCaminar   = cargarListaFrames("abajo", idxAbajoCaminar, escalaCap);
    framesAbajoLanzar    = cargarListaFrames("abajo", idxAbajoLanzar, escalaCap);
    framesAbajoSinEscudo = cargarListaFrames("abajo", idxAbajoSinEscudo, escalaCap);

    spriteArribaQuieto  = prepararFrame(":/capitan2/arriba/1.png", escalaCap);
    framesArribaCaminar = cargarListaFrames("arriba", idxArribaCaminar, escalaCap);

    spriteDerechaQuieto  = prepararFrame(":/capitan2/derecha/3.png", escalaCap);
    framesDerechaCaminar = cargarListaFrames("derecha", idxDerechaCaminar, escalaCap);
    framesDerechaLanzar  = cargarListaFrames("derecha", idxDerechaLanzar, escalaCap);

    const QTransform flipH = QTransform().scale(-1.0, 1.0);
    spriteIzquierdaQuieto  = spriteDerechaQuieto.transformed(flipH, Qt::SmoothTransformation);
    framesIzquierdaCaminar = voltearListaHorizontal(framesDerechaCaminar);
    framesIzquierdaLanzar  = voltearListaHorizontal(framesDerechaLanzar);

    // Bufo
    QPixmap bufoRaw = prepararFrame(":/capitan/vida/4.png", 1.0);
    if (!bufoRaw.isNull())
        spriteBufo = bufoRaw.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Escudo en vuelo (radio = 12)
    QPixmap escRaw = prepararFrame(":/capitan2/escudo/1.png", 1.0);
    if (!escRaw.isNull())
        spriteEscudo = escRaw.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Bala enemiga
    QPixmap balaRaw = prepararFrame(":/capitan/danio/1.png", 1.0);
    if (!balaRaw.isNull())
        spriteBala = balaRaw.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Piso, muros, esbirro y jefe
    QPixmap pisoRaw = prepararFrame(":/capitan2/jefe/1.png", 1.0);
    if (!pisoRaw.isNull())
        spritePiso = pisoRaw.scaled(int(datos::LADO_ARENA), int(datos::LADO_ARENA), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    spriteMuroHorizontal = prepararFrame(":/capitan2/jefe/2.png", 1.0);
    spriteMuroVertical   = prepararFrame(":/capitan2/jefe/3.png", 1.0);

    QPixmap esbRaw = prepararFrame(":/capitan2/jefe/4.png", 1.0);
    if (!esbRaw.isNull()) {
        const int dEsb = int(nivel.getEsbirros().empty() ? 28.0 : nivel.getEsbirros().front().getRadio() * 2.2);
        spriteEsbirro = esbRaw.scaled(dEsb, dEsb, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPixmap jefeRaw = prepararFrame(":/capitan2/jefe/5.png", 1.0);
    const obstaculo& c = nivel.getJefe().getCuerpo();
    if (!jefeRaw.isNull()) {
        spriteJefe = jefeRaw.scaled(int(c.getAncho()), int(c.getAlto()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void vistaN2::dibujarArena()
{
    QRectF area;
    const std::vector<obstaculo>& paredes = nivel.getParedes();
    const obstaculo* cuerpoJefe = paredes.empty() ? nullptr : &paredes.back();
    for (size_t i = 0; i < paredes.size(); ++i) {
        const obstaculo& o = paredes[i];
        const QRectF r(o.getIzquierda(), o.getArriba(), o.getAncho(), o.getAlto());
        area = area.united(r);
        if (&o == cuerpoJefe)
            continue;
        const bool esHorizontal = (o.getAncho() >= o.getAlto());
        const QPixmap& baseMuro = esHorizontal ? spriteMuroHorizontal : spriteMuroVertical;
        if (!baseMuro.isNull()) {
            if (esHorizontal) {
                const int altoMuro = std::max(1, int(o.getAlto()));
                QPixmap modulo = baseMuro.scaledToHeight(altoMuro, Qt::SmoothTransformation);
                const int pasoX = std::max(1, modulo.width());
                for (double x = o.getIzquierda(); x < o.getDerecha() - 0.5; x += pasoX) {
                    const int anchoRestante = std::min(pasoX, int(std::ceil(o.getDerecha() - x)));
                    QPixmap pieza = (anchoRestante < modulo.width())
                                        ? modulo.copy(0, 0, anchoRestante, modulo.height())
                                        : modulo;
                    QGraphicsPixmapItem* item = escena->addPixmap(pieza);
                    item->setPos(x, o.getArriba());
                    item->setZValue(1);
                }
            } else {
                const int anchoMuro = std::max(1, int(o.getAncho()));
                QPixmap modulo = baseMuro.scaledToWidth(anchoMuro, Qt::SmoothTransformation);
                const int pasoY = std::max(1, modulo.height());
                for (double y = o.getArriba(); y < o.getAbajo() - 0.5; y += pasoY) {
                    const int altoRestante = std::min(pasoY, int(std::ceil(o.getAbajo() - y)));
                    QPixmap pieza = (altoRestante < modulo.height())
                                        ? modulo.copy(0, 0, modulo.width(), altoRestante)
                                        : modulo;
                    QGraphicsPixmapItem* item = escena->addPixmap(pieza);
                    item->setPos(o.getIzquierda(), y);
                    item->setZValue(1);
                }
            }
        } else {
            escena->addRect(r, QPen(Qt::black), QBrush(QColor(90, 90, 90)))->setZValue(1);
        }
    }

    // Esquinas de la arena (tamano g x g)
    const double g = datos::GROSOR_PARED;
    const QPointF esquinas[4] = {
        QPointF(area.left(), area.top()),
        QPointF(area.right() - g, area.top()),
        QPointF(area.left(), area.bottom() - g),
        QPointF(area.right() - g, area.bottom() - g)
    };
    const double angulosEsquina[4] = { 45.0, -45.0, -45.0, 45.0 };
    const QPixmap& baseEsquina = !spriteMuroHorizontal.isNull() ? spriteMuroHorizontal : spriteMuroVertical;
    for (int i = 0; i < 4; ++i) {
        if (!baseEsquina.isNull()) {
            QPixmap sq = baseEsquina.scaled(int(g), int(g), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QPixmap rot = sq.transformed(QTransform().rotate(angulosEsquina[i]), Qt::SmoothTransformation);
            QGraphicsPixmapItem* esqItem = escena->addPixmap(rot);
            esqItem->setPos(esquinas[i].x() + (g - rot.width()) / 2.0,
                            esquinas[i].y() + (g - rot.height()) / 2.0);
            esqItem->setZValue(2);
        } else {
            escena->addRect(esquinas[i].x(), esquinas[i].y(), g, g,
                            QPen(Qt::black), QBrush(QColor(110, 110, 120)))->setZValue(2);
        }
    }

    if (!spritePiso.isNull()) {
        QGraphicsPixmapItem* pisoItem = escena->addPixmap(spritePiso);
        pisoItem->setPos(area.left(), area.top());
        pisoItem->setZValue(-10);
    } else {
        QGraphicsRectItem* piso = escena->addRect(area, QPen(Qt::NoPen), QBrush(QColor(55, 60, 75)));
        piso->setZValue(-10);
    }
}

void vistaN2::dibujarPersonaje()
{
    const QPixmap& inicial = spriteAbajoQuieto.isNull() ? QPixmap() : spriteAbajoQuieto;
    dibujoPersonaje = escena->addPixmap(inicial);
    if (!inicial.isNull())
        dibujoPersonaje->setOffset(-inicial.width() / 2.0, -inicial.height() / 2.0);
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujoPersonaje->setZValue(5);
}

void vistaN2::dibujarJefe()
{
    const obstaculo& c = nivel.getJefe().getCuerpo();
    if (!spriteJefe.isNull()) {
        dibujoJefe = escena->addPixmap(spriteJefe);
        dibujoJefe->setPos(c.getIzquierda() + (c.getAncho() - spriteJefe.width()) / 2.0,
                           c.getArriba() + (c.getAlto() - spriteJefe.height()) / 2.0);
    } else {
        dibujoJefe = escena->addPixmap(QPixmap());
        dibujoJefe->setPos(c.getIzquierda(), c.getArriba());
    }
    dibujoJefe->setZValue(3);
}

void vistaN2::crearEfectos()
{
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
        if (!e.getactivo() || e.gettipo() == efecto::bufo) {
            it->setVisible(false);
            continue;
        }
        it->setVisible(true);
        it->setRect(e.getIzquierda(), e.getArriba(), e.getAncho(), e.getAlto());
        switch (e.gettipo()) {
        case efecto::senal: it->setBrush(COLOR_SENAL);  it->setZValue(6); break;
        case efecto::danio: it->setBrush(COLOR_CHORRO); it->setZValue(2); break;
        case efecto::gas:   it->setBrush(COLOR_GAS);    it->setZValue(2); break;
        default:            it->setBrush(COLOR_OTRO);   it->setZValue(2); break;
        }
    }
}

void vistaN2::crearBufos()
{
    const int n = int(nivel.getEfectos().size());
    dibujoBufos.reserve(n);
    for (int i = 0; i < n; ++i) {
        QGraphicsPixmapItem* it = escena->addPixmap(spriteBufo);
        it->setZValue(4);
        it->setVisible(false);
        dibujoBufos.append(it);
    }
}

void vistaN2::dibujarBufos()
{
    const std::vector<efecto>& efectos = nivel.getEfectos();
    for (int i = 0; i < int(efectos.size()); ++i) {
        const efecto& e = efectos[i];
        QGraphicsPixmapItem* it = dibujoBufos[i];
        const bool mostrarBufo = e.getactivo() && (e.gettipo() == efecto::bufo);
        it->setVisible(mostrarBufo);
        if (mostrarBufo) {
            if (!spriteBufo.isNull()) {
                const int w = std::max(1, int(e.getAncho()));
                const int h = std::max(1, int(e.getAlto()));
                QPixmap escalado = spriteBufo.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                it->setPixmap(escalado);
                it->setPos(e.getIzquierda() + (e.getAncho() - escalado.width()) / 2.0,
                           e.getArriba() + (e.getAlto() - escalado.height()) / 2.0);
            } else {
                it->setPos(e.getIzquierda(), e.getArriba());
            }
        }
    }
}

void vistaN2::crearEsbirros()
{
    const std::vector<esbirro>& esbirros = nivel.getEsbirros();
    dibujoEsbirros.reserve(int(esbirros.size()));
    for (size_t i = 0; i < esbirros.size(); ++i)
    {
        QGraphicsPixmapItem* it = escena->addPixmap(spriteEsbirro);
        if (!spriteEsbirro.isNull())
            it->setOffset(-spriteEsbirro.width() / 2.0, -spriteEsbirro.height() / 2.0);
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
        const bool vivo = esbirros[i].estaVivo();
        dibujoEsbirros[i]->setVisible(vivo);
        if (vivo)
            dibujoEsbirros[i]->setPos(esbirros[i].getPosicion());
    }
}

const QPixmap& vistaN2::elegirFrameCapitan() const
{
    const int dx = nivel.getPersonaje().getDireccionX();
    const int dy = nivel.getPersonaje().getDireccionY();
    const bool moviendose = (dx != 0 || dy != 0);
    bool escudoEnAire = false;
    const std::vector<proyectil>& escudos = nivel.getPersonaje().getEscudos();
    for (size_t i = 0; i < escudos.size(); ++i) {
        if (escudos[i].estaActivo()) {
            escudoEnAire = true;
            break;
        }
    }
    const int dirX = moviendose ? dx : ultimaDirX;
    const int dirY = moviendose ? dy : ultimaDirY;
    const bool horizontal = (std::abs(dirX) > std::abs(dirY));
    const int step = int(tiempoAnimacion * FPS_ANIMACION);
    if (tiempoLanzando > 0.0) {
        if (horizontal) {
            const QVector<QPixmap>& v = (dirX < 0) ? framesIzquierdaLanzar : framesDerechaLanzar;
            if (!v.isEmpty())
                return v[step % v.size()];
        } else if (!framesAbajoLanzar.isEmpty()) {
            return framesAbajoLanzar[step % framesAbajoLanzar.size()];
        }
    }
    if (horizontal) {
        if (dirX < 0) {
            if (moviendose && !framesIzquierdaCaminar.isEmpty())
                return framesIzquierdaCaminar[step % framesIzquierdaCaminar.size()];
            return spriteIzquierdaQuieto;
        } else {
            if (moviendose && !framesDerechaCaminar.isEmpty())
                return framesDerechaCaminar[step % framesDerechaCaminar.size()];
            return spriteDerechaQuieto;
        }
    }
    if (dirY < 0) {
        if (moviendose && !framesArribaCaminar.isEmpty())
            return framesArribaCaminar[step % framesArribaCaminar.size()];
        return spriteArribaQuieto;
    }
    if (escudoEnAire && !framesAbajoSinEscudo.isEmpty())
        return framesAbajoSinEscudo[step % framesAbajoSinEscudo.size()];
    if (moviendose && !framesAbajoCaminar.isEmpty())
        return framesAbajoCaminar[step % framesAbajoCaminar.size()];
    return spriteAbajoQuieto;
}

void vistaN2::avanzarLogica(double dt)
{
    tiempoAnimacion += dt;
    if (tiempoLanzando > 0.0)
        tiempoLanzando = std::max(0.0, tiempoLanzando - dt);
    const int dx = nivel.getPersonaje().getDireccionX();
    const int dy = nivel.getPersonaje().getDireccionY();
    if (dx != 0 || dy != 0) {
        ultimaDirX = dx;
        ultimaDirY = dy;
    }
    nivel.actualizar(dt);
    const int vidasActuales = nivel.getPersonaje().getVidas();
    if (vidasActuales < vidasPrevias) {
        if (vidasActuales <= 0)
            reproducirMuerte();
        else
            reproducirDanio();
    }
    const capitanN2& cap = nivel.getPersonaje();
    const bool fortachonAhora = cap.getFortachon() && cap.getTFortachon() > 0.0;
    if (fortachonAhora && !fortachonPrevio) {
        reproducirBufo();
    }
    fortachonPrevio = fortachonAhora;
    vidasPrevias = vidasActuales;
    const bool jefeVivo = nivel.getJefe().estaVivo();
    if (jefeEstabaVivo && !jefeVivo) {
        jefeEstabaVivo = false;
        reproducirVictoria();
        terminar(true);
        return;
    }
    if (!nivel.getPersonaje().estaVivo()) {
        reproducirGameOver();
        terminar(false);
    }
}

void vistaN2::dibujar(double)
{
    const capitanN2& cap = nivel.getPersonaje();
    const QPixmap& frame = elegirFrameCapitan();
    if (!frame.isNull()) {
        dibujoPersonaje->setPixmap(frame);
        dibujoPersonaje->setOffset(-frame.width() / 2.0, -frame.height() / 2.0);
    }
    dibujoPersonaje->setPos(cap.getPosicion());

    // Parpadeo durante invulnerabilidad
    if (cap.getT_invulnerable() > 0.0)
        dibujoPersonaje->setVisible((int(cap.getT_invulnerable() * 10.0) % 2) == 0);
    else
        dibujoPersonaje->setVisible(true);

    dibujarPool(cap.getEscudos(), dibujoEscudos, true);
    dibujarPool(nivel.getBalas(), dibujoBalas, false);
    dibujarEfectos();
    dibujarBufos();
    dibujarEsbirros();

    const bool curando = nivel.getJefe().estaCurando();
    dibujoJefe->setOpacity(curando ? 0.65 : 1.0);

    const double tFort = cap.getTFortachon();
    const bool fortachonActivo = cap.getFortachon() && tFort > 0.0;

    textoVidas->setText(QString("Vidas: %1     Jefe: %2 / %3%4%5")
                            .arg(cap.getVidas())
                            .arg(nivel.getJefe().getVida())
                            .arg(nivel.getJefe().getVidaMax())
                            .arg(curando ? "   CURANDO" : "")
                            .arg(fortachonActivo
                                     ? QString("   FORTACHON (%1s)").arg(tFort, 0, 'f', 1)
                                     : QString()));
}

void vistaN2::actualizarDireccion()
{
    nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0), (teclaS ? 1 : 0) - (teclaW ? 1 : 0));
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
    if (e->button() == Qt::LeftButton) {
        if (nivel.lanzarEscudo(mapToScene(e->pos())))
            tiempoLanzando = DURACION_ANIM_LANZAR;
    } else {
        vistaNivel::mousePressEvent(e);
    }
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