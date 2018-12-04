#include <mpi.h>
#include <random>
#include <iostream>
#include <fstream>
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
int tics = 0;
string output;

int nInfectadas;
int nSusceptibles;
int nMuertas;
int nRecuperadas;

int nInfectadas2;
int nSusceptibles2;
int nMuertas2;
int nRecuperadas2;

int * infectados;
int * personas;
int * cambios;
int * cambios2;

//Funciones
void init();
void iniciaEspacio(int mid, int cnt_procesos);
void print();
void actualizarMatriz(int mid);
void actualizarEstado();

void uso(string nombre_prog);

void obt_args(
	char*    argv[]        /* in  */,
	int&     cantidadP     /* out */,
	int&     potenciaVirus,
	int&     probaRecu,
	int&     ticsMuerte,
	int&     cantInfectada,
	int&     tam);

int main(int argc, char* argv[]) {
	double local_start, local_finish, local_elapsed, elapsed;
	int mid; // id de cada proceso
	int cnt_proc; // cantidad de procesos
	MPI_Status mpi_status; // para capturar estado al finalizar invocación de funciones MPI

	/* Arrancar ambiente MPI */
	MPI_Init(&argc, &argv);             		/* Arranca ambiente MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &mid); 		/* El comunicador le da valor a id (rank del proceso) */
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_proc);  /* El comunicador le da valor a p (número de procesos) */

	srand(time(NULL));

#  ifdef DEBUG 
	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD);
#  endif

	obt_args(argv, cantidadP, potenciaVirus, probaRecu, ticsMuerte, cantInfectada, tam);

	if (mid == 0) {
		local_start = MPI_Wtime();
	}

	init();
	iniciaEspacio(mid, cnt_proc);
	print();
	actualizarMatriz(mid);

	if (mid == 0) {

		local_finish = MPI_Wtime();
		local_elapsed = local_finish - local_start;
		MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

		output += "\n\nTiempo Total transcurrido = " + to_string(elapsed) + "s";
		output += "\nTiempo promedio por Tic: = " + to_string(elapsed / tics) + "s";

		ofstream out("Simulador.txt");
		out << output;
		out.close();

		cout << output << endl; //Editar
		int n;
		cin >> n;
	}

	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	MPI_Finalize();
	return 0;
}

void print() {
	output += "\nInfectadas: " + to_string(nInfectadas);
	output += "\nSusceptibles: " + to_string(nSusceptibles);
	output += "\nMuertas: " + to_string(nMuertas);
	output += "\nRecuperadas: " + to_string(nRecuperadas);

	//for (int i = 0; i < cantidadP * 3; i += 3) {
	//	std::cout << " P: ";
	//	cout << personas[i] << " | ";
	//	cout << personas[i + 1] << " | ";
	//	cout << personas[i + 2] << endl;
	//}
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

void iniciaEspacio(int mid, int cnt_procesos) {

	random_device rd;
	cantidadP = cantidadP / cnt_procesos;
	cambios = new int[tam*tam];
	cambios2 = new int[tam*tam];
	infectados = new int[tam*tam];

	for (int i = 0; i < tam*tam; i++) {
		infectados[i] = 0;
		cambios[i] = 0;
	}

	int tamTotal = 3 * cantidadP;
	personas = new int[tamTotal];

	// Insercion de personas
	int j = cantInfectada;
	for (int i = 0; j > 0; i += 3) {
		int x = rd() % (tam*tam);
		personas[i] = x;
		++infectados[x];                   // Posicion
		personas[i + 1] = INFECTADA;         // Estado
		personas[i + 2] = ticsMuerte;        // Tics
		--j;
	}
	j = cantidadP - cantInfectada;
	for (int i = cantInfectada * 3; j > 0; i += 3) {
		int x = rd() % (tam*tam);
		personas[i] = x;
		personas[i + 1] = SUSCEPTIBLE;
		personas[i + 2] = 0;
		--j;
	}

}

void actualizarEstado() {
	// I EMPIEZA EN ESTADO
	random_device rd;

	for (int i = 1; i < cantidadP * 3; i += 3) {
		switch (personas[i]) {
		case INFECTADA: {
			if (personas[i + 1] <= 1) {
				if ((rd() % 100) < probaRecu) {
					//cout << "SE RECUPERO UNA PERSONA INFECTADA" << endl << endl;
					personas[i] = RECUPERADA;
					nRecuperadas++;
				}
				else {
					//cout << "SE MURIO UNA PERSONA INFECTADA" << endl << endl;
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
			bool seInfecto = false;
			for (int g = 0; g < infectados[personas[i - 1]]; g++) {
				if (rd() % 100 < potenciaVirus) {
					seInfecto = true;
				}
			}
			if (seInfecto) {
				//cout << "SE INFECTO UNA PERSONA SUSCEPTIBLE" << endl << endl;
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

void actualizarMatriz(int mid) {

	random_device rd;

	double tic_start, tic_finish, tic_elapsed, elapsedTic;
	int random = 0;
	int indice = 0;

	while (nInfectadas > 0) { //While de los tic

		if (mid == 0) {
			output += "#TIC: " + to_string(tics);
			tic_start = MPI_Wtime();
			++tics;
		}

		nInfectadas = 0;
		nSusceptibles = 0;

		// Cambiar Posicion de Personas
		for (int i = 0; i < cantidadP * 3; i += 3) {
			random = rd() % 4;
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
				cambios[personas[i]] -= 1;
				cambios[indice]++;
			}
			personas[i] = indice;
		}


		// COMUNICACION

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Reduce(&cambios, &cambios2, tam*tam, MPI_INT, MPI_SUM, mid, MPI_COMM_WORLD);
		

		for (int z = 0; z < tam*tam; z++) {
			infectados[z] += cambios2[z];
		}

		actualizarEstado();

		for (int g = 0; g < tam*tam; g++) {
			cambios[g] = 0;
			cambios2[g] = 0;
		}

		if (mid == 0) {
			tic_finish = MPI_Wtime();
			tic_elapsed = tic_finish - tic_start;
			MPI_Reduce(&tic_elapsed, &elapsedTic, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

			MPI_Reduce(&nInfectadas, &nInfectadas2, 1, MPI_INT, MPI_SUM, mid, MPI_COMM_WORLD);
			MPI_Reduce(&nSusceptibles, &nSusceptibles2, 1, MPI_INT, MPI_SUM, mid, MPI_COMM_WORLD);
			MPI_Reduce(&nMuertas, &nMuertas2, 1, MPI_INT, MPI_SUM, mid, MPI_COMM_WORLD);
			MPI_Reduce(&nRecuperadas, &nRecuperadas2, 1, MPI_INT, MPI_SUM, mid, MPI_COMM_WORLD);

			print();
			output += "\nTiempo transcurrido en Tic = " + to_string(elapsedTic) + "\n\n";
		}
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

void uso(string nombre_prog /* in */) {
	cerr << nombre_prog.c_str() << " secuencia de parámetros de entrada" << endl;
	exit(0);
}  /* uso */
