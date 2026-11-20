#include "vistaN1.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>
#include <QImage>// recortar las imagenes
#include <QTransform> // transformaciones; scale(-1, 1) voltea el sprite al mirar a la izquierda
#include <QBrush>
#include <QPen> // borde de las figuras (Qt::NoPen = sin borde)
#include <QDebug>
#include <cmath>
using namespace std;
vistaN1::vistaN1(QWidget* parent)
    : vistaNivel(parent),
    nivel(),dibujoPersonaje(nullptr),dibujoSprite(nullptr),
    efectoDanio(nullptr),teclaA(false),teclaD(false),
    estadoAnim(QUIETO),tAnim(0.0),mirandoIzq(false)
{
    escena->setSceneRect(0.0, 0.0, nivel.getAnchoMapa(), datos::ALTO_ESCENA);
    cargarSprites();
    dibujarFondo();
    dibujarMapa();
    dibujarPersonaje();
    //
    dibujarHUD();
    //proyectiles
    //  voltear disparo hacia la izquierda
    QPixmap spriteBala = prepararFrame(":/capitan/danio/1.png", 1.0);
    if (!spriteBala.isNull()) {
        spriteBala = spriteBala.scaledToHeight(12, Qt::SmoothTransformation)
        .transformed(QTransform().scale(-1, 1));
    }

    // voltear lacrimogena a la izquierda
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
}
void vistaN1::agregarCapa(const QString& ruta, double factor, int z)
{
    QPixmap img(ruta);
    if (img.isNull())
        return;
    img = img.scaledToHeight(int(datos::ALTO_ESCENA), Qt::SmoothTransformation);

    // Ancho que debe cubrir la capa: la pantalla mas lo que se desplaza
    const double recorrido = (nivel.getAnchoMapa() - datos::ANCHO_ESCENA) * factor;
    const double cubrir = datos::ANCHO_ESCENA + recorrido;
    // Item vacio que contiene los trozos repetidos de la imagen
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
    // Borde izquierdo de la camara, sin salirse del mapa
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
    // Tres capas con distinta velocidad: la lejana se mueve poco, la cercana mas
    agregarCapa(":/capitan/fondo/1.png",  0.2, -30);   // fondo
    agregarCapa(":/capitan/medio/2.png",  0.5, -20);   // medio
    agregarCapa(":/capitan/frente/3.png", 0.8, -10);   // frente
}


void vistaN1::dibujarMapa()
{
    const QPen borde(Qt::black);
    const QBrush solida(QColor(90, 90, 90));
    const QBrush atravesable(QColor(120, 170, 255));

    for (const obstaculo& o : nivel.getPlataformas())
        escena->addRect(o.getIzquierda(), o.getArriba(), o.getAncho(), o.getAlto(),
                        borde, o.getAtravesable() ? atravesable : solida);
}

void vistaN1::dibujarPersonaje()
{
    const double r = nivel.getPersonaje().getRadio();
    //HIT BOX SE GENERA EN 0.0 Y SE POSICIONA EN EL CENTRO DEL CAPITAN (este no se ve pero juan si es neseario verlo cambia el qpn a qt::red)
    dibujoPersonaje = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,QPen(Qt::NoPen), QBrush(Qt::NoBrush));
    dibujoPersonaje->setPos(nivel.getPersonaje().getPosicion());
    //Z=5 encima del fondo(-10)/plataforma(-5)
    dibujoPersonaje->setZValue(5);
    dibujoSprite = new QGraphicsPixmapItem(dibujoPersonaje);
    // Tinte rojo para el golpe; se activa solo unos instantes
    efectoDanio = new QGraphicsColorizeEffect();
    efectoDanio->setColor(Qt::red);
    efectoDanio->setStrength(0.8);
    efectoDanio->setEnabled(false);
    dibujoSprite->setGraphicsEffect(efectoDanio);
    actualizarSprite();
}
void vistaN1::dibujarHUD()
{
    // Se cargan una sola vez, igual que los sprites: prepararFrame recorta
    QPixmap l = prepararFrame(":/capitan/vida/1.png", 1.0);
    QPixmap v = prepararFrame(":/capitan/vida/2.png", 1.0);
    if (!l.isNull())
        corazonLleno = l.scaledToHeight(32, Qt::SmoothTransformation);
    if (!v.isNull())
        corazonVacio = v.scaledToHeight(32, Qt::SmoothTransformation);

    // Un item por cada vida maxima
    const int total = nivel.getPersonaje().getVidasMax();
    for (int i = 0; i < total; ++i) {
        QGraphicsPixmapItem* c = escena->addPixmap(corazonLleno);
        c->setZValue(100);   // por encima de todo
        corazones.append(c);
    }
}

void vistaN1::actualizarHUD()
{
    const int vidas = nivel.getPersonaje().getVidas();
    for (int i = 0; i < corazones.size(); ++i) {
        // lleno si todavia le queda esa vida, vacio si la perdio
        corazones[i]->setPixmap(i < vidas ? corazonLleno : corazonVacio);
        corazones[i]->setPos(mapToScene(QPoint(15 + i * 40, 15)));
    }
}

QPixmap vistaN1::prepararFrame(const QString& ruta, double escala)
{
    QImage img(ruta);
    if (img.isNull())
        return QPixmap();
    img = img.convertToFormat(QImage::Format_ARGB32);

    // Calcula el rectangulo que ocupa el personaje
    int minX = img.width(), minY = img.height(), maxX = -1, maxY = -1;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(img.pixel(x, y)) >= 10) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }
    }
    if (maxX < 0)
        return QPixmap();
    // Recorta el espacio vacio de la imagen original (612x408)
    img = img.copy(minX, minY, maxX - minX + 1, maxY - minY + 1);
    // Escala segun el factor calculado en cargarSprites()
    QPixmap p = QPixmap::fromImage(img);
    return p.scaled(int(p.width() * escala), int(p.height() * escala),Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QVector<QPixmap> vistaN1::cargarFrames(const QString& carpeta, int desde, int hasta, double escala)
{
    QVector<QPixmap> frames;
    for (int i = desde; i <= hasta; ++i) {
        QPixmap p = prepararFrame(QString(":/capitan/%1/%2.png").arg(carpeta).arg(i), escala);
        if (!p.isNull())
            frames.append(p);
    }
    return frames;
}

void vistaN1::cargarSprites()
{
    // Una sola escala para todos los frames, calculada con "quieto" como referencia, para que el personaje no cambie de tamano entre animaciones
    QPixmap ref = prepararFrame(":/capitan/quieto/1.png", 1.0);
    const double escala = ref.isNull() ? 1.0 : 64.0 / ref.height();

    animaciones[QUIETO]  = cargarFrames("quieto",  1,  5,  escala);
    animaciones[AGACHAR] = cargarFrames("agachar", 6,  8,  escala);
    animaciones[CAMINAR] = cargarFrames("caminar", 9,  13, escala);
    animaciones[CORRER]  = cargarFrames("correr",  14, 18, escala);
    animaciones[SALTO]   = cargarFrames("salto",   19, 21, escala);

    qDebug() << "frames quieto:" << animaciones[QUIETO].size();   // debe decir 5
}

void vistaN1::actualizarSprite()
{
    const capitanN1& c = nivel.getPersonaje();
    const double vx = std::abs(c.getVelocidad().x());

    //  elegir animacion segun la fisica (en el aire, agachado, velocidad)
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

    // recordar hacia donde mira (solo cambia si hay tecla presionada)
    if (c.getDireccion() != 0)
        mirandoIzq = (c.getDireccion() < 0);

    // reiniciar el tiempo si cambio de animacion
    if (nuevo != estadoAnim) {
        estadoAnim = nuevo;
        tAnim = 0.0;
    }
    tAnim += datos::DT;
    const QVector<QPixmap>& frames = animaciones[estadoAnim];
    if (frames.isEmpty() || !dibujoSprite)
        return;
    //Escoger el frame: el salto usa la velocidad vertical, el resto avanza con el tiempo
    int idx;
    if (estadoAnim == SALTO) {
        const double vy = c.getVelocidad().y();
        idx = (vy < -100.0) ? 0 : (vy > 100.0 ? 2 : 1);
        idx = qMin(idx, int(frames.size()) - 1);
    } else {
        idx = int(tAnim * 10.0) % frames.size();
    }
    //Voltear la imagen si mira a la izquierda
    QPixmap p = frames[idx];
    if (mirandoIzq)
        p = p.transformed(QTransform().scale(-1, 1));
    //Centrar en x y apoyar los pies en la base del circulo
    dibujoSprite->setPixmap(p);
    dibujoSprite->setPos(-p.width() / 2.0, c.getRadio() - p.height());
    // Invulnerabilidad: parpadeo
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
    nivel.actualizar(dt);
    if (!nivel.getPersonaje().estaVivo()) {
        actualizarHUD();
        terminar(false);
    }
}

void vistaN1::dibujar(double a)
{
    //el a es para evitar que la lgica y la pantalla tengan diferencia de frames
    const capitanN1& c = nivel.getPersonaje();
    const QPointF pos = c.getPosicionAnterior() + (c.getPosicion() - c.getPosicionAnterior()) * a;

    dibujoPersonaje->setPos(pos);
     actualizarSprite();
    actualizarFondo(pos);
     dibujarPool(nivel.getBalas(), dibujoBalas, false);
     dibujarPool(nivel.getLacrimogenas(), dibujoLacrimogenas, true);
     actualizarNubes();
    centerOn(pos);
    actualizarHUD();

}

void vistaN1::actualizarDireccion()
{nivel.setDireccionPersonaje((teclaD ? 1 : 0) - (teclaA ? 1 : 0));}

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
    default: break;
    }
}
void vistaN1::alSoltar(int tecla) {
    switch (tecla) {
    case Qt::Key_A:
        teclaA = false;
        actualizarDireccion(); break;
    case Qt::Key_D:
        teclaD = false;
        actualizarDireccion(); break;
    default: break;
    }
}

void vistaN1::alPerderFoco() {
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
        it->setZValue(6); // por encima del capitán (Z=5) para envolverlo en humo
        it->setVisible(false);
        dibujoNubes.append(it);
    }
}

void vistaN1::actualizarNubes()
{
    if (framesGas.isEmpty())
        return;
    const std::vector<efecto>& nubes = nivel.getNubes();
    const double duracionTotal = 3.0; // misma DURACION_NUBE de nivel1.cpp
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