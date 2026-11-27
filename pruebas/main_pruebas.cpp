#include "pruebas.h"
#include <iostream>

// main del proyecto de pruebas (el del juego es otro)
int main()
{
    std::cout << "========== PRUEBAS DE LA LOGICA DEL JUEGO ==========\n";
    pruebaObjetoCircular();
    pruebaZona();
    pruebaObstaculo();
    pruebaProyectil();
    pruebaCapitan();
    pruebaCapitanN1();
    pruebaCapitanN2();
    pruebaEfecto();
    pruebaEsbirro();
    pruebaMuroPerseguidor();
    pruebaJefe();
    pruebaNivel1();
    pruebaNivel2();
    std::cout << "\nFallos totales: " << fallosTotales() << "\n";
    return fallosTotales() == 0 ? 0 : 1;
}
