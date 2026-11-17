#ifndef PROYECTIL_H
#define PROYECTIL_H
#include "objetoCircular.h"

class proyectil:public objetoCircular
{
private:
    int rebotesMax;
    double coefRestitucion;
    bool activo;
public:
    proyectil(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
              bool gravedad_, int rebotes_, double coefRestitucion_);
    proyectil(const proyectil& o) = default;

    //lo usamos para reutilizar
    void reiniciar(const QPointF& pos,const QPointF& vel, int reboresMax);
    void registrarRebote();

    //acceso
    int    getRebotesMax() const;
    double getCoefRestitucion() const;
    bool   estaActivo() const;
    void setRebotesMax(int r);
    void setCoefRestitucion(double e);
    void setActivo(bool a);

    //operadores
    proyectil& operator=(const proyectil& o);
    bool operator==(const proyectil& o) const;
};

#endif // PROYECTIL_H
