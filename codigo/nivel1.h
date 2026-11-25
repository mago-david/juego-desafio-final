#ifndef NIVEL1_H
#define NIVEL1_H
#include<vector>
#include "capitanN1.h"
#include "obstaculo.h"
#include "proyectil.h"
#include "efecto.h"
#include "muroPerseguidor.h"
class nivel1
{
private:
    //datos de obstaculos y personaje
    std::vector<obstaculo> plataformas;
    capitanN1 personaje;
    std::vector<efecto> manifestantes;
    //los proyectiles
    std::vector<proyectil> balas;
    std::vector<proyectil> lacrimogenas;
    std::vector<efecto> nubes;
    //ubicaciones donde se va a disparar y tal
    int primeraAltura;
    int disparosRestantes;
    double tReloj;
    //bufo
    std::vector<efecto> bufos;
    void construirBufos();
    //creacion mapa
    double anchoMapa;
    void construirMapa();
    //muro perseguidor
    muroPerseguidor muro;

    //ataques proyectil
    void construirProy();
    bool lanzarEn(std::vector<proyectil>& pool, const QPointF& pos,  const QPointF& vel, int rebotes);
    void chocarCon(proyectil& p, bool ca) const;
    void actualizarProyectiles(double dt);
    void lanzarProyectiles(double dt);
    void elegirAlturas();
    void dispararSiguiente();
    bool puntoLibre(double x, double y, double r) const;
    void lanzarNube(const QPointF& centro);
public:
    nivel1();
    nivel1(int vidas_);
    nivel1(const nivel1& o)=default;
    ~nivel1();
    //mueve personaje y coliciones
    void actualizar(double dt);

    //disparos(lanza en ubicacion vacia)
    bool lanzarBala(const QPointF& pos, const QPointF& vel);
    bool lanzarLacrimogena(const QPointF& pos, const QPointF& vel);

    //acceso
    const std::vector<obstaculo>& getPlataformas() const;
    const capitanN1& getPersonaje() const;
    const std::vector<efecto>& getManifestantes() const;
    const std::vector<efecto>& getBufos() const;
    const std::vector<proyectil>& getBalas() const;
    const std::vector<proyectil>& getLacrimogenas() const;
    const std::vector<efecto>& getNubes() const;
    int getPrimeraAltura() const;
    int getDisparosRestantes() const;
    double getTReloj() const;
    double getAnchoMapa() const;
    const muroPerseguidor& getMuro() const;
    void setPlataformas(const std::vector<obstaculo>& p);
    void setPersonaje(const capitanN1& p);
    void setManifestantes(const std::vector<efecto>& m);
    void setBufos(const std::vector<efecto>& b);
    void setBalas(const std::vector<proyectil>& b);
    void setLacrimogenas(const std::vector<proyectil>& l);
    void setNubes(const std::vector<efecto>& n);
    void setPrimeraAltura(int i);
    void setDisparosRestantes(int n);
    void setTReloj(double t);
    void setAnchoMapa(double a);
    void setMuro(const muroPerseguidor& m);
    //atajos
    void setDireccionPersonaje(int d);
    bool saltar();
    bool bajarPlataforma();

    //operadores
    nivel1& operator=(const nivel1& o);
    bool operator==(const nivel1& o) const;
};

#endif // NIVEL1_H
