#ifndef MUROPERSEGUIDOR_H
#define MUROPERSEGUIDOR_H
#include "zona.h"
#include "capitan.h"

class muroPerseguidor:public zona
{
private:
    double velocidad;//pixeles x s
    bool activo;
public:
    muroPerseguidor();
    muroPerseguidor(const QPointF& esquina_, double ancho_, double alto_,double velocidad_ = 60.0);
    muroPerseguidor(const muroPerseguidor& o);
    ~muroPerseguidor();
    //accion
    void actualizar(double dt,double xcapi);
    bool aplicar(capitan& c)const;
    //acceso
    double getvelocidad()const;
    bool getactivo()const;
    void setvelocidad(double v);
    void setactivo(bool a);
    //operadores
    muroPerseguidor& operator=(const muroPerseguidor& o);
    bool operator==(const muroPerseguidor& o)const;
};

#endif // MUROPERSEGUIDOR_H
