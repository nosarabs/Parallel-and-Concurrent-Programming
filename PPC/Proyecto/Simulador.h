#ifndef PPC_GoL_S // Programación Paralela y Concurrente - Conway's Game of Life
#define PPC_GoL_S

#include <iostream>
using namespace std;

#include "Persona.h"

class Simulador {

public:
    Simulador();
    ~Simulador();

    void init();

private:
    int cantidadPersonas;
    int potenciaVirus;
    int probaRecu;
    int probaMuerte;
    int ogInfectada;
    int tam;
    int tics;


};

#endif
