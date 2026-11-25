#ifndef CAPITANN2_H
#define CAPITANN2_H
#include "capitan.h"
#include "proyectil.h"
#include <vector>
#include "obstaculo.h"
class capitanN2:public capitan
{
private:
    int direccionX;
    int direccionY;
    double tRecarga;
    std::vector<proyectil> escudos;
    bool fortachon;
    double tFortachon;
public:
    capitanN2();
    explicit capitanN2(const QPointF& pos, int vidas_ = datos::VIDAS_INICIALES, bool fortachon = false);
    capitanN2(const capitanN2& o) = default;
    ~capitanN2();
    // movimiento, recarga y movimiento de los escudos
    void actualizar(double dt) override;

    // ataque
    bool lanzarEscudo(const QPointF& objetivo);
    // choques
    void resolverParedes(const std::vector<obstaculo>& paredes);
    // rebote de escudo contra objetos circulares (enemigos, balas)
    int escudosGolpean(const objetoCircular& blanco);
    // ataque contra obstaculo (el jefe)
    int escudosGolpean(const obstaculo& blanco);

    // acceso
    int getDireccionX() const;
    int getDireccionY() const;
    double getTRecarga() const;
    const std::vector<proyectil>& getEscudos() const;
    void setDireccionX(int d);
    void setDireccionY(int d);
    void setTRecarga(double t);
    void setEscudos(const std::vector<proyectil>& e);
    bool getFortachon() const;
    void setFortachon(bool f);
    double getTFortachon() const;
    int getDanioEscudo() const;
    void activarBufoDanio(double duracionSeg = 10.0);

    // operadores
    capitanN2& operator=(const capitanN2& o);
    bool operator==(const capitanN2& o) const;
};

#endif // CAPITANN2_H