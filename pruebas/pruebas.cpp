#include "pruebas.h"
#include <iostream>
using namespace std;

namespace { int total = 0, correctas = 0, fallos = 0; }

void titulo(const string& nombre) { cout << "\n---------- " << nombre << " ----------\n"; }

void verificar(bool condicion, const string& descripcion)
{
    ++total;
    if (condicion) ++correctas;
    cout << (condicion ? "  OK     " : "  FALLO  ") << descripcion << "\n";
}

void resumen(const string& nombre)
{
    cout << "=> " << nombre << ": " << correctas << " de " << total << " correctas\n";
    fallos += total - correctas;
    total = correctas = 0;
}

int fallosTotales() { return fallos; }
