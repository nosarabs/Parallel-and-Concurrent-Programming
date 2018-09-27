#include "Simulador.h"

Simulador::Simulador(){
  this->init();

}

Simulador::~Simulador(){

}

void Simulador::init(){

    cout << "Cantidad de Personas: ";
    cin >> cantidadPersonas;

    cout << "Potencia Infecciosa de Virus: ";
    cin >> potenciaVirus;

    cout << "Probabilidad de Recuperación: ";
    cin >> probaRecu;

    cout << "Probabilidad de Muerte: ";
    cin >> probaMuerte;

    cout << "Porcentaje de Personas Originalmente Infectadas: ";
    cin >> ogInfectada;

    cout << "Tamaño del Espacio Bidimensional: " << endl;
    cout << "1) 100x100 \n2) 500x500 \n3) 1000x1000" << endl;
    cin >> tam;
    if (x == 1) {
      tam = 100;
    } else if(x == 2) {
      tam = 500;
    } else {
      tam = 1000;
    }

    cout << "Duración de la Simulación: ";
    cin >> tics;

}
