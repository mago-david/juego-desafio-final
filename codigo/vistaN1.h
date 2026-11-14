#ifndef VISTAN1_H
#define VISTAN1_H
#include "vistaNivel.h"
#include "nivel1.h"

class QGraphicsEllipseItem;

class vistaN1:public vistaNivel
{
    Q_OBJECT
private:
    nivel1 nivel;
    QGraphicsEllipseItem* dibujoPersonaje;
    bool teclaA;
    bool teclaD;

    void dibujarMapa();
    void dibujarPersonaje();
    void actualizarDireccion();
protected:
    void avanzarLogica(double dt) override;
    void dibujar(double alfa) override;
    void alPulsar(int tecla) override;
    void alSoltar(int tecla) override;
    void alPerderFoco() override;
public:
    explicit vistaN1(QWidget* parent = nullptr);
    const nivel1& getNivel() const;
    bool getTeclaA() const;
    bool getTeclaD() const;
};

#endif // VISTAN1_H
