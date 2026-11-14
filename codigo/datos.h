#ifndef DATOS_H
#define DATOS_H
namespace datos {

//Escenario
const double ANCHO_ESCENA = 800.0;
const double ALTO_ESCENA  = 600.0;
// Fisica
const double GRAVEDAD = 500.0;
const double DT       = 1.0 / 60.0;
const double EPSILON  = 1e-9;        // tolerancia del == entre doubles
//valor para no vribar
const double UMBRAL_REPOSO = 15.0;

// Rozamiento viscoso F = -b*v
const double LIMITE_VEL    = 8.0;    // velocidad maxima = a / b
const double COEF_FRICCION = 20.0;   // b dentro de una nube de gas
const double VEL_CORRER    = 250.0;  // px/s

// Vidas
const int VIDAS_INICIALES = 3;
}
#endif // DATOS_H
