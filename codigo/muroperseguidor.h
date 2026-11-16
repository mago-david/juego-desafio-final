#ifndef MUROPERSEGUIDOR_H
#define MUROPERSEGUIDOR_H
#include "zona.h"
#include "capitan.h"

class muroperseguidor:public zona
{
private:
    double velocidad;//pixeles x s
    bool activo;
public:
    muroperseguidor();
    muroperseguidor(const QPointF& esquina_, double ancho_, double alto_,double velocidad_ = 60.0);
    muroperseguidor(const muroperseguidor& o);
    ~muroperseguidor();
    void actualizar(double dt);
    bool aplicar(capitan& c)const;
    double getvelocidad()const;
    bool getactivo()const;
    void setvelocidad(double v);
    void setactivo(bool a);
    muroperseguidor& operator=(const muroperseguidor& o);
    bool operator==(const muroperseguidor& o)const;
};

#endif // MUROPERSEGUIDOR_H
