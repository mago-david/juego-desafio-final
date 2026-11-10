#include "obstaculo.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
using namespace std;
obstaculo::obstaculo(const vector2D& esquina_, double ancho_, double alto_,
                     bool atravesable_):zona(esquina_, ancho_, alto_),atravesable(atravesable_)
{
    setAncho(ancho_);
    setAlto(alto_);
}
obstaculo::obstaculo(const vector2D& esquina_, double lado)
    :obstaculo(esquina_, lado, lado,false){}

//acceso
bool obstaculo::getAtravesable()const
{return atravesable;}
void obstaculo::setAtravesable(bool a)
{atravesable = a;}

//colisiones
bool obstaculo::tocado(const entidadFisica& e)const
{
    const vector2D& c = e.getPosicion();
    double qx = max(getIzquierda(),min(c.getX(), getDerecha()));
    double qy = max(getArriba(),min(c.getY(), getAbajo()));
    double dx = c.getX() - qx;
    double dy = c.getY() - qy;
    double r  = e.getRadio();
    return dx * dx + dy * dy <= r * r;
}
ladoImpacto obstaculo::colisionar(entidadFisica& e,double coefRestitucion) const
{
    if (coefRestitucion < 0.0 || coefRestitucion > 1.0)
    {
        throw out_of_range("El coeficiente de restitucion debe estar entre 0 y 1");
    }
    vector2D p=e.getPosicion();
    vector2D v=e.getVelocidad();
    double r=e.getRadio();
    //con esto sabemos si los puntos x,y de la particula estan dentro del rec
    //=MAX(minimo rec,MIN(la pocision,maximo rec))
    double qx = max(getIzquierda(), min(p.getX(), getDerecha()));
    double qy = max(getArriba(),    min(p.getY(), getAbajo()));
    vector2D q(qx, qy);
    //aqui vemos si hay choque si la distancia entre la particula
    //y el rec es menor que su radio^2 hay choque
    vector2D d = p - q;
    double dist2 = d.magnitudP2();
    if (dist2 > r * r)
    {
        return ladoImpacto::ninguno;
    }
    vector2D n;
    double penetracion;
    ladoImpacto lado;
    //con esto sabemos que tan metido esta la particula
    if (dist2 > 0.0)
    {
        double dist = sqrt(dist2);
        n = d / dist;
        penetracion = r - dist;
        bool fueraX = p.getX() < getIzquierda() || p.getX() > getDerecha();
        bool fueraY = p.getY() < getArriba()    || p.getY() > getAbajo();

        if (fueraX && fueraY)
            lado = ladoImpacto::esquina;
        else if (fueraX)
            lado = (p.getX() < getIzquierda()) ? ladoImpacto::izquierda : ladoImpacto::derecha;
        else
            lado = (p.getY() < getArriba()) ? ladoImpacto::arriba : ladoImpacto::abajo;
    }
    else {
        // 2b. Centro DENTRO (iba muy rapido): sacar por el lado mas cercano
        double dIzq = p.getX() - getIzquierda();
        double dDer = getDerecha() - p.getX();
        double dArr = p.getY() - getArriba();
        double dAbj = getAbajo() - p.getY();
        double m = min(min(dIzq, dDer),min(dArr, dAbj));

        if (m == dIzq)      { n = vector2D(-1, 0); lado = ladoImpacto::izquierda; }
        else if (m == dDer) { n = vector2D( 1, 0); lado = ladoImpacto::derecha; }
        else if (m == dArr) { n = vector2D(0, -1); lado = ladoImpacto::arriba; }
        else                { n = vector2D(0,  1); lado = ladoImpacto::abajo; }
        penetracion = m + r;
    }
    // 3. Si ya se aleja, no rebotar (evita que vibre pegada)
    double vn = v.productoPunto(n);
    if (vn >= 0.0)
        return ladoImpacto::ninguno;

    // 4. Sacarla del obstaculo y separar la velocidad
    p += n * penetracion;
    vector2D vPerp = n * vn;
    vector2D vPar  = v - vPerp;

    // 5. Apoyada (gravedad): se anula v perpendicular y no cuenta como choque
    if (-vn < datos::UMBRAL_REPOSO) {
        e.setPosicion(p);
        e.setVelocidad(vPar);
        return ladoImpacto::ninguno;
    }

    // 6. Rebote inelastico
    e.setPosicion(p);
    e.setVelocidad(vPar - vPerp * coefRestitucion);
    return lado;
}







