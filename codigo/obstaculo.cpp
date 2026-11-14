#include "obstaculo.h"
#include "datos.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
using namespace std;
obstaculo::obstaculo(const QPointF& esquina_, double ancho_, double alto_,
                     bool atravesable_):zona(esquina_, ancho_, alto_),atravesable(atravesable_)
{
    setAncho(ancho_);
    setAlto(alto_);
}
obstaculo::obstaculo(const QPointF& esquina_, double lado)
    :obstaculo(esquina_, lado, lado,false){}

//acceso
bool obstaculo::getAtravesable()const
{return atravesable;}

void obstaculo::setAtravesable(bool a)
{atravesable = a;}

//operadores
obstaculo& obstaculo::operator=(const obstaculo& o)
{
    if (this != &o) {
        zona::operator=(o);
        atravesable = o.atravesable;
    }
    return *this;
}
bool obstaculo::operator==(const obstaculo& o) const
{
    return zona::operator==(o) && atravesable == o.atravesable;
}

//colisiones
bool obstaculo::tocado(const objetoCircular& e)const
{
    const QPointF& c = e.getPosicion();
    double qx = max(getIzquierda(),min(c.x(), getDerecha()));
    double qy = max(getArriba(),min(c.y(), getAbajo()));
    double dx = c.x() - qx;
    double dy = c.y() - qy;
    double r  = e.getRadio();
    return dx * dx + dy * dy <= r * r;
}
bool obstaculo::colisionar(objetoCircular& e,double coefRestitucion) const
{
    if (coefRestitucion < 0.0 || coefRestitucion > 1.0)
    {
        throw out_of_range("El coeficiente de restitucion debe estar entre 0 y 1");
    }
    QPointF p=e.getPosicion();
    QPointF v=e.getVelocidad();
    double r=e.getRadio();
    //con esto sabemos si los puntos x,y de la particula estan dentro del rec
    //=MAX(minimo rec,MIN(la pocision,maximo rec))
    double qx = max(getIzquierda(), min(p.x(), getDerecha()));
    double qy = max(getArriba(),    min(p.y(), getAbajo()));
    QPointF q(qx, qy);
    //aqui vemos si hay choque si la distancia entre la particula
    //y el rec es menor que su radio^2 hay choque
    QPointF d = p - q;
    double dist2 = QPointF::dotProduct(d, d);
    if (dist2 > r * r)
    {
        return false;
    }
    QPointF n;
    double penetracion;
    //con esto sabemos que tan metido esta la particula
    if (dist2 > 0.0)
    {
        //Centro fuera del rectangulo (caso normal)
        double dist = sqrt(dist2);
        n = d / dist;
        penetracion = r - dist;
    }
    else {
        // 2b. Centro DENTRO (iba muy rapido): sacar por el lado mas cercano
        double dIzq = p.x() - getIzquierda();
        double dDer = getDerecha() - p.x();
        double dArr = p.y() - getArriba();
        double dAbj = getAbajo() - p.y();
        double m = min(min(dIzq, dDer),min(dArr, dAbj));

        if (m == dIzq)
            n = QPointF(-1, 0);
        else if (m == dDer)
            n = QPointF( 1, 0);
        else if (m == dArr)
            n = QPointF(0, -1);
        else
            n = QPointF(0,  1);
        penetracion = m + r;
    }
    // en esta parte si el producto punto entre la velocidad y la normal si es negativa
    //significa q va hacia una nuperficie, si no se aleja entonces no pasa nada
    double vn = QPointF::dotProduct(v, n);
    if (vn >= 0.0)
        return false;

    //con esta p se saca lo justo para que roce la superficie
    p += n * penetracion;
    //velocidad q choca contra superficie
    QPointF vPerp = n * vn;
    //velocidad q se "desliza", osea no le pasa nada
    QPointF vPar  = v - vPerp;

    // si su velocidad en este caso solo gravedad es menor a un rango especifico
    //decimos q esta en reposo no chocando dejamos su pocision y la velocidad q "desliza"
    if (-vn < datos::UMBRAL_REPOSO) {
        e.setPosicion(p);
        e.setVelocidad(vPar);
        return false;
    }

    // con esto la ponemos en su posicion la Vparalela se mantiene y la Vperpendicular
    //se invierte pero con coefRes definimos si rebota, si, no, o si pierde enegia
    e.setPosicion(p);
    e.setVelocidad(vPar - vPerp * coefRestitucion);
    return true;
}
