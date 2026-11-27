#ifndef PRUEBAS_H
#define PRUEBAS_H
#include <string>
#include <cmath>
#include <exception>

void titulo(const std::string& nombre);
void verificar(bool condicion, const std::string& descripcion);
void resumen(const std::string& nombre);   // imprime "=> nombre: X de Y correctas" y reinicia el conteo
int fallosTotales();

inline bool casi(double a, double b, double tol = 1e-6) { return std::fabs(a - b) <= tol; }

// true si ejecutar f lanza una excepcion
template <typename F>
bool lanza(F f)
{
    try { f(); } catch (const std::exception&) { return true; }
    return false;
}

void pruebaObjetoCircular();
void pruebaZona();
void pruebaObstaculo();
void pruebaProyectil();
void pruebaCapitan();
void pruebaCapitanN1();
void pruebaCapitanN2();
void pruebaEfecto();
void pruebaEsbirro();
void pruebaMuroPerseguidor();
void pruebaJefe();
void pruebaNivel1();
void pruebaNivel2();
#endif
