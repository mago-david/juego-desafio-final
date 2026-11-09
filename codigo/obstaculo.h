#ifndef OBSTACULO_H
#define OBSTACULO_H
#include "vector2D.h"
#include "entidadFisica.h"
#include "datos.h"
#include "zona.h"


enum class ladoImpacto { ninguno, arriba, abajo, izquierda, derecha, esquina };

class obstaculo:public zona
{
private:
    bool atravesable;
public:
    obstaculo(const vector2D& esquina_, double ancho_,
              double alto_,bool atravesable_=false);
    obstaculo(const vector2D& esquina_, double lado);

    //acceso
    bool getAtravesable()const;
    void setAtravesable(bool t);

    //colisiones
    bool tocado(const entidadFisica& e)const;
    ladoImpacto colisionar(entidadFisica& e,
                           double coefRestitucion = datos::COEF_RESTITUCION) const;
};

#endif // OBSTACULO_H
