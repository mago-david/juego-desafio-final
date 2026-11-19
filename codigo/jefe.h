#ifndef JEFE_H
#define JEFE_H

class jefe
{
public:
    enum class cuadrante { noroeste, noreste, suroeste, sureste };
    enum class combo { chorrosBalas, chorrosLluvia, balasLluvia };
    enum class fase { recarga, aviso, disparo };
public:
    jefe();
};

#endif // JEFE_H
