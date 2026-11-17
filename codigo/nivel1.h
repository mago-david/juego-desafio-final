#ifndef NIVEL1_H
#define NIVEL1_H
#include<vector>
#include "capitanN1.h"
#include "obstaculo.h"
#include "proyectil.h"
using namespace std;
class nivel1
{
private:
    //datos de obstaculos y personaje
    vector<obstaculo> plataformas;
    capitanN1 personaje;
    //los proyectiles
    vector<proyectil> balas;
    vector<proyectil> lacrimogenas;
    //ubicaciones donde se va a disparar y tal
    int primeraAltura;
    int disparosRestantes;
    double tReloj;


    //creacion mapa
    double anchoMapa;
    void construirMapa();
    //ataques proyectil
    void construirProy();
    bool lanzarEn(vector<proyectil>& pool, const QPointF& pos,
                  const QPointF& vel, int rebotes);
    void chocarCon(proyectil& p) const;
    void actualizarProyectiles(double dt);
    void lanzarProyectiles(double dt);
    void elegirAlturas();
    void dispararSiguiente();
    bool puntoLibre(double x, double y, double r) const;
public:
    nivel1();
    nivel1(const nivel1& o)=default;
    //mueve personaje y coliciones
    void actualizar(double dt);

    //disparos(lanza en ubicacion vacia)
    bool lanzarBala(const QPointF& pos, const QPointF& vel);
    bool lanzarLacrimogena(const QPointF& pos, const QPointF& vel);

    //acceso
    const std::vector<obstaculo>& getPlataformas() const;
    const capitanN1& getPersonaje() const;
    double getAnchoMapa() const;
    void setPlataformas(const std::vector<obstaculo>& p);
    void setPersonaje(const capitanN1& p);
    void setAnchoMapa(double a);

    const vector<proyectil>& getBalas() const;
    const vector<proyectil>& getLacrimogenas() const;
    int getPrimeraAltura() const;
    int getDisparosRestantes() const;
    double getTReloj() const;
    void setBalas(const vector<proyectil>& b);
    void setLacrimogenas(const vector<proyectil>& p);
    void setPrimeraAltura(int i);
    void setDisparosRestantes(int n);
    void setTReloj(double t);

    void setDireccionPersonaje(int d);
    bool saltar();
    bool bajarPlataforma();

    //operadores
    nivel1& operator=(const nivel1& o);
    bool operator==(const nivel1& o) const;
};

#endif // NIVEL1_H
