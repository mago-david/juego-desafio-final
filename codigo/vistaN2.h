#ifndef VISTAN2_H
#define VISTAN2_H
#include <QVector>
#include "vistaNivel.h"
#include "nivel2.h"

class QGraphicsEllipseItem;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;
class QMouseEvent;

class vistaN2:public vistaNivel
{
    Q_OBJECT
private:
    nivel2 nivel;
    QGraphicsEllipseItem* dibujoPersonaje;
    QVector<QGraphicsEllipseItem*> dibujoEscudos;
    QGraphicsRectItem* dibujoJefe;
    QVector<QGraphicsEllipseItem*> dibujoBalas;
    QVector<QGraphicsRectItem*> dibujoEfectos;     // uno por efecto de la reserva
    QVector<QGraphicsEllipseItem*> dibujoEsbirros; // uno por hueco del pool
    QGraphicsSimpleTextItem* textoVidas;
    bool teclaW;
    bool teclaA;
    bool teclaS;
    bool teclaD;

    void dibujarArena();
    void dibujarPersonaje();
    void dibujarJefe();
    void crearEfectos();
    void dibujarEfectos();
    void crearEsbirros();
    void dibujarEsbirros();
    void actualizarDireccion();
protected:
    void avanzarLogica(double dt) override;
    void dibujar(double alfa) override;
    void alPulsar(int tecla) override;
    void alSoltar(int tecla) override;
    void alPerderFoco() override;
    void mousePressEvent(QMouseEvent* e) override;
public:
    explicit vistaN2(QWidget* parent = nullptr);
    const nivel2& getNivel() const;
    bool getTeclaW() const;
    bool getTeclaA() const;
    bool getTeclaS() const;
    bool getTeclaD() const;
};

#endif // VISTAN2_H