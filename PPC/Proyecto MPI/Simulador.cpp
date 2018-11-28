ï»¿// Antonio Ercolani Rojas B62456
   // ProgramaciÃ³n Paralela y Concurrente - Conway's Game of Life
#include "Simulador.h"
#include "Persona.h"

using namespace std;
using namespace std::chrono;

Simulador::Simulador() {
	this->init();
	omp_init_lock(&lck);
}

Simulador::~Simulador() {

}

// Pregunta los datos de Entrada
void Simulador::init() {

	int h;
	cout << "Cantidad de Hilos (0 para maxima cantidad posible): " << endl;
	cin >> h;
	setHilos(h);
	output += "\nHilos Usados: " + to_string(hilos);

	cout << "Cantidad de Personas: ";
	cin >> cantidadP;
	output += "\nCantidad de personas: " + to_string(cantidadP);

	cout << "Potencia Infecciosa de Virus [0-100]: ";
	cin >> potenciaVirus;
	output += "\nPotencia del Virus: " + to_string(potenciaVirus) + "%";

	cout << "Probabilidad de Recuperacion [0-100]: ";
	cin >> probaRecu;
	output += "\nProbabilidad Recuperacion: " + to_string(probaRecu) + "%";

	cout << "Tics para Muerte o Recuperacion: ";
	cin >> ticsMuerte;
	output += "\nTics Para Muerte: " + to_string(ticsMuerte);


	cout << "Porcentaje de Personas Originalmente Infectadas [0 - 100]: ";
	cin >> cantInfectada;
	output += "\nPorcentaje de personas infectadas inicial: " + to_string(cantInfectada) + "%";
	cantInfectada = (cantidadP / cantInfectada);

	cout << "Tamano del Espacio Bidimensional: " << endl;

	cout << "1) 100x100 \n2) 500x500 \n3) 1000x1000 \n4) 35x35" << endl;
	cin >> tamano;

	if (tamano == 1) {
		tamano = 100;
		output += "\nEspacio: 100x100";
	}
	else if (tamano == 2) {
		tamano = 500;
		output += "\nEspacio: 500x500";
	}
	else if (tamano == 3) {
		tamano = 1000;
		output += "\nEspacio: 1000x1000";
	}
	else {
		tamano = 35;
		output += "\nEspacio: 35x35";
	}
}

void Simulador::iniciarMatriz() {
	int randomInt1;
	int randomInt2;

	matriz.resize(tamano);
	for (int i = 0; i < tamano; ++i) {
		matriz[i].resize(tamano);
	}

	// Insercion de las Personas
	int pInfectadas = cantInfectada;
	int pSaludables = cantidadP - pInfectadas;

	for (int k = 0; k < pSaludables; ++k) {
		Persona* p = new Persona();
		// Inicializar datos de las Persona p Saludable
		p->setEstado(Susceptible);
		// Escoger PosiciÃ³n aleatoria
		randomInt1 = rand() % tamano;
		randomInt2 = rand() % tamano;
		matriz[randomInt1][randomInt2].push_back(*p);
		tSusceptibles++;
	}

	for (int k = 0; k < pInfectadas; ++k) {
		Persona* p = new Persona();
		// Inicializar datos de las Persona p Infectada
		p->setEstado(Infectada);
		// Escoger PosiciÃ³n aleatoria
		randomInt1 = rand() % tamano;
		randomInt2 = rand() % tamano;
		matriz[randomInt1][randomInt2].push_back(*p);
		tInfectadas++;
	}

}


string Simulador::actualizarMatriz() {
	steady_clock::time_point t1 = steady_clock::now();
	steady_clock::time_point t2 = steady_clock::now();
	int tics = 0;

	// Reiniciamos los contadores de cantidad de personas en cada estado
	while (tInfectadas != 0) {
		t1 = steady_clock::now();

		tMuertas = 0;
		tRecuperadas = 0;
		tInfectadas = 0;
		tSusceptibles = 0;
		int infectadas = 0;

		// Recorrido actualizando la posicion de las personas
#pragma omp parallel for num_threads(hilos)
		for (int i = 0; i < tamano; ++i) {
			for (int j = 0; j < tamano; ++j) {
				for (int y = 0; y < matriz[i][j].size(); ++y) {
					moverPersonas(i, j, y);
				}
			}
		}

		// Recorrrido actualizando el estado de las personas
#pragma omp parallel for num_threads(hilos)
		for (int i = 0; i < tamano; ++i) {
			for (int j = 0; j < tamano; ++j) {
				infectadas = personasInfectadas(i, j);
				for (int y = 0; y < matriz[i][j].size(); ++y) {
					actualizarEstado(i, j, y, infectadas);
				}
			}
		}

		++tics;
		output += "\n\nTIC#: " + to_string(tics);
		output += "\nSUSCEPTIBLES \nPromedio: " + to_string(tSusceptibles / tics) + " / Porcentaje: " + to_string((tSusceptibles * 100) / cantidadP) + "% / Cantidad: " + to_string(tSusceptibles);
		output += "\nINFECTADAS: \nPromedio: " + to_string(tInfectadas / tics) + " / Porcentaje: " + to_string((tInfectadas * 100) / cantidadP) + "% / Cantidad: " + to_string(tInfectadas);
		output += "\nRECUPARADAS: \nPromedio: " + to_string(tRecuperadas / tics) + " / Porcentaje: " + to_string((tRecuperadas * 100) / cantidadP) + "% / Cantidad: " + to_string(tRecuperadas);
		output += "\nMUERTAS: \nPromedio: " + to_string(tMuertas / tics) + " / Porcentaje: " + to_string((tMuertas * 100) / cantidadP) + "% / Cantidad: " + to_string(tMuertas);
		t2 = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		output += "\nDuracion de Tic: " + to_string(time_span.count()) + "s";
		duracion += time_span.count();
	}
	output += "\n\nTics totales: " + to_string(tics);
	output += "\nDuracion por tic: " + to_string(duracion / tics) + "s";
	return output;
}

// Cuenta el numero de personas infectadas en la lista
int Simulador::personasInfectadas(int i, int j) {
	list<Persona>::iterator it;
	Estado estado;
	int nInfectadas = 0;
	for (int y = 0; y < matriz[i][j].size(); ++y) {
		if (matriz[i][j][y].getEstado() == Infectada) {
			nInfectadas++;
		}
	}
	return nInfectadas;
}

// Actualiza el estado de la persona de acuerdo a la cantidad de peprsonas infectadas
// Se utiliza el pragma omp atomic para evitar conflictos con los contadores
void Simulador::actualizarEstado(int i, int j, int y, int inf) {
	Estado est = matriz[i][j][y].getEstado();
	switch (est) {
	case Infectada: {
		if (matriz[i][j][y].getTiempoInfectado() >= ticsMuerte) {
			random_device rd;
			int randomInt1 = rd() % 100;
			if (randomInt1 < probaRecu) {
				matriz[i][j][y].setEstado(Recuperada);
				matriz[i][j][y].resetTiempo();
#pragma omp atomic 
				tRecuperadas++;
			}
			else {
				matriz[i][j][y].setEstado(Muerta);
				tMuertas++;
			}
		}
		else {
			matriz[i][j][y].addTiempo();
#pragma omp atomic 
			tInfectadas++;
		}
		break;
	}
	case Susceptible: {
		random_device rd;
		int randomInt2 = rd() % 100;
		bool infectada = false;
		for (int g = 0; g < inf; g++) {
			if (randomInt2 < cantInfectada) {
				infectada = true;
			}
		}
		if (infectada) {
			matriz[i][j][y].setEstado(Infectada);
#pragma omp atomic 
			tInfectadas++;

		}
		else {
#pragma omp atomic 
			tSusceptibles++;
		}
		break;
	}
	case Muerta: {
#pragma omp atomic 
		tMuertas++;
		break;
	}
	case Recuperada: {
#pragma omp atomic 
		tRecuperadas++;
		break;
	}
	}
}

// Cambia de posicion a la persona
void Simulador::moverPersonas(int i, int j, int y) {
	int indice;
	int random;
	random_device rd;
	Persona p = matriz[i][j][y];
	random = rd() % 4;
	switch (random) {

	case 0: {
		indice = i + 1;
		break;
	}
	case 1: {
		indice = i - 1;
		break;
	}
	case 2: {
		indice = j + 1;
		break;
	}
	case 3: {
		indice = j - 1;
		break;
	}
	}
	if (indice < 0) {
		indice = tamano - 1;
	}
	else if (indice >= tamano) {
		indice = 0;
	}
	matriz[i][j].erase(matriz[i][j].begin() + y);
	if (random < 2) {
		matriz[indice][j].push_back(p);
	}
	else {
		matriz[i][indice].push_back(p);
	}
}

// Set cantidad de Hilos 
void Simulador::setHilos(int n) {
	if (n > 0) {
		hilos = n;
	}
	else {
		hilos = omp_get_max_threads();
	}
}

int Simulador::getHilos() {
	return hilos;
}
