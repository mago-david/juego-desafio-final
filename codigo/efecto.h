#ifndef EFECTO_H
#define EFECTO_H
#include "zona.h"
#include "capitan.h"

class efecto : public zona
{
public:
    enum tipoefecto { gas, bufo, danio, senal };
private:
    tipoefecto tipo;
    double duracion;      // gas/explosion/chorro: segundos de vida que faltan; velocidad: duración del bufo
    double fuerzarebote;  // empuje del tipo danio
    bool activo;          // true activo, false libre o ya pasó
public:
    efecto();
    efecto(const QPointF& esquina_, double ancho_, double alto_, tipoefecto tipo_,
           double duracion_ = datos::DURACION_EFECTO, double fuerzarebote_ = datos::REBOTE_EFECTO);
    efecto(const efecto& o);
    ~efecto();
    //procesos
    void actualizar(double dt);
    void activar(const QPointF& esquina_, double duracion_);
    bool aplicar(capitan& c);
    //acceso
    tipoefecto gettipo() const;
    double getduracion() const;
    double getfuerzarebote() const;
    bool getactivo() const;
    void settipo(tipoefecto t);
    void setduracion(double d);
    void setfuerzarebote(double f);
    void setactivo(bool a);
    //operadores
    efecto& operator=(const efecto& o);
    bool operator==(const efecto& o) const;
};

#endif // EFECTO_H