#include "vistaN1.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <cmath>

vistaN1::vistaN1(QWidget* parent)
    : vistaNivel(parent),
    nivel(),dibujoPersonaje(nullptr),dibujoSprite(nullptr),
    teclaA(false),teclaD(false),
    estadoAnim(QUIETO),tAnim(0.0),mirandoIzq(false)
{
    escena->setSceneRect(0.0, 0.0, nivel.getAnchoMapa(), datos::ALTO_ESCENA);
    cargarSprites();
    dibujarFondo();
    dibujarMapa();
    dibujarPersonaje();
    dibujarEfectos();//prueba
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


QPixmap vistaN1::imagenObstaculo(const obstaculo& o)
{
    // Suelo, paredes y techo ocupan casi todo el mapa
    if (o.getAncho() > 1000.0 || o.getAlto() > 400.0)
        return QPixmap();
    // ajustar imagen
    if (o.getAtravesable()) {
        if (o.getIzquierda() < 1100.0)
            return prepararFrame(":/capitan/obstaculos/3.png", 1.0);
        return prepararFrame(":/capitan/obstaculos/4.png", 1.0);
    }
    if (o.getAncho() >= 120.0)
        return prepararFrame(":/capitan/obstaculos/1.png", 1.0);
    return prepararFrame(":/capitan/obstaculos/2.png", 1.0);
}

void vistaN1::dibujarMapa()
{
    const QPen borde(Qt::black);
    const QBrush solida(QColor(90, 90, 90));
    const QBrush atravesable(QColor(120, 170, 255));

    for (const obstaculo& o : nivel.getPlataformas()) {
        QPixmap tex = imagenObstaculo(o);

        if (tex.isNull()) {
            // suelo, paredes, techo (o en caso de no cargar algo)
            QGraphicsRectItem* r = escena->addRect(o.getIzquierda(), o.getArriba(),
                                                   o.getAncho(), o.getAlto(),
                                                   borde, o.getAtravesable() ? atravesable : solida);
            r->setZValue(0);
            continue;
        }
        // ajuste de imagen a hit box
        tex = tex.scaledToWidth(int(o.getAncho()), Qt::SmoothTransformation);
        if (tex.height() > o.getAlto())
            tex = tex.scaledToHeight(int(o.getAlto()), Qt::SmoothTransformation);
        // centrar
        QGraphicsPixmapItem* img = escena->addPixmap(tex);
        img->setPos(o.getIzquierda() + (o.getAncho() - tex.width()) / 2.0,
                    o.getAbajo() - tex.height());
        img->setZValue(1);
    }
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
    actualizarSprite();
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
}
void vistaN1::avanzarLogica(double dt)
{
    nivel.actualizar(dt);
}

void vistaN1::dibujar(double a)
{
    //el a es para evitar que la lgica y la pantalla tengan diferencia de frames
    const capitanN1& c = nivel.getPersonaje();
    const QPointF pos = c.getPosicionAnterior() + (c.getPosicion() - c.getPosicionAnterior()) * a;
    qDebug() << "vidas" << c.getVidas() << "lento" << c.getT_lento()
             << "veloz" << c.getT_veloz() << "inv" << c.getT_invulnerable();
    //qdebug pruebas
    dibujoPersonaje->setPos(pos);
     actualizarSprite();
    actualizarFondo(pos);
    centerOn(pos);

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
void vistaN1::dibujarEfectos()
{
    for (size_t i = 0; i < nivel.getEfectos().size(); ++i) {
        const efecto& e = nivel.getEfectos()[i];
        QColor color;
        if (e.gettipo() == efecto::gas)            color = QColor(80, 200, 80, 110);
        else if (e.gettipo() == efecto::velocidad) color = QColor(255, 220, 0, 160);
        else if (e.gettipo() == efecto::vida)      color = QColor(255, 60, 120, 160);
        else                                       color = QColor(255, 0, 0, 110);

        QGraphicsRectItem* r = escena->addRect(e.getIzquierda(), e.getArriba(),
                                               e.getAncho(), e.getAlto(),
                                               QPen(Qt::NoPen), QBrush(color));
        r->setZValue(2);
    }
}






















