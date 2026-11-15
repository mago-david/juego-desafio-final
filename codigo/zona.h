#ifndef ZONA_H
#define ZONA_H
#include <QPointF>
#include "objetoCircular.h"
class zona
{
private:
    QPointF esquina;
    double ancho;
    double alto;
public:
    zona(const QPointF& esquina_, double ancho_, double alto_);
    zona(const zona& o) = default;

    //acceso
    const QPointF& getEsquina() const;
    double getAncho() const;
    double getAlto() const;
    void setEsquina(const QPointF& e);
    void setAncho(double a);
    void setAlto(double a);
    //bordes
    double getIzquierda() const;
    double getDerecha() const;
    double getArriba() const;
    double getAbajo() const;
    //operadores
    zona& operator=(const zona& o);
    bool operator==(const zona& o) const;
    //esta adentro?
    bool contiene(const objetoCircular& e) const;
};
#endif // ZONA_H
