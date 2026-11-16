#ifndef NIVEL1_H
#define NIVEL1_H
#include<vector>
#include "efecto.h"
#include "capitanN1.h"
#include "obstaculo.h"
using namespace std;
class nivel1
{
private:
    vector<efecto> efectos;
    //lo que esta arriba de este comentario es momentanio para probar
    vector<obstaculo> plataformas;
    capitanN1 personaje;
    double anchoMapa;
    void construirMapa();
public:
    const std::vector<efecto>& getEfectos() const;
    //prueba
    nivel1();
    nivel1(const nivel1& o)=default;
    //mueve personaje y coliciones
    void actualizar(double dt);

    //acceso
    const std::vector<obstaculo>& getPlataformas() const;
    const capitanN1& getPersonaje() const;
    double getAnchoMapa() const;
    void setPlataformas(const std::vector<obstaculo>& p);
    void setPersonaje(const capitanN1& p);
    void setAnchoMapa(double a);

    void setDireccionPersonaje(int d);
    bool saltar();
    bool bajarPlataforma();

    //operadores
    nivel1& operator=(const nivel1& o);
    bool operator==(const nivel1& o) const;
};

#endif // NIVEL1_H
