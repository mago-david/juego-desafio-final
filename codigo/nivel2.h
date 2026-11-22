#ifndef NIVEL2_H
#define NIVEL2_H
#include "obstaculo.h"
#include "capitanN2.h"
class nivel2
{
private:
    std::vector<obstaculo> paredes;
    capitanN2 personaje;

    void construirArena();
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
    void setParedes(const std::vector<obstaculo>& p);
    void setPersonaje(const capitanN2& p);

    //operadores
    nivel2& operator=(const nivel2& o);
    bool operator==(const nivel2& o) const;
};

#endif // NIVEL2_H
