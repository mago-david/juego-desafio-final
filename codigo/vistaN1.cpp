#include "vistaN1.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <algorithm>
#include <cmath>
namespace {
QString formatearMMSS(double segundos)
{
    const int totalSeg = std::max(0, int(std::ceil(segundos)));
    const int minutos  = totalSeg / 60;
    const int segs     = totalSeg % 60;
    return QString("%1:%2")
        .arg(minutos, 2, 10, QChar('0'))
        .arg(segs, 2, 10, QChar('0'));
}
}

vistaN1::vistaN1(QWidget* parent)
    : vistaNivel(parent),
    nivel(), dibujoPersonaje(nullptr), dibujoSprite(nullptr),
    efectoDanio(nullptr), teclaA(false), teclaD(false),
    estadoAnim(QUIETO), tAnim(0.0), mirandoIzq(false),tiempoRestante(70.0),
    muertePorTiempoEjecutada(false), cronometroActivo(false),fondoCronometro(nullptr),
    textoTiempoRestante(nullptr), dibujoMuro(nullptr)
{
    escena->setSceneRect(0.0, 0.0, nivel.getAnchoMapa(), datos::ALTO_ESCENA);
    cargarSprites();
    dibujarFondo();
    dibujarMapa();
    dibujarPersonaje();
    dibujarHUD(nivel.getPersonaje().getVidasMax());

    dibujoMuro = escena->addRect(0.0, 0.0, 1.0, datos::ALTO_ESCENA,
                                 QPen(Qt::NoPen), QBrush(Qt::black));
    dibujoMuro->setZValue(15);

    // Proyectiles
    QPixmap spriteBala = prepararFrame(":/capitan/danio/1.png", 1.0);
    if (!spriteBala.isNull()) {
        spriteBala = spriteBala.scaledToHeight(12, Qt::SmoothTransformation)
        .transformed(QTransform().scale(-1, 1));
    }

    QPixmap spriteLacri = prepararFrame(":/capitan/danio/2.png", 1.0);
    if (!spriteLacri.isNull()) {
        spriteLacri = spriteLacri.scaledToHeight(20, Qt::SmoothTransformation)
        .transformed(QTransform().scale(-1, 1));
    }

    crearDibujos(nivel.getBalas(), dibujoBalas, spriteBala);
    crearDibujos(nivel.getLacrimogenas(), dibujoLacrimogenas, spriteLacri);

    // Animación del gas
    QPixmap refGas = prepararFrame(":/capitan/danio/5.png", 1.0);
    const double escalaGas = refGas.isNull() ? 1.0 : 100.0 / refGas.height();
    framesGas = cargarFrames("danio", 3, 7, escalaGas);
    crearNubes();
    crearBufos();
    //CRONO
    fondoCronometro = escena->addRect(0.0, 0.0, 108.0, 34.0,
                                      QPen(QColor(255, 255, 255, 90), 1.5),
                                      QBrush(QColor(15, 18, 28, 200)));
    fondoCronometro->setZValue(99);

    QFont fuenteCrono("Consolas", 14, QFont::Bold);
    textoTiempoRestante = escena->addSimpleText(QStringLiteral("01:10"), fuenteCrono);
    textoTiempoRestante->setBrush(Qt::white);
    textoTiempoRestante->setZValue(100);
}


void vistaN1::agregarCapa(const QString& ruta, double factor, int z)
{
    QPixmap img(ruta);
    if (img.isNull())
        return;
    img = img.scaledToHeight(int(datos::ALTO_ESCENA), Qt::SmoothTransformation);

    const double recorrido = (nivel.getAnchoMapa() - datos::ANCHO_ESCENA) * factor;
    const double cubrir = datos::ANCHO_ESCENA + recorrido;

    QGraphicsPixmapItem* base = escena->addPixmap(QPixmap());
    base->setZValue(z);
    for (int x = 0; x < int(cubrir) + img.width(); x += img.width()) {
        QGraphicsPixmapItem* trozo = new QGraphicsPixmapItem(img, base);
        trozo->setPos(x, 0.0);
    }
    capasItem.append(base);
    capasFactor.append(factor);
}

void vistaN1::actualizarFondo(const QPointF& pos)
{
    double izq = pos.x() - datos::ANCHO_ESCENA / 2.0;
    if (izq < 0.0)
        izq = 0.0;
    const double maxIzq = nivel.getAnchoMapa() - datos::ANCHO_ESCENA;
    if (izq > maxIzq)
        izq = maxIzq;
    for (int i = 0; i < capasItem.size(); ++i)
        capasItem[i]->setPos(izq * (1.0 - capasFactor[i]), 0.0);
}

void vistaN1::dibujarFondo()
{
    agregarCapa(":/capitan/fondo/1.png",  0.2, -30);
    agregarCapa(":/capitan/medio/2.png",  0.5, -20);
    agregarCapa(":/capitan/frente/3.png", 0.8, -10);
}

void vistaN1::dibujarMapa()
{
    QPixmap tex[9];
    for (int id = 1; id <= 8; ++id)
        tex[id] = prepararFrame(QString(":/capitan/obstaculos/%1.png").arg(id), 1.0);

    for (const obstaculo& o : nivel.getPlataformas()) {
        const int id = o.getSpriteId();
        if (id <= 0 || id > 8 || tex[id].isNull())
            continue;
        if (id == 7 && o.getAncho() > o.getAlto() * 2)
            continue;

        QPixmap esc = tex[id].scaled(int(o.getAncho()), int(o.getAlto()),
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QGraphicsPixmapItem* item = escena->addPixmap(esc);
        item->setPos(o.getIzquierda(), o.getArriba());
        item->setZValue(o.getAtravesable() ? 6 : 8);
    }

    if (!tex[8].isNull()) {
        for (const efecto& m : nivel.getManifestantes()) {
            QPixmap esc = tex[8].scaled(int(m.getAncho()), int(m.getAlto()),
                                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QGraphicsPixmapItem* item = escena->addPixmap(esc);
            item->setPos(m.getIzquierda(), m.getArriba());
            item->setZValue(9);
        }
    }
}

void vistaN1::dibujarPersonaje()
{
    const double r = nivel.getPersonaje().getRadio();
    dibujoPersonaje = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r, QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    dibujoPersonaje->setZValue(5);

    dibujoSprite = new QGraphicsPixmapItem(dibujoPersonaje);
    efectoDanio = new QGraphicsColorizeEffect();
    efectoDanio->setColor(Qt::red);
    efectoDanio->setStrength(0.8);
    efectoDanio->setEnabled(false);
    dibujoSprite->setGraphicsEffect(efectoDanio);
    actualizarSprite();
}

void vistaN1::cargarSprites()
{
    QPixmap ref = prepararFrame(":/capitan/quieto/1.png", 1.0);
    const double escala = ref.isNull() ? 1.0 : 64.0 / ref.height();

    animaciones[QUIETO]  = cargarFrames("quieto",  1,  5,  escala);
    animaciones[AGACHAR] = cargarFrames("agachar", 6,  8,  escala);
    animaciones[CAMINAR] = cargarFrames("caminar", 9,  13, escala);
    animaciones[CORRER]  = cargarFrames("correr",  14, 18, escala);
    animaciones[SALTO]   = cargarFrames("salto",   19, 21, escala);

    for (int e = 0; e < N_ESTADOS; ++e) {
        animacionesIzq[e].reserve(animaciones[e].size());
        for (const QPixmap& f : animaciones[e])
            animacionesIzq[e].append(f.transformed(QTransform().scale(-1, 1)));
    }
}

void vistaN1::actualizarSprite()
{
    const capitanN1& c = nivel.getPersonaje();
    const double vx = std::abs(c.getVelocidad().x());

    Estado nuevo;
    if (!c.getEnSuelo())
        nuevo = SALTO;
    else if (c.getT_sinPlataformas() > 0.0)
        nuevo = AGACHAR;
    else if (vx > 150.0)
        nuevo = CORRER;
    else if (vx > 20.0)
        nuevo = CAMINAR;
    else
        nuevo = QUIETO;

    if (c.getDireccion() != 0)
        mirandoIzq = (c.getDireccion() < 0);

    if (nuevo != estadoAnim) {
        estadoAnim = nuevo;
        tAnim = 0.0;
    }
    tAnim += datos::DT;

    const QVector<QPixmap>& frames = mirandoIzq ? animacionesIzq[estadoAnim] : animaciones[estadoAnim];
    if (frames.isEmpty() || !dibujoSprite)
        return;

    int idx;
    if (estadoAnim == SALTO) {
        const double vy = c.getVelocidad().y();
        idx = (vy < -100.0) ? 0 : (vy > 100.0 ? 2 : 1);
        idx = qMin(idx, int(frames.size()) - 1);
    } else {
        idx = int(tAnim * 10.0) % frames.size();
    }

    const QPixmap& p = frames[idx];
    dibujoSprite->setPixmap(p);
    dibujoSprite->setPos(-p.width() / 2.0, c.getRadio() - p.height());

    const double inv = c.getT_invulnerable();
    if (inv > 0.0) {
        dibujoSprite->setVisible(int(inv * 10.0) % 2 == 0);
        efectoDanio->setEnabled(inv > 1.2);
    } else {
        dibujoSprite->setVisible(true);
        efectoDanio->setEnabled(false);
    }
}

void vistaN1::avanzarLogica(double dt)

{
    const int vidasAntes = nivel.getPersonaje().getVidas();
    nivel.actualizar(dt);
    const int vidasAhora = nivel.getPersonaje().getVidas();

    if (!nivel.getPersonaje().estaVivo()) {
        actualizarHUD(vidasAhora);
        reproducirMuerte();
        terminar(false);
        return;
    }
    if (vidasAhora < vidasAntes) {
        reproducirDanio();
    }
    if (nivel.getPersonaje().getPosicion().x() >= nivel.getAnchoMapa() - 150.0) {
        terminar(true);
        return;
    }
    if (!cronometroActivo && (teclaD || nivel.getPersonaje().getPosicion().x() > 100.5)) {
        cronometroActivo = true;
    }

    if (cronometroActivo && !muertePorTiempoEjecutada) {
        tiempoRestante = std::max(0.0, tiempoRestante - dt);

        if (tiempoRestante <= 0.0) {
            muertePorTiempoEjecutada = true;
            actualizarHUD(0);
            reproducirMuerte();
            terminar(false);
            return;
        }
    }

    if (nivel.getPersonaje().getPosicion().x() >= nivel.getAnchoMapa() - 150.0) {
        terminar(true);
        return;
    }
}

void vistaN1::dibujar(double a)
{
    const capitanN1& c = nivel.getPersonaje();
    const QPointF pos = c.getPosicionAnterior() + (c.getPosicion() - c.getPosicionAnterior()) * a;

    const double maxCamIzq = nivel.getAnchoMapa() - datos::ANCHO_ESCENA;
    double camIzq = pos.x() - datos::ANCHO_ESCENA / 2.0;
    if (camIzq < 0.0) {
        camIzq = 0.0;
    } else if (camIzq > maxCamIzq) {
        camIzq = maxCamIzq;
    }
    const double centroCamX = camIzq + datos::ANCHO_ESCENA / 2.0;

    dibujoPersonaje->setPos(pos);
    actualizarSprite();
    actualizarFondo(QPointF(centroCamX, pos.y()));
    dibujarPool(nivel.getBalas(), dibujoBalas, false);
    dibujarPool(nivel.getLacrimogenas(), dibujoLacrimogenas, true);
    actualizarNubes();
    actualizarBufos();

    const double derMuro = nivel.getMuro().getDerecha();
    const double izqMuro = camIzq;
    if (dibujoMuro) {
        if (derMuro > izqMuro) {
            dibujoMuro->setVisible(true);
            dibujoMuro->setRect(0.0, 0.0, derMuro - izqMuro, datos::ALTO_ESCENA);
            dibujoMuro->setPos(izqMuro, 0.0);
        } else {
            dibujoMuro->setVisible(false);
        }
    }

    centerOn(centroCamX, datos::ALTO_ESCENA / 2.0);
    actualizarHUD(nivel.getPersonaje().getVidas());
    const QRectF vistaVisible = mapToScene(viewport()->rect()).boundingRect();
    const double xCentro = vistaVisible.left() + vistaVisible.width() * 0.5;
    const double yArriba = vistaVisible.top() + 10.0;

    if (fondoCronometro) {
        fondoCronometro->setPos(xCentro - 54.0, yArriba);
    }

    if (textoTiempoRestante) {
        textoTiempoRestante->setText(formatearMMSS(tiempoRestante));
        const double anchoTexto = textoTiempoRestante->boundingRect().width();
        textoTiempoRestante->setPos(xCentro - anchoTexto * 0.5, yArriba + 6.0);

        if (tiempoRestante <= 10.0) {
            const bool parpadeoAlerta = (int(tiempoRestante * 6.0) % 2) == 0;
            textoTiempoRestante->setBrush(parpadeoAlerta ? QColor(255, 65, 65) : QColor(255, 190, 60));
        } else {
            textoTiempoRestante->setBrush(Qt::white);
        }
    }
}

void vistaN1::actualizarDireccion()
{
    nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0));
}

void vistaN1::alPulsar(int tecla)
{
    switch (tecla) {
    case Qt::Key_A:
        teclaA = true;
        actualizarDireccion();
        break;
    case Qt::Key_D:
        teclaD = true;
        actualizarDireccion();
        break;
    case Qt::Key_Space:
        nivel.saltar();
        break;
    case Qt::Key_Shift:
        nivel.bajarPlataforma();
        break;
    default:
        break;
    }
}

void vistaN1::alSoltar(int tecla)
{
    switch (tecla) {
    case Qt::Key_A:
        teclaA = false;
        actualizarDireccion();
        break;
    case Qt::Key_D:
        teclaD = false;
        actualizarDireccion();
        break;
    default:
        break;
    }
}

void vistaN1::alPerderFoco()
{
    teclaA = false;
    teclaD = false;
    actualizarDireccion();
}

const nivel1& vistaN1::getNivel() const
{ return nivel; }

bool vistaN1::getTeclaA() const
{ return teclaA; }

bool vistaN1::getTeclaD() const
{ return teclaD; }

void vistaN1::crearNubes()
{
    const std::vector<efecto>& nubes = nivel.getNubes();
    dibujoNubes.reserve(int(nubes.size()));
    for (size_t i = 0; i < nubes.size(); ++i)
    {
        QGraphicsPixmapItem* it = escena->addPixmap(QPixmap());
        it->setZValue(6);
        it->setVisible(false);
        dibujoNubes.append(it);
    }
}

void vistaN1::actualizarNubes()
{
    if (framesGas.isEmpty())
        return;
    const std::vector<efecto>& nubes = nivel.getNubes();
    const double duracionTotal = 3.0;
    for (int i = 0; i < int(nubes.size()); ++i)
    {
        const bool activo = nubes[i].getactivo();
        dibujoNubes[i]->setVisible(activo);
        if (!activo)
            continue;
        double progreso = 1.0 - (nubes[i].getduracion() / duracionTotal);
        progreso = std::max(0.0, std::min(0.999, progreso));
        int idx = int(progreso * framesGas.size());
        const QPixmap& frame = framesGas[idx];
        dibujoNubes[i]->setPixmap(frame);
        double centroX = nubes[i].getIzquierda() + nubes[i].getAncho() / 2.0;
        double centroY = nubes[i].getArriba() + nubes[i].getAlto() / 2.0;
        dibujoNubes[i]->setPos(centroX - frame.width() / 2.0,
                               centroY - frame.height() / 2.0);
    }
}

void vistaN1::crearBufos()
{
    const std::vector<efecto>& bufos = nivel.getBufos();
    dibujoBufos.reserve(int(bufos.size()));
    for (const efecto& b : bufos)
    {
        QGraphicsPixmapItem* it = escena->addPixmap(corazonExtra);
        it->setZValue(4);
        double cx = b.getIzquierda() + b.getAncho() / 2.0;
        double cy = b.getArriba() + b.getAlto() / 2.0;
        it->setPos(cx - corazonExtra.width() / 2.0, cy - corazonExtra.height() / 2.0);
        it->setVisible(b.getactivo());
        dibujoBufos.append(it);
    }
}

void vistaN1::actualizarBufos()
{
    const std::vector<efecto>& bufos = nivel.getBufos();
    for (int i = 0; i < int(bufos.size()); ++i) {
        if (dibujoBufos[i] && !bufos[i].getactivo()) {
            reproducirBufo();
            escena->removeItem(dibujoBufos[i]);
            delete dibujoBufos[i];
            dibujoBufos[i] = nullptr;
        }
    }
}
