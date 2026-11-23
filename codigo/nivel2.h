#ifndef NIVEL2_H
#define NIVEL2_H
#include "obstaculo.h"
#include "capitanN2.h"
#include "jefe.h"
#include "efecto.h"
#include "proyectil.h"
class nivel2
{
private:
    std::vector<obstaculo> paredes;
    capitanN2 personaje;
    jefe jefeFinal;
    std::vector<efecto> efectos;
    std::vector<proyectil>balas;
    std::vector<esbirro> esbirros;

    void construirArena();
    void construirBalas();
    void actualizarBalas(double dt);
    void actualizarEfectos(double dt);
    void actualizarEsbirros(double dt);
public:
    nivel2();
    nivel2(const nivel2& o) = default;

    //mueve cap y escudo y los choca
    void actualizar(double dt);

    //atajos para la vista
    void setDireccionPersonaje(int dx, int dy);
    bool lanzarEscudo(const QPointF& objetivo);

    //Acceso
    const std::vector<obstaculo>& getParedes() const;
    const capitanN2& getPersonaje() const;
    const jefe& getJefe() const;
    const std::vector<efecto>& getEfectos() const;
    const std::vector<proyectil>& getBalas() const;
    void setParedes(const std::vector<obstaculo>& p);
    void setPersonaje(const capitanN2& p);
    void setJefe(const jefe& j);
    void setEfectos(const std::vector<efecto>& e);
    void setBalas(const std::vector<proyectil>& b);
    const std::vector<esbirro>& getEsbirros() const;
    void setEsbirros(const std::vector<esbirro>& e);

    //operadores
    nivel2& operator=(const nivel2& o);
    bool operator==(const nivel2& o) const;
};

#endif // NIVEL2_H
