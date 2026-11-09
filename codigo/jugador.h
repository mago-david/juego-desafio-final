#ifndef JUGADOR_H
#define JUGADOR_H
#include "entidadFisica.h"
#include "datos.h"
class jugador:public entidadFisica
{
private:
    int vidas;
    int vidasMax;
    double T_invulnerable;
    double T_veloz;
    bool enNube;
public:
    jugador(const vector2D& pos, const vector2D& vel, double masa_, double radio_,
            bool usaGravedad_ = true, int vidas_ = datos::VIDAS_INICIALES,
            int vidasMax_ = datos::VIDAS_INICIALES);
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
    //jugabilidad
    bool recibirDanio();
    bool ganarVida();
    void morir();
    bool estaVivo()const;
    double getLimiteVel()const;
    double getBufoVelocidad()const;
};

#endif // JUGADOR_H
