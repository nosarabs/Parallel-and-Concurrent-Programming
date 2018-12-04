#include <mpi.h>
#include <random>
#include <iostream>
#include <string>
#include <time.h>
using namespace std;

enum Estado { SUSCEPTIBLE = 1, INFECTADA, RECUPERADA, MUERTA };

//Variables
int cantidadP = 0;
int potenciaVirus = 0;
int probaRecu = 0;
int ticsMuerte = 0;
int cantInfectada = 0;
int tam = 0;
string output;

int nInfectadas;
int nSusceptibles;
int nMuertas;
int nRecuperadas;

int * infectados;
int * personas;
int * cambios;

//Funciones
void init();
void iniciaEspacio();
void print();
void actualizarMatriz();
void acualizarEstado();

void obt_args(
	char*    argv[]        /* in  */,
	int&     cantidadP     /* out */,
	int&     potenciaVirus,
	int&     probaRecu,
	int&     ticsMuerte,
	int&     cantInfectada,
	int&     tam);

int main(int argc, char* argv[]) {

	srand(time(NULL));

	obt_args(argv, cantidadP, potenciaVirus, probaRecu, ticsMuerte, cantInfectada, tam);

	init();
	iniciaEspacio();
	print();
	actualizarMatriz();

	return 0;
}

void print() {
	cout << "Infectadas: " << nInfectadas << endl;
	cout << "Susceptibles: " << nSusceptibles << endl;
	cout << "Muertas: " << nMuertas << endl;
	cout << "Recuperadas: " << nRecuperadas << endl << endl;

	for (int i = 0; i < cantidadP * 3; i += 3) {
		std::cout << " P: ";
		cout << personas[i] << " | ";
		cout << personas[i + 1] << " | ";
		cout << personas[i + 2] << endl;
	}

	cout << endl;
}

void init() {


	output += "\nCantidad de personas: " + to_string(cantidadP);


	output += "\nPotencia del Virus: " + to_string(potenciaVirus) + "%";


	output += "\nProbabilidad Recuperacion: " + to_string(probaRecu) + "%";


	output += "\nTics Para Muerte: " + to_string(ticsMuerte);


	output += "\nPorcentaje de Personas Originalmente Infectadas: " + to_string(cantInfectada) + "%";

	output += "\nEspacio: " + to_string(tam) + "x" + to_string(tam);

	cantInfectada = cantidadP * cantInfectada / 100;
	nInfectadas = cantInfectada;
	nSusceptibles = cantidadP - cantInfectada;

}

void iniciaEspacio() {

	cambios = new int[tam*tam];
	infectados = new int[tam*tam];

	int tamTotal = 3 * cantidadP;
	personas = new int[tamTotal];

	// Insercion de personas
	int j = cantInfectada;
	for (int i = 0; j > 0; i += 3) {
		int x = rand() % (tam*tam);
		personas[i] = x;
		++cambios[x];                   // Posicion
		personas[i + 1] = INFECTADA;         // Estado
		personas[i + 2] = ticsMuerte;        // Tics
		--j;
	}
	j = cantidadP - cantInfectada;
	for (int i = cantInfectada * 3; j > 0; i += 3) {
		int x = rand() % (tam*tam);
		personas[i] = x;
		personas[i + 1] = SUSCEPTIBLE;
		personas[i + 2] = 0;
		--j;
	}

}

void actualizarEstado() {
	// I EMPIEZA EN ESTADO
	for (int i = 1; i < cantidadP * 3; i += 3) {
		switch (personas[i]) {
		case INFECTADA: {
			if (personas[i + 1] < 1) {
				if ((rand() % 100) < probaRecu) {
					cout << "SE RECUPERO UNA PERSONA INFECTADA" << endl << endl;
					personas[i] = RECUPERADA;
					nRecuperadas++;
				}
				else {
					cout << "SE MURIO UNA PERSONA INFECTADA" << endl << endl;
					personas[i] = MUERTA;
					nMuertas++;
				}
				cambios[personas[i - 1]]--;
			}
			else {
				personas[i + 1]--;
				nInfectadas++;
			}
			break;
		}
		case SUSCEPTIBLE: {
			default_random_engine generator;
			binomial_distribution<int> distribution(infectados[personas[i - 1]], potenciaVirus / 100);
			if (distribution(generator)) {
				cout << "SE INFECTO UNA PERSONA SUSCEPTIBLE" << endl << endl;
				personas[i] = INFECTADA;
				cambios[personas[i - 1]]++;
				personas[i + 1] = ticsMuerte;
				nInfectadas++;
			}
			else {
				nSusceptibles++;
			}
			break;
		}
		}
	}
}

// Antonio
void actualizarMatriz() {
	int tics = 0;
	int random = 0;
	int indice = 0;
	while (nInfectadas > 0) { //While de los tic
		++tics;
		nInfectadas = 0;
		nSusceptibles = 0;

		// Cambiar Posicion de Personas
		for (int i = 0; i < cantidadP * 3; i += 3) {
			random = rand() % 4;
			switch (random) {
			case 0: {
				indice = personas[i] + 1;
				break;
			}
			case 1: {
				indice = personas[i] - 1;
				break;
			}
			case 2: {
				indice = personas[i] + tam;
				break;
			}
			case 3: {
				indice = personas[i] - tam;
				break;
			}
			}
			// Si resulta que x es menor a 0 o mayor al tam
			if (random < 2) {
				if (indice < 0) {
					indice = tam - 1;
				}
				else if (indice >= (tam*tam)) {
					indice = (tam*tam) - tam;
				}
				// Si resulta que y es menor a 0 o mayor al tam
			}
			else {
				if (indice < 0) {
					indice = (tam*tam) + indice;
				}
				else if (indice >= (tam*tam)) {
					indice = (indice - (tam*tam));
				}
			}
			if (personas[i + 1] == INFECTADA) {
				cambios[personas[i]]--;
				cambios[indice]++;
			}
			personas[i] = indice;
		}


    // COMUNICACION
		for (int z = 0; z < tam*tam; z++) {
			infectados[z] += cambios[z];
		}

		actualizarEstado();
		print();
	}
	// Cambiar Estado de las personas

}

void obt_args(
	char*    argv[]        /* in  */,
	int&     cantidadP /* out */, int& potenciaVirus, int& probaRecu, int& ticsMuerte, int& cantInfectada, int& tam) {

	cantidadP = strtol(argv[1], NULL, 10); // se obtiene valor del argumento 1 pasado por "línea de comandos".
	potenciaVirus = strtol(argv[2], NULL, 10);
	probaRecu = strtol(argv[3], NULL, 10);
	ticsMuerte = strtol(argv[4], NULL, 10);
	cantInfectada = strtol(argv[5], NULL, 10);
	tam = strtol(argv[6], NULL, 10);

}  /* obt_args */
