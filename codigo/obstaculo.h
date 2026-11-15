#ifndef OBSTACULO_H
#define OBSTACULO_H
#include <QPointF>
#include "objetoCircular.h"
#include "zona.h"

class obstaculo:public zona
{
private:
    bool atravesable;
public:
    obstaculo(const QPointF& esquina_, double ancho_,
              double alto_,bool atravesable_=false);
    obstaculo(const QPointF& esquina_, double lado);
    obstaculo(const obstaculo& o) = default;

    //acceso
    bool getAtravesable()const;
    void setAtravesable(bool t);

    //operadores
    obstaculo& operator=(const obstaculo& o);
    bool operator==(const obstaculo& o) const;

    //colisiones
    bool tocado(const objetoCircular& e)const;
    // (rebote con e = coefRestitucion).
    // Devuelve true solo si hubo un impacto real: false si no se tocan, si ya se aleja
    // o si esta apoyado (velocidad perpendicular menor que UMBRAL_REPOSO).
    // Cada quien pasa su coeficiente (0 = aterriza sin rebotar, 1 = elastico,entre medio inelastico).
    bool colisionar(objetoCircular& e, double coefRestitucion) const;
};

#endif // OBSTACULO_H
