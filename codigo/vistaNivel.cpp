#include "vistaNivel.h"
#include "datos.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QImage>
#include <QKeyEvent>
#include <QTimer>
#include <QSoundEffect>
#include <QUrl>
#include <algorithm>
#include <QResizeEvent>
#include <QPainter>

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
    enPantallaFinal(false),
    ganoNivel(false),
    esUltimoNivel(false),
    spriteVictoria(nullptr),
    btnSiguienteNivel(nullptr),
    sonidoGolpe1(nullptr),
    sonidoGolpe2(nullptr),
    sonidoMuerte(nullptr),
    sonidoBufo(nullptr),
    turnoGolpe(0),
    escena(new QGraphicsScene(this))
{
    setScene(escena);
    escena->setItemIndexMethod(QGraphicsScene::NoIndex);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setRenderHint(QPainter::SmoothPixmapTransform);

    QPalette paleta = palette();
    paleta.setColor(QPalette::Window, Qt::black);
    setPalette(paleta);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);

    reloj->setTimerType(Qt::PreciseTimer);
    connect(reloj, &QTimer::timeout, this, &vistaNivel::avanzar);

    crearUI_Final();
    connect(relojMuerte, &QTimer::timeout, this, &vistaNivel::actualizarFundidoMuerte);

    const qreal volumenEfectos = 0.8;
    const qreal volumenGolpejefe= 0.3;
    sonidoGolpe1 = new QSoundEffect(this);
    sonidoGolpe1->setSource(QUrl(QStringLiteral("qrc:/capitan/sonidos_danio/1.wav")));
    sonidoGolpe1->setVolume(volumenEfectos);

    sonidoGolpe2 = new QSoundEffect(this);
    sonidoGolpe2->setSource(QUrl(QStringLiteral("qrc:/capitan/sonidos_danio/2.wav")));
    sonidoGolpe2->setVolume(volumenEfectos);

    sonidoMuerte = new QSoundEffect(this);
    sonidoMuerte->setSource(QUrl(QStringLiteral("qrc:/capitan/sonidos_danio/3.wav")));
    sonidoMuerte->setVolume(volumenEfectos);

    sonidoBufo = new QSoundEffect(this);
    sonidoBufo->setSource(QUrl(QStringLiteral("qrc:/capitan/bufo/bufo.wav")));
    sonidoBufo->setVolume(volumenEfectos);

    sonidoGolpeJefe= new QSoundEffect(this);
    sonidoGolpeJefe->setSource(QStringLiteral("qrc:/capitan/sonidos_danio/4.wav"));
    sonidoGolpeJefe->setVolume(volumenGolpejefe);
}

void vistaNivel::setUltimoNivel(bool ultimo)
{
    esUltimoNivel = ultimo;
}

void vistaNivel::iniciar()
{
    acumulado = 0.0;
    cronometro.start();
    pausado = false;
    enPantallaFinal = false;
    alPerderFoco();
    reloj->start(16);
    setFocus();
}

void vistaNivel::pausar()
{
    pausado = true;
    reloj->stop();
}

void vistaNivel::terminar(bool gano)
{
    alPerderFoco();
    pausar();
    ganoNivel = gano;
    enPantallaFinal = true;
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

    btnMenu->setText("Menu");
    btnSiguienteNivel->setText("Siguiente nivel");
    btnReintentar->setText("Reiniciar");

    posicionarBotonesFinales();

    btnMenu->show();
    btnReintentar->show();
    btnMenu->raise();
    btnReintentar->raise();

    if (!esUltimoNivel) {
        btnSiguienteNivel->show();
        btnSiguienteNivel->raise();
    } else {
        btnSiguienteNivel->hide();
    }
}

void vistaNivel::avanzar()
{
    acumulado += std::min(cronometro.nsecsElapsed() * datos::EPSILON, 0.1);
    cronometro.start();

    while (!pausado && acumulado >= datos::DT) {
        avanzarLogica(datos::DT);
        acumulado -= datos::DT;
    }
    if (!pausado)
        dibujar(acumulado / datos::DT);
}

void vistaNivel::keyPressEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat())
        return;
    alPulsar(e->key());
}

void vistaNivel::keyReleaseEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat())
        return;
    alSoltar(e->key());
}

void vistaNivel::focusOutEvent(QFocusEvent* e)
{
    alPerderFoco();
    QGraphicsView::focusOutEvent(e);
}

void vistaNivel::resizeEvent(QResizeEvent* e)
{
    QGraphicsView::resizeEvent(e);
    const double s = std::min(width() / datos::ANCHO_ESCENA, height() / datos::ALTO_ESCENA);
    const int w = int(datos::ANCHO_ESCENA * s);
    const int h = int(datos::ALTO_ESCENA * s);
    const int mx = (width() - w) / 2;
    const int my = (height() - h) / 2;
    setViewportMargins(mx, my, width() - w - mx, height() - h - my);
    setTransform(QTransform::fromScale(s, s));

    if (enPantallaFinal)
        posicionarBotonesFinales();
}

void vistaNivel::crearDibujos(const std::vector<proyectil>& pool,
                              QVector<QGraphicsEllipseItem*>& items, const QColor& color)
{
    items.reserve(int(pool.size()));
    for (const proyectil& p : pool) {
        const double r = p.getRadio();
        QGraphicsEllipseItem* it = escena->addEllipse(-r, -r, 2.0 * r, 2.0 * r,
                                                      QPen(Qt::NoPen), QBrush(color));
        it->setZValue(4);
        it->setVisible(false);
        items.append(it);
    }
}

void vistaNivel::dibujarPool(const std::vector<proyectil>& pool,
                             const QVector<QGraphicsEllipseItem*>& items)
{
    for (int i = 0; i < int(pool.size()); ++i) {
        const bool activo = pool[i].estaActivo();
        items[i]->setVisible(activo);
        if (activo)
            items[i]->setPos(pool[i].getPosicion());
    }
}

void vistaNivel::crearDibujos(const std::vector<proyectil>& pool,
                              QVector<QGraphicsPixmapItem*>& items, const QPixmap& sprite)
{
    items.reserve(int(pool.size()));
    for (size_t i = 0; i < pool.size(); ++i) {
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
    for (int i = 0; i < int(pool.size()); ++i) {
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

    for (int i = 0; i < vidasMax; ++i) {
        const QPixmap& sp = (i < datos::VIDAS_INICIALES) ? corazonLleno : corazonExtra;
        QGraphicsPixmapItem* c = escena->addPixmap(sp);
        c->setZValue(100);
        corazones.append(c);
    }
}

void vistaNivel::actualizarHUD(int vidas)
{
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

    int minX = img.width(), minY = img.height(), maxX = -1, maxY = -1;
    for (int y = 0; y < img.height(); ++y) {
        const QRgb* fila = reinterpret_cast<const QRgb*>(img.constScanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(fila[x]) >= 10) {
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
    QPixmap p = QPixmap::fromImage(img);
    return p.scaled(int(p.width() * escala), int(p.height() * escala),
                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
        const QRgb* fila = reinterpret_cast<const QRgb*>(img.constScanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(fila[x]) >= 10) {
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
void vistaNivel::crearUI_Final()
{
    telonNegro = escena->addRect(0.0, 0.0, datos::ANCHO_ESCENA, datos::ALTO_ESCENA,
                                 QPen(Qt::NoPen), QBrush(QColor(0, 0, 0, 0)));
    telonNegro->setZValue(200);
    telonNegro->setVisible(false);

    QPixmap imgMoriste = recortarSprite(":/capitan/vida/3.png", datos::ALTO_ESCENA);
    spriteMoriste = escena->addPixmap(imgMoriste);
    spriteMoriste->setZValue(201);
    spriteMoriste->setVisible(false);

    btnReintentar = new QPushButton("Reintentar", this);
    btnMenu = new QPushButton("Menu", this);
    const QString estiloBoton =
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

    btnSiguienteNivel = new QPushButton("Siguiente nivel", this);
    btnSiguienteNivel->setStyleSheet(estiloBoton);
    btnSiguienteNivel->hide();
    connect(btnSiguienteNivel, &QPushButton::clicked, this, &vistaNivel::solicitarSiguienteNivel);
}
void vistaNivel::mostrarPantallaMuerte()
{
    opacidadMuerte = 0.0;
    const QPointF origenCamara = mapToScene(QPoint(0, 0));

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
    const int alfa = int(opacidadMuerte * 255);
    telonNegro->setBrush(QBrush(QColor(0, 0, 0, alfa)));
    spriteMoriste->setOpacity(opacidadMuerte);
    if (opacidadMuerte >= 1.0) {
        relojMuerte->stop();
        btnReintentar->setText("Reintentar");
        btnMenu->setText("Menu");
        posicionarBotonesFinales();
        btnReintentar->show();
        btnMenu->show();
        btnReintentar->raise();
        btnMenu->raise();
    }
}
void vistaNivel::posicionarBotonesFinales()
{
    const int w = width();
    const int h = height();
    const int anchoBtn = std::max(190, w / 6);
    const int altoBtn  = std::max(45, h / 14);

    if (ganoNivel) {
        const int sep = 20;
        const int yPos = int(h * 0.84);
        if (!esUltimoNivel) {
            const int inicioX = (w - (anchoBtn * 3 + sep * 2)) / 2;
            btnMenu->setGeometry(inicioX, yPos, anchoBtn, altoBtn);
            btnSiguienteNivel->setGeometry(inicioX + anchoBtn + sep, yPos, anchoBtn, altoBtn);
            btnReintentar->setGeometry(inicioX + (anchoBtn + sep) * 2, yPos, anchoBtn, altoBtn);
        } else {
            const int inicioX = (w - (anchoBtn * 2 + sep)) / 2;
            btnMenu->setGeometry(inicioX, yPos, anchoBtn, altoBtn);
            btnReintentar->setGeometry(inicioX + anchoBtn + sep, yPos, anchoBtn, altoBtn);
        }
    } else {
        const int centroX = (w - anchoBtn) / 2;
        btnReintentar->setGeometry(centroX, int(h * 0.62), anchoBtn, altoBtn);
        btnMenu->setGeometry(centroX, int(h * 0.62) + altoBtn + 16, anchoBtn, altoBtn);
    }
}
void vistaNivel::reproducirDanio()
{
    QSoundEffect* actual = (turnoGolpe % 2 == 0) ? sonidoGolpe1 : sonidoGolpe2;
    turnoGolpe = (turnoGolpe + 1) % 2;
    if (actual && actual->status() != QSoundEffect::Error) {
        actual->play();
    }
}
void vistaNivel::reproducirMuerte()
{
    if (sonidoMuerte && sonidoMuerte->status() != QSoundEffect::Error) {
        sonidoMuerte->play();
    }
}
void vistaNivel::reproducirBufo()
{
    if (sonidoBufo && sonidoBufo->status() != QSoundEffect::Error) {
        sonidoBufo->play();
    }
}
void vistaNivel::reproducirGolpeJefe()
{
    if (sonidoGolpeJefe && sonidoGolpeJefe->status() != QSoundEffect::Error) {
        sonidoGolpeJefe->play();
    }
}