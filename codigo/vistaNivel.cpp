#include "vistaNivel.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QImage>
#include <QKeyEvent>
#include <QTimer>
#include <algorithm>

vistaNivel::vistaNivel(QWidget* parent)
    : QGraphicsView(parent),
    reloj(new QTimer(this)),
    acumulado(0.0),
    pausado(true),
    relojMuerte(new QTimer(this)),
    opacidadMuerte(0.0),
    telonNegro(nullptr),
    spriteMoriste(nullptr),
    btnReintentar(nullptr),
    btnMenu(nullptr),
    escena(new QGraphicsScene(this))
{
    setScene(escena);
    //el metodo de arbol no nos sirve
    escena->setItemIndexMethod(QGraphicsScene::NoIndex);
    //no barra de desplazamiento
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //no marco
    setFrameShape(QFrame::NoFrame);
    //centramos en caso de pantalla mas grande
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //el focus para los controles
    setFocusPolicy(Qt::StrongFocus);
    //precise para evitar tirones
    reloj->setTimerType(Qt::PreciseTimer);
    connect(reloj, &QTimer::timeout, this, &vistaNivel::avanzar);
    //el tiempo incia en cada nivel osea cada vista
    crearUI_Muerte();
    connect(relojMuerte, &QTimer::timeout, this, &vistaNivel::actualizarFundidoMuerte);
}

//"momentos"

void vistaNivel::iniciar()
{
    acumulado = 0.0;
    cronometro.start();
    pausado = false;
    enPantallaFinal = false;
    reloj->start(16);
    setFocus();
}

void vistaNivel::pausar()
{
    pausado=true;
    reloj->stop();
}

void vistaNivel::terminar(bool gano)
{
    pausar();
    ganoNivel = gano;
    emit terminado(gano);
    if (!gano) {
        mostrarPantallaMuerte();
        return;
    }
    const QPointF origenCamara = mapToScene(viewport()->rect()).boundingRect().topLeft();
    telonNegro->setRect(origenCamara.x(), origenCamara.y(),
                        datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    telonNegro->setBrush(QBrush(QColor(0, 0, 0, 255)));
    telonNegro->setVisible(true);
    if (spriteVictoria && !spriteVictoria->pixmap().isNull()) {
        spriteVictoria->setPos(origenCamara);
        spriteVictoria->setVisible(true);
    }
    const int w = width(), h = height();
    const int anchoBtn = 190, altoBtn = 45, sep = 20;
    const int inicioX = (w - (anchoBtn * 3 + sep * 2)) / 2;
    const int yPos = int(h * 0.84);
    btnMenu->setText("Menu");
    btnSiguienteNivel->setText("Siguiente nivel");
    btnReintentar->setText("Reiniciar");
    btnMenu->setGeometry(inicioX, yPos, anchoBtn, altoBtn);
    btnSiguienteNivel->setGeometry(inicioX + anchoBtn + sep, yPos, anchoBtn, altoBtn);
    btnReintentar->setGeometry(inicioX + (anchoBtn + sep) * 2, yPos, anchoBtn, altoBtn);
    btnMenu->show();
    btnSiguienteNivel->show();
    btnReintentar->show();
    btnMenu->raise();
    btnSiguienteNivel->raise();
    btnReintentar->raise();
}
//ejecucion
void vistaNivel::avanzar()
{
    //tiempo real
    //seguridad para no encadenar lageos
    acumulado+=std::min(cronometro.nsecsElapsed()*datos::EPSILON,0.1);
    cronometro.start();

    while(!pausado && acumulado>=datos::DT)
    {
        avanzarLogica(datos::DT);
        acumulado-=datos::DT;
    }
    //sin esto solo se dibuja en cada paso y da tirones
    if(!pausado)
        dibujar(acumulado/datos::DT);
}

//controles
void vistaNivel::keyPressEvent(QKeyEvent* e) {
    if (e->isAutoRepeat())
        return;
    alPulsar(e->key());
}

void vistaNivel::keyReleaseEvent(QKeyEvent* e) {
    if (e->isAutoRepeat())
        return;
    alSoltar(e->key());
}

void vistaNivel::focusOutEvent(QFocusEvent* e) {
    alPerderFoco();
    QGraphicsView::focusOutEvent(e);
}

void vistaNivel::crearDibujos(const std::vector<proyectil>& pool,
                              QVector<QGraphicsEllipseItem*>& items, const QColor& color)
{
    items.reserve(int(pool.size()));
    for (const proyectil& p : pool)
    {
        const double r = p.getRadio();
        QGraphicsEllipseItem* it = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                QPen(Qt::NoPen), QBrush(color));
        it->setZValue(4); // sobre el mapa, bajo el capitan (5)
        it->setVisible(false); // la escena es la duena del item
        items.append(it);
    }
}

void vistaNivel::dibujarPool(const std::vector<proyectil>& pool,
                             const QVector<QGraphicsEllipseItem*>& items)
{
    for (int i = 0; i < int(pool.size()); ++i)
    {
        const bool activo = pool[i].estaActivo();
        items[i]->setVisible(activo);
        if (activo)
            items[i]->setPos(pool[i].getPosicion());
    }
}
//las de abajo son los proyectiles definitivos para n1 y seguramente para n2 de momento dejar las de arriba para evitar
void vistaNivel::crearDibujos(const std::vector<proyectil>& pool,
                              QVector<QGraphicsPixmapItem*>& items, const QPixmap& sprite)
{
    items.reserve(int(pool.size()));
    for (size_t i = 0; i < pool.size(); ++i)
    {
        QGraphicsPixmapItem* it = escena->addPixmap(sprite);
        it->setOffset(-sprite.width() / 2.0, -sprite.height() / 2.0);
        it->setTransformOriginPoint(0.0, 0.0);
        it->setZValue(4);
        it->setVisible(false);
        items.append(it);
    }
}

void vistaNivel::dibujarPool(const std::vector<proyectil>& pool,
                             const QVector<QGraphicsPixmapItem*>& items, bool rotar)
{
    for (int i = 0; i < int(pool.size()); ++i)
    {
        const bool activo = pool[i].estaActivo();
        items[i]->setVisible(activo);
        if (activo) {
            items[i]->setPos(pool[i].getPosicion());
            if (rotar)
                items[i]->setRotation(items[i]->rotation() + 12.0);
        }
    }
}
void vistaNivel::dibujarHUD(int vidasMax)
{
    QPixmap l = prepararFrame(":/capitan/vida/1.png", 1.0);
    QPixmap v = prepararFrame(":/capitan/vida/2.png", 1.0);
    QPixmap e = prepararFrame(":/capitan/vida/4.png", 1.0);
    if (!l.isNull())
        corazonLleno = l.scaledToHeight(32, Qt::SmoothTransformation);
    if (!v.isNull())
        corazonVacio = v.scaledToHeight(32, Qt::SmoothTransformation);
    if (!e.isNull())
        corazonExtra = e.scaledToHeight(32, Qt::SmoothTransformation);
    // Un item por cada vida maxima (4 en total)
    for (int i = 0; i < vidasMax; ++i) {
        const QPixmap& sp = (i < datos::VIDAS_INICIALES) ? corazonLleno : corazonExtra;
        QGraphicsPixmapItem* c = escena->addPixmap(sp);
        c->setZValue(100);
        corazones.append(c);
    }
}
void vistaNivel::actualizarHUD(int vidas)
{;
    const QPointF origen = mapToScene(15, 15);

    for (int i = 0; i < corazones.size(); ++i) {
        corazones[i]->setPos(origen.x() + i * 36.0, origen.y());

        if (i < datos::VIDAS_INICIALES) {
            corazones[i]->setPixmap(i < vidas ? corazonLleno : corazonVacio);
            corazones[i]->setVisible(true);
        } else {
            corazones[i]->setVisible(i < vidas);
        }
    }
}

QPixmap vistaNivel::prepararFrame(const QString& ruta, double escala)
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

QVector<QPixmap> vistaNivel::cargarFrames(const QString& carpeta, int desde, int hasta, double escala)
{
    QVector<QPixmap> frames;
    for (int i = desde; i <= hasta; ++i) {
        QPixmap p = prepararFrame(QString(":/capitan/%1/%2.png").arg(carpeta).arg(i), escala);
        if (!p.isNull())
            frames.append(p);
    }
    return frames;
}

void vistaNivel::alPerderFoco() {}

double vistaNivel::getAcumulado() const
{ return acumulado; }
bool vistaNivel::getPausado() const
{ return pausado; }
QPixmap vistaNivel::recortarSprite(const QString& ruta, double altoDeseado)
{
    QImage img(ruta);
    if (img.isNull())
        return QPixmap();
    img = img.convertToFormat(QImage::Format_ARGB32);

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

    img = img.copy(minX, minY, maxX - minX + 1, maxY - minY + 1);
    return QPixmap::fromImage(img).scaledToHeight(int(altoDeseado), Qt::SmoothTransformation);
}

void vistaNivel::crearUI_Muerte()
{
    telonNegro = escena->addRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA,QPen(Qt::NoPen), QBrush(QColor(0, 0, 0, 0)));
    telonNegro->setZValue(200);
    telonNegro->setVisible(false);
   //pantalla de muerte
    QPixmap imgMoriste = recortarSprite(":/capitan/vida/3.png",datos::ALTO_ESCENA);
    spriteMoriste = escena->addPixmap(imgMoriste);
    spriteMoriste->setZValue(201);
    spriteMoriste->setVisible(false);

    btnReintentar = new QPushButton("Reintentar", this);
    btnMenu= new QPushButton("Menu", this);
    QString estiloBoton =
        "QPushButton {"
        "  background-color: #222222;"
        "  color: white;"
        "  border: 2px solid #999999;"
        "  border-radius: 6px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #444444; border-color: white; }"
        "QPushButton:pressed { background-color: #111111; }";

    btnReintentar->setStyleSheet(estiloBoton);
    btnMenu->setStyleSheet(estiloBoton);
    const int anchoBtn = 190;
    const int altoBtn  = 45;
    const int centroX  = int(datos::ANCHO_ESCENA - anchoBtn) / 2;
    btnReintentar->setGeometry(centroX, 340, anchoBtn, altoBtn);
    btnMenu->setGeometry(centroX, 405, anchoBtn, altoBtn);
    btnReintentar->hide();
    btnMenu->hide();
    connect(btnReintentar, &QPushButton::clicked, this, &vistaNivel::solicitarReintento);
    connect(btnMenu,       &QPushButton::clicked, this, &vistaNivel::solicitarMenu);
    QPixmap imgVictoria(":/capitan/vida/5.png");
    if (!imgVictoria.isNull())
        imgVictoria = imgVictoria.scaled(int(datos::ANCHO_ESCENA), int(datos::ALTO_ESCENA),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    spriteVictoria = escena->addPixmap(imgVictoria);
    spriteVictoria->setZValue(201);
    spriteVictoria->setVisible(false);

    btnSiguienteNivel = new QPushButton("2. Siguiente nivel", this);
    btnSiguienteNivel->setStyleSheet(estiloBoton);
    btnSiguienteNivel->hide();
    connect(btnSiguienteNivel, &QPushButton::clicked, this, &vistaNivel::solicitarSiguienteNivel);
}
void vistaNivel::mostrarPantallaMuerte()
{
    opacidadMuerte = 0.0;
    QPointF origenCamara = mapToScene(QPoint(0, 0));

    telonNegro->setRect(origenCamara.x(), origenCamara.y(),
                        datos::ANCHO_ESCENA, datos::ALTO_ESCENA);
    telonNegro->setBrush(QBrush(QColor(0, 0, 0, 0)));
    telonNegro->setVisible(true);
    if (!spriteMoriste->pixmap().isNull()) {
        spriteMoriste->setPos(origenCamara.x(), origenCamara.y());
        spriteMoriste->setOpacity(0.0);
        spriteMoriste->setVisible(true);
    }
    relojMuerte->start(16);
}
void vistaNivel::actualizarFundidoMuerte()
{
    opacidadMuerte = std::min(1.0, opacidadMuerte + 0.025);
    int alfa = int(opacidadMuerte * 255);
    telonNegro->setBrush(QBrush(QColor(0, 0, 0, alfa)));
    spriteMoriste->setOpacity(opacidadMuerte);
    if (opacidadMuerte >= 1.0) {
        relojMuerte->stop();
        btnReintentar->show();
        btnMenu->show();
    }
}
void vistaNivel::posicionarBotonesFinales()
{
    const int w = width();
    const int h = height();
    const int anchoBtn = std::max(210, w / 6);
    const int altoBtn  = std::max(46, h / 14);

    if (ganoNivel) {
        const int sep = 20;
        const int inicioX = (w - (anchoBtn * 3 + sep * 2)) / 2;
        const int yPos = int(h * 0.84);
        btnMenu->setGeometry(inicioX, yPos, anchoBtn, altoBtn);
        btnSiguienteNivel->setGeometry(inicioX + anchoBtn + sep, yPos, anchoBtn, altoBtn);
        btnReintentar->setGeometry(inicioX + (anchoBtn + sep) * 2, yPos, anchoBtn, altoBtn);
    } else {
        const int centroX = (w - anchoBtn) / 2;
        btnReintentar->setGeometry(centroX, int(h * 0.62), anchoBtn, altoBtn);
        btnMenu->setGeometry(centroX, int(h * 0.62) + altoBtn + 16, anchoBtn, altoBtn);
    }
}


