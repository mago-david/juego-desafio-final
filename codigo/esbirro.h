#ifndef ESBIRRO_H
#define ESBIRRO_H
#include "objetoCircular.h"

class esbirro:public objetoCircular
{
private:
    int vida;
public:
    esbirro();
    esbirro(const QPointF& pos, int vida_);
    esbirro(const esbirro& o) = default;
    ~esbirro();
    //acciones
    void reiniciar(const QPointF& pos, int vida_);
    void perseguir(const QPointF& objetivo);
    void recibirGolpe(int d);
    bool estaVivo() const;
    //acceso
    int getVida() const;
    void setVida(int v);
    //operadores
    esbirro& operator=(const esbirro& o);
    bool operator==(const esbirro& o) const;
};

#endif // ESBIRRO_H