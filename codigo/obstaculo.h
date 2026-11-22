#ifndef OBSTACULO_H
#define OBSTACULO_H
#include <QPointF>
#include "objetoCircular.h"
#include "zona.h"

class obstaculo:public zona
{
private:
    bool atravesable;
    int spriteId;
public:
    obstaculo(const QPointF& pos, double ancho, double alto, bool atravesable = false, int spriteId = 0);
    obstaculo(const QPointF& esquina_, double lado);
    obstaculo(const obstaculo& o) = default;

    //acceso
    bool getAtravesable()const;
    void setAtravesable(bool t);
    int getSpriteId() const;
    void setSpriteId(int id);

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
    bool tocaCirculo(const QPointF& c, double r) const;
};

#endif // OBSTACULO_H
