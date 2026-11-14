#ifndef CAPITANN1_H
#define CAPITANN1_H
#include "capitan.h"
#include <vector>
#include "obstaculo.h"

class capitanN1:public capitan
{
private:
    int direccion;
    int saltosRestantes;
    bool enSuelo;
    QPointF posicionAnterior;
    double T_sinPlataformas;
public:
    explicit capitanN1(const QPointF& pos, int vidas_ = datos::VIDAS_INICIALES);
    capitanN1(const capitanN1& o) = default;
    capitanN1();
    //aceleracion,tiempo de traspasar
    void actualizar(double dt) override;

    //acceso
    int getDireccion()const;
    void setDireccion(int d);
    int  getSaltosRestantes() const;
    void setSaltosRestantes(int s);
    bool getEnSuelo() const;
    void setEnSuelo(bool e);
    const QPointF& getPosicionAnterior() const;
    void setPosicionAnterior(const QPointF& p);
    double getT_sinPlataformas() const;
    void setT_sinPlataformas(double t);

    //operadores
    capitanN1& operator=(const capitanN1& o);
    bool operator==(const capitanN1& o)const;

    //acciones
    bool saltar();
    void resolverPlataformas(const std::vector<obstaculo>& plataformas);
    bool bajarPlataforma();
};

#endif // CAPITANN1_H
