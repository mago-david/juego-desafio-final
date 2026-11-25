#include "vistaN2.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QPainter>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <algorithm>
#include <cmath>
#include "efecto.h"
#include "esbirro.h"
#include "jefe.h"
#include "proyectil.h"
#include <QGraphicsColorizeEffect>

namespace
{
const QColor COLOR_SENAL(255, 220, 0, 110);
const QColor COLOR_OTRO(80, 200, 80, 150);
const double DURACION_ANIM_LANZAR = 0.25;
const double FPS_ANIMACION        = 10.0;
}

vistaN2::vistaN2(QWidget* parent)
    : vistaNivel(parent),
    nivel(), dibujoPersonaje(nullptr), dibujoJefe(nullptr),barraJefeFondo(nullptr),barraJefeRelleno(nullptr),
    barraJefeBorde(nullptr),efectoCuracionJefe(nullptr),ultimaDirX(0), ultimaDirY(1), tiempoAnimacion(0.0),tiempoLanzando(0.0),
    vidasPrevias(nivel.getPersonaje().getVidas()),
    jefeEstabaVivo(nivel.getJefe().estaVivo()),vidaJefePrevia(nivel.getJefe().getVida()),
    fortachonPrevio(nivel.getPersonaje().getFortachon() && nivel.getPersonaje().getTFortachon() > 0.0),
    teclaW(false), teclaA(false), teclaS(false), teclaD(false)
{
    setUltimoNivel(true);
    escena->setSceneRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    setBackgroundBrush(Qt::black);
    cargarSprites();
    dibujarArena();
    dibujarJefe();
    dibujarPersonaje();
    dibujarHUD(nivel.getPersonaje().getVidasMax());
    crearBarraJefe();
    crearDibujos(nivel.getPersonaje().getEscudos(), dibujoEscudos, spriteEscudo);
    crearDibujos(nivel.getBalas(), dibujoBalas, spriteBala);
    crearEfectos();
    crearBufos();
    crearHumos();
    crearAguas();
    crearEsbirros();

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
    // Escala base relativa al diámetro del Capitán (radio = 20)
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

    // Humo
    const double altoHumo = 64.0;
    QPixmap refHumo = prepararFrame(":/capitan/danio/3.png", 1.0);
    const double escalaHumo = (!refHumo.isNull() && refHumo.height() > 0)
                                  ? (altoHumo / double(refHumo.height()))
                                  : 1.0;
    for (int idx = 3; idx <= 7; ++idx) {
        QPixmap p = prepararFrame(QString(":/capitan/danio/%1.png").arg(idx), escalaHumo);
        if (!p.isNull())
            framesHumo.append(p);
    }

    // Chorro de agua
    for (int idx = 6; idx <= 8; ++idx) {
        QPixmap p = prepararFrame(QString(":/capitan2/jefe/%1.png").arg(idx), 1.0);
        if (!p.isNull())
            framesAgua.append(p);
    }

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

    // Esquinas de la arena (tamaño g x g)
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
    efectoCuracionJefe = new QGraphicsColorizeEffect(this);
    efectoCuracionJefe->setColor(QColor(70, 220, 110));
    efectoCuracionJefe->setStrength(0.75);
    efectoCuracionJefe->setEnabled(false);
    if (dibujoJefe)
        dibujoJefe->setGraphicsEffect(efectoCuracionJefe);
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
        if (!e.getactivo() || e.gettipo() == efecto::bufo || e.gettipo() == efecto::gas || e.gettipo() == efecto::danio) {
            it->setVisible(false);
            continue;
        }
        it->setVisible(true);
        it->setRect(e.getIzquierda(), e.getArriba(), e.getAncho(), e.getAlto());
        switch (e.gettipo()) {
        case efecto::senal: it->setBrush(COLOR_SENAL); it->setZValue(6); break;
        default:            it->setBrush(COLOR_OTRO);  it->setZValue(2); break;
        }
    }
}
void vistaN2::crearBarraJefe()
{
    const double anchoBarra = 360.0;
    const double altoBarra  = 18.0;
    const double xBarra     = (datos::ANCHO_ESCENA - anchoBarra) / 2.0;
    const double yBarra     = 14.0;

    barraJefeFondo = escena->addRect(xBarra, yBarra, anchoBarra, altoBarra,
                                     QPen(Qt::NoPen), QBrush(QColor(35, 35, 45, 220)));
    barraJefeFondo->setZValue(100);

    barraJefeRelleno = escena->addRect(xBarra, yBarra, anchoBarra, altoBarra,
                                       QPen(Qt::NoPen), QBrush(QColor(210, 45, 45)));
    barraJefeRelleno->setZValue(101);

    barraJefeBorde = escena->addRect(xBarra, yBarra, anchoBarra, altoBarra,
                                     QPen(Qt::white, 2.0), QBrush(Qt::NoBrush));
    barraJefeBorde->setZValue(102);
}
void vistaN2::actualizarBarraJefe()
{
    if (!barraJefeRelleno || !barraJefeBorde)
        return;
    const jefe& j = nivel.getJefe();
    const double vidaActual = std::max(0.0, double(j.getVida()));
    const double vidaMax    = std::max(1.0, double(j.getVidaMax()));
    const double proporcion = std::clamp(vidaActual / vidaMax, 0.0, 1.0);
    const QRectF rectBase = barraJefeBorde->rect();
    barraJefeRelleno->setRect(rectBase.x(), rectBase.y(),
                              rectBase.width() * proporcion, rectBase.height());
    const bool curando = j.estaCurando();
    barraJefeRelleno->setBrush(curando ? QBrush(QColor(70, 200, 110))
                                       : QBrush(QColor(210, 45, 45)));
    if (efectoCuracionJefe && dibujoJefe) {
        if (curando) {
            const bool pulsoVerde = (int(tiempoAnimacion * 12.0) % 2) == 0;
            efectoCuracionJefe->setEnabled(pulsoVerde);
        } else {
            efectoCuracionJefe->setEnabled(false);
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

void vistaN2::crearHumos()
{
    const int n = int(nivel.getEfectos().size());
    dibujoHumos.reserve(n);
    for (int i = 0; i < n; ++i) {
        QGraphicsPixmapItem* it = escena->addPixmap(QPixmap());
        it->setZValue(3);
        it->setVisible(false);
        dibujoHumos.append(it);
    }
}

void vistaN2::dibujarHumos()
{
    const std::vector<efecto>& efectos = nivel.getEfectos();
    const int step = int(tiempoAnimacion * FPS_ANIMACION);
    for (int i = 0; i < int(efectos.size()); ++i) {
        const efecto& e = efectos[i];
        QGraphicsPixmapItem* it = dibujoHumos[i];
        const bool mostrarHumo = e.getactivo() && (e.gettipo() == efecto::gas);
        it->setVisible(mostrarHumo);
        if (mostrarHumo && !framesHumo.isEmpty()) {
            const QSize tamActual(std::max(1, int(e.getAncho())), std::max(1, int(e.getAlto())));
            if (tamCacheHumo != tamActual || cacheHumos.size() != framesHumo.size()) {
                tamCacheHumo = tamActual;
                cacheHumos.clear();
                cacheHumos.reserve(framesHumo.size());
                for (const QPixmap& base : framesHumo) {
                    cacheHumos.append(base.scaled(tamActual, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                }
            }
            const QPixmap& escalado = cacheHumos[step % cacheHumos.size()];
            if (!escalado.isNull()) {
                it->setOffset(-escalado.width() / 2.0, -escalado.height() / 2.0);
                it->setPixmap(escalado);
                it->setPos(e.getIzquierda() + e.getAncho() / 2.0,
                           e.getArriba() + e.getAlto() / 2.0);
            }
        }
    }
}

void vistaN2::crearAguas()
{
    const int n = int(nivel.getEfectos().size());
    dibujoAguas.reserve(n);
    for (int i = 0; i < n; ++i) {
        QGraphicsPixmapItem* it = escena->addPixmap(QPixmap());
        it->setZValue(2);
        it->setVisible(false);
        dibujoAguas.append(it);
    }
}

void vistaN2::dibujarAguas()
{
    const std::vector<efecto>& efectos = nivel.getEfectos();
    const int step = int(tiempoAnimacion * FPS_ANIMACION);
    for (int i = 0; i < int(efectos.size()); ++i) {
        const efecto& e = efectos[i];
        QGraphicsPixmapItem* it = dibujoAguas[i];
        const bool mostrarAgua = e.getactivo() && (e.gettipo() == efecto::danio);
        it->setVisible(mostrarAgua);
        if (mostrarAgua && !framesAgua.isEmpty()) {
            const int w = std::max(1, int(e.getAncho()));
            const int h = std::max(1, int(e.getAlto()));
            const QSize tamActual(w, h);

            if (tamCacheAgua != tamActual || cacheAguas.size() != framesAgua.size()) {
                tamCacheAgua = tamActual;
                cacheAguas.clear();
                cacheAguas.reserve(framesAgua.size());
                const bool ataqueHorizontal = (w > h);

                for (const QPixmap& base : framesAgua) {
                    QPixmap tramo = base;
                    if (!ataqueHorizontal) {
                        tramo = tramo.transformed(QTransform().rotate(90.0), Qt::SmoothTransformation);
                        tramo = tramo.scaledToWidth(w, Qt::SmoothTransformation);
                    } else {
                        tramo = tramo.scaledToHeight(h, Qt::SmoothTransformation);
                    }
                    QPixmap completo(w, h);
                    completo.fill(Qt::transparent);
                    QPainter painter(&completo);
                    painter.drawTiledPixmap(0, 0, w, h, tramo);
                    painter.end();
                    cacheAguas.append(completo);
                }
            }

            const QPixmap& listo = cacheAguas[step % cacheAguas.size()];
            if (!listo.isNull()) {
                it->setPixmap(listo);
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

    const capitanN2& cap = nivel.getPersonaje();
    const int vidasActuales = cap.getVidas();
    if (vidasActuales < vidasPrevias) {
        if (vidasActuales <= 0)
            reproducirMuerte();
        else
            reproducirDanio();
    }
    vidasPrevias = vidasActuales;

    const bool fortachonAhora = cap.getFortachon() && cap.getTFortachon() > 0.0;
    if (fortachonAhora && !fortachonPrevio) {
        reproducirBufo();
    }
    fortachonPrevio = fortachonAhora;

    const bool jefeVivo = nivel.getJefe().estaVivo();
    if (jefeEstabaVivo && !jefeVivo) {
        jefeEstabaVivo = false;
        terminar(true);
        return;
    }
    if (!cap.estaVivo()) {
        terminar(false);
    }
    const int vidaJefeActual = nivel.getJefe().getVida();
    if (vidaJefeActual < vidaJefePrevia) {
        reproducirGolpeJefe();
    }
    vidaJefePrevia = vidaJefeActual;
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
    dibujarHumos();
    dibujarAguas();
    dibujarEsbirros();

    const bool curando = nivel.getJefe().estaCurando();
    dibujoJefe->setOpacity(curando ? 0.65 : 1.0);

    actualizarHUD(nivel.getPersonaje().getVidas());
    actualizarBarraJefe();
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