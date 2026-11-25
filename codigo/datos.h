#ifndef DATOS_H
#define DATOS_H
#include <random>
namespace datos {

//Escenario
const double ANCHO_ESCENA = 800.0;
const double ALTO_ESCENA  = 600.0;
// Fisica
const double GRAVEDAD = 500.0;
const double DT = 1.0 / 60.0;
const double EPSILON = 1e-9;        // tolerancia del == entre doubles
//valor para no vribar
const double UMBRAL_REPOSO = 15.0;

// Rozamiento viscoso F = -b*v
const double LIMITE_VEL = 8.0;    // velocidad maxima = a / b
const double COEF_FRICCION = 20.0;   // b dentro de una nube de gas
const double VEL_CORRER = 250.0;  // px/s

const double DURACION_EFECTO = 3.0;    // s: vida de la nube / duracion del bufo
const double REBOTE_EFECTO = 250.0;  // px/s del empuje al recibir danio (= velocidad maxima de correr)
const double VEL_MURO = 60.0;
// Vidas
const int VIDAS_INICIALES = 3;

const double GROSOR_PARED = 50.0;   // paredes solidas: mas gruesas que lo que avanza un escudo por paso
const double LADO_ARENA = ALTO_ESCENA;
const double X_ARENA = (ANCHO_ESCENA - LADO_ARENA) / 2.0;
const double CENTRO_X = X_ARENA + LADO_ARENA / 2.0;
const double CENTRO_Y = LADO_ARENA / 2.0;

// Jefe
const double ANCHO_JEFE = 160.0;
const double ALTO_JEFE  = 70.0;
const double X_JEFE = CENTRO_X - ANCHO_JEFE / 2.0;          // esquina superior izquierda
const double Y_JEFE = GROSOR_PARED;
const int NUM_COMBOS = 3;
const int NUM_ATAQUES=3;
//bomberman
const int NUM_BOMBAS = 6;
//generador mt19937 compartido (se define en jefe.cpp y se reutiliza en nivel2)
extern std::mt19937 generador;
}
#endif // DATOS_H
