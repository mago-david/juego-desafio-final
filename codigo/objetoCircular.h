#ifndef OBJETOCIRCULAR_H
#define OBJETOCIRCULAR_H
#include <QPointF>

class objetoCircular
{
protected:
    QPointF velocidad;
    QPointF posicion;
private:
    double masa;
    double radio;
    bool gravedad;
public:
    objetoCircular(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
                   bool usaGravedad_ = true);
    objetoCircular(const objetoCircular& o) = default;
    virtual ~objetoCircular();

    //movimiento en el tiempo
    virtual void actualizar(double dt);

    //acceso
    const QPointF& getPosicion() const;
    const QPointF& getVelocidad() const;
    double getMasa() const;
    double getRadio() const;
    bool getUsaGravedad() const;
    void setPosicion(const QPointF& p);
    void setVelocidad(const QPointF& v);
    void setMasa(double m);
    void setRadio(double r);
    void setUsaGravedad(bool g);

    //operadores
    objetoCircular& operator=(const objetoCircular& o);
    bool operator==(const objetoCircular& o) const;
};

#endif // OBJETOCIRCULAR_H
