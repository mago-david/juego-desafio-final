#ifndef VISTANIVEL_H
#define VISTANIVEL_H
#include <QGraphicsView>
#include <QElapsedTimer>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QPixmap>
#include "proyectil.h"

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QPushButton;
class QTimer;
class QKeyEvent;
class vistaNivel:public QGraphicsView
{
    Q_OBJECT
private:
    QTimer* reloj;
    QElapsedTimer cronometro;
    double acumulado;
    bool pausado;
    QTimer* relojMuerte;
    double opacidadMuerte;
    QGraphicsRectItem* telonNegro;
    QGraphicsPixmapItem* spriteMoriste;
    QPushButton* btnReintentar;
    QPushButton* btnMenu;

    void crearUI_Muerte();
    void mostrarPantallaMuerte();
    QPixmap recortarSprite(const QString& ruta, double altoDeseado);

private slots:
    void avanzar();
    void actualizarFundidoMuerte();
protected:
    //la escena la hacen las hijas
    QGraphicsScene* escena;

    explicit vistaNivel(QWidget* parent = nullptr);

    //ni idea como describir
    virtual void avanzarLogica(double dt) = 0;
    virtual void dibujar(double alfa) = 0;
    virtual void alPulsar(int tecla) = 0;
    virtual void alSoltar(int tecla) = 0;
    virtual void alPerderFoco();

    void terminar(bool gano);
    //los controles
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;

    //cositas proyectiles
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsEllipseItem*>& items, const QColor& color);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsEllipseItem*>& items);
    //las de abajo son los proyectiles definitivos para n1 y seguramente para n2 de momento dejar las de arriba para evitar errores
    void crearDibujos(const std::vector<proyectil>& pool,
                      QVector<QGraphicsPixmapItem*>& items, const QPixmap& sprite);
    void dibujarPool(const std::vector<proyectil>& pool,
                     const QVector<QGraphicsPixmapItem*>& items, bool rotar = false);
public:
    //"momentos"
    void iniciar();
    void pausar();
    double getAcumulado() const;
    bool getPausado() const;
signals:
    void terminado(bool gano);
    void solicitarReintento();
    void solicitarMenu();
};

#endif // VISTANIVEL_H
