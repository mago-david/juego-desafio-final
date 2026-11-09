#ifndef ZONA_H
#define ZONA_H
#include "vector2D.h"
#include "entidadFisica.h"
class zona
{
private:
    vector2D esquina;
    double ancho;
    double alto;
public:
    zona(const vector2D& esquina_, double ancho_, double alto_);

    //acceso
    const vector2D& getEsquina() const;
    double getAncho() const;
    double getAlto() const;
    void setEsquina(const vector2D& e);
    void setAncho(double a);
    void setAlto(double a);
    //bordes
    double getIzquierda() const;
    double getDerecha() const;
    double getArriba() const;
    double getAbajo() const;
    //esta adentro?
    bool contiene(const entidadFisica& e) const;
};
#endif // ZONA_H
