#include "proyectil.h"
#include <stdexcept>
#include "datos.h"
#include <cmath>
using namespace std;

proyectil::proyectil(const QPointF& pos, const QPointF& vel, double masa_, double radio_,
                     bool usaGravedad_, int rebotes_, double coefRestitucion_)
    : objetoCircular(pos, vel, masa_, radio_, usaGravedad_),
    rebotesMax(0), coefRestitucion(0.0), activo(true)
{
    setRebotesMax(rebotes_);
    setCoefRestitucion(coefRestitucion_);
}

//reutilizar
