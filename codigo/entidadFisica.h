#ifndef ENTIDADFISICA_H
#define ENTIDADFISICA_H
#include "vector2D.h"

class entidadFisica
{
protected:
    vector2D velocidad;
    vector2D posicion;
private:
    double masa;
    double radio;
    bool gravedad;
public:
    entidadFisica(const vector2D& pos, const vector2D& vel, double masa_, double radio_,
                  bool usaGravedad_ = true);
    entidadFisica(const entidadFisica& o) = default;
    virtual ~entidadFisica();

    //movimiento en el tiempo
    virtual void actualizar(double dt);

    //acceso
    const vector2D& getPosicion() const;
    const vector2D& getVelocidad() const;
    double getMasa() const;
    double getRadio() const;
    bool getUsaGravedad() const;
    void setPosicion(const vector2D& p);
    void setVelocidad(const vector2D& v);
    void setMasa(double m);
    void setRadio(double r);
    void setUsaGravedad(bool g);

    //operadores
    entidadFisica& operator=(const entidadFisica& o);
    bool operator==(const entidadFisica& o) const;

    //magnitudes fisicas
    vector2D momento() const;
    double energiaCinetica() const;
};

#endif // ENTIDADFISICA_H
