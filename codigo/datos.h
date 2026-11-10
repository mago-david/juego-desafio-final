#ifndef DATOS_H
#define DATOS_H
namespace datos {

// Escenario
const double ANCHO_ESCENA = 800.0;
const double ALTO_ESCENA  = 600.0;

// Fisica
const double GRAVEDAD = 500.0;
const double DT       = 1.0 / 60.0;

// Colisiones
const double COEF_RESTITUCION = 0.9;

const double UMBRAL_REPOSO = 15.0;
//movimieto
const double VEL_CORRER = 250.0;
const double VEL_SALTO  = 450.0;
const double COEF_FRICCION=20.0;
const double LIMITE_VEL= 8.0;
//vida
const int    VIDAS_INICIALES = 3; //yo creo quitar
const int    VIDAS_MAXIMAS_NIVEL2 = 4;//yo creo quitar
//estados
const double TIEMPO_INVULNERABLE = 1.5;
const double FACTOR_VELOZ        = 1.5;
const double DURACION_VELOZ      = 5.0;
//daño
const double DANO_ESCUDO           = 10.0;
const double DANO_ESCUDO_FORTACHON = 15.0;
//nube de gas
const double DURACION_NUBE = 6.0;
}
#endif // DATOS_H
