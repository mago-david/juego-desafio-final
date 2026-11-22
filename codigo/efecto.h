#ifndef EFECTO_H
#define EFECTO_H
#include "zona.h"
#include "capitan.h"

class efecto : public zona
{
public:
    enum tipoefecto { gas, velocidad, vida, danio, explosion, chorro };
private:
    tipoefecto tipo;
    double duracion;      // gas/explosion/chorro: segundos de vida que faltan; velocidad: duración del bufo
    double fuerzarebote;  // empuje del tipo danio
    double aviso;         // segundos de aviso que faltan (no daña); 0 = ya activo
    bool activo;          // true activo, false libre o ya pasó

    bool esTemporal() const;   // gas, explosion y chorro expiran solos
public:
    efecto();
    efecto(const QPointF& esquina_, double ancho_, double alto_, tipoefecto tipo_,
           double duracion_ = 3.0, double fuerzarebote_ = 250.0);
    efecto(const efecto& o);
    ~efecto();
    void actualizar(double dt);
    void activar(const QPointF& esquina_, double duracion_);
    void activar(const QPointF& esquina_, double ancho_, double alto_,
                 tipoefecto tipo_, double aviso_, double duracion_);
    bool aplicar(capitan& c);
    tipoefecto gettipo() const;
    double getduracion() const;
    double getfuerzarebote() const;
    double getaviso() const;
    bool getactivo() const;
    void settipo(tipoefecto t);
    void setduracion(double d);
    void setfuerzarebote(double f);
    void setaviso(double a);
    void setactivo(bool a);
    efecto& operator=(const efecto& o);
    bool operator==(const efecto& o) const;
};

#endif // EFECTO_H