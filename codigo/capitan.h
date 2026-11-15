#ifndef CAPITAN_H
#define CAPITAN_H
#include "objetoCircular.h"
#include "datos.h"
class capitan:public objetoCircular
{
private:
    int vidas;
    int vidasMax;
    double T_invulnerable;
    double T_veloz;
    bool enNube;
public:
    capitan(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
            bool usaGravedad_ = true, int vidas_ = datos::VIDAS_INICIALES,
            int vidasMax_ = datos::VIDAS_INICIALES);
    capitan(const capitan& o) = default;
    //movimiento
    void actualizar(double dt) override;
    //acceso
    int getVidas() const;
    int getVidasMax() const;
    double getT_invulnerable() const;
    double getT_veloz() const;
    bool getEnNube()const;
    void setVidas(int v);
    void setVidasMax(int m);
    void setT_invulnerable(double t);
    void setT_veloz(double t);
    void setEnNube(bool n);
    //operadores
    capitan& operator=(const capitan& o);
    bool operator==(const capitan& o) const;
    //jugabilidad
    bool recibirDanio();
    bool ganarVida();
    bool estaVivo()const;
    double getLimiteVel()const;
    double getBufoVelocidad()const;
};

#endif // CAPITAN_H
