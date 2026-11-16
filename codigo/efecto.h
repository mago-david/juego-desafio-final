#ifndef EFECTO_H
#define EFECTO_H
#include "zona.h"
#include "capitan.h"

class efecto:public zona
{
public:
    enum tipoefecto{gas,velocidad,vida,danio};
private:
    tipoefecto tipo;
    double duracion;//tiempo del bufo o debufo
    double fuerzarebote;//daño al jugador
    bool activo;//true activo false inactivo o ya paso
public:
    efecto();
    efecto(const QPointF& esquina_,double ancho_,double alto,tipoefecto tipo_,double duracion_=3.0,double fuerzarebote_=250.0);
    efecto(const efecto& o);
    ~efecto();
    bool aplicar(capitan& c);
    tipoefecto gettipo() const;
    double getduracion()const;
    double getfuerzarebote()const;
    bool getactivo()const;
    void settipo(tipoefecto t);
    void setduracion(double d);
    void setfuerzarebote(double f);
    void setactivo(bool a);
    efecto& operator=(const efecto& o);
    bool operator==(const efecto& o)const;
};

#endif // EFECTO_H
