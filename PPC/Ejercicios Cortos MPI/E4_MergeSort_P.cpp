/* Archivo:      mpi_plantilla.cpp
* Propósito:   ....
*
* Compilación:   mpicxx -g -Wall -o mpi_plantilla mpi_plantilla.cpp
* Ejecución:     mpiexec -n <num_proc> ./mpi_plantilla <secuencia de valores de parámetros>
*
* Entradas:     ...
* Salidas:    ...
*
* Notas:
* 1.  bandera DEBUG produce salida detallada para depuración.
*
*/

#include <mpi.h>
#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <chrono> // para medir el tiempo de ejecución
#include <vector>
#include <algorithm>
using namespace std;

//#define DEBUG

void uso(string nombre_prog);
void genNums(int* vec, int n, int mid);
void print(int* vec, int mid);
void mergeSort(int* vec, int mid, int cantxProc);
void mergeParalelo(int* local, int mid, int cnt_procesos, int cantxProc);

int tam = 20;
int cantxProc;

void obt_args(
	char*    argv[]        /* in  */,
	int&     dato_salida  /* out */);

int main(int argc, char* argv[]) {
	int mid; // id de cada proceso
	int cnt_proc; // cantidad de procesos
	MPI_Status mpi_status; // para capturar estado al finalizar invocación de funciones MPI

						   /* Arrancar ambiente MPI */
	MPI_Init(&argc, &argv);             		/* Arranca ambiente MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &mid); 		/* El comunicador le da valor a id (rank del proceso) */
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_proc);  /* El comunicador le da valor a p (número de procesos) */

#  ifdef DEBUG
	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD);
#  endif

	/* ejecución del proceso principal */
	cantxProc = tam / cnt_proc;
	int* local = new int[tam];
	int* total = new int[tam];

	for (int i = 0; i < tam; i++) {
		local[i] = 0;
		total[i] = 0;
	}

	genNums(local, cantxProc, mid);
	mergeSort(local, mid, cantxProc);

	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	mergeParalelo(local, mid, cnt_proc, cantxProc);
	if (mid == 0) {
		print(local, mid);
	}
	int n;
	cin >> n;

	/* finalización de la ejecución paralela */
	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	MPI_Finalize();
	return 0;
}  /* main */


void genNums(int* vec, int n, int mid) {
	int min = n * mid;
	int max = min + n;
	for (int i = min; i < max; ++i) {
		vec[i] = rand() % 11;
	}
}

void mergeSort(int* vec, int mid, int cantxProc) {
	sort(vec + mid * cantxProc, vec + (mid*cantxProc) + cantxProc);
}

void print(int* vec, int mid) {
	cout << " Imprimiendo vec de: " << mid << endl;
	for (int i = 0; i < tam; i++) {
		cout << vec[i] << ",";
	}
}

void mergeParalelo(int* local, int mid, int cnt_procesos, int cantxProc) {
	cout << "cnt: " << cnt_procesos << endl;
	int* temporal = new int[tam];
	int j = 0;
	int mitad = 0;
	int pot = 0;
	int pot2 = 0;
	bool enviado = false;
	int i = 1;
	while (i <= log2(cnt_procesos) && enviado == false) {
		j = cantxProc * (mid + 1);
		pot = pow(2, i);
		pot2 = pow(2, i - 1);
		if (mid%pot == 0) {
			//Recibe
			cout << endl;
			cout << "Recv: " << mid << " de: " << mid + pot2 << " Pot y Pot2 " << pot << " | " << pot2  << endl;
			MPI_Recv(temporal, tam, MPI_INT, mid + pot2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			mitad = j;

			for (j; j < tam; ++j) {
				local[j] = temporal[j];
			}

			for (int h = 0; h < tam; ++h) {
				cout << "Mid: " << mid << " " << local[h] << endl;
			}
			if (mid == 0 && i == 2) {
				cout << "Inicio " << cantxProc * mid << " Mitad " << mitad << " Final " << local+j << endl;
			}
			inplace_merge(local + cantxProc * mid, local + mitad, local + j);
		}
		else if (mid > 0) {
			//manda
			cout << "Send: " << mid << " a: " << mid - pot2 << " Pot y Pot2 " << pot << " | " << pot2 << endl;
			MPI_Send(local, tam, MPI_INT, mid - pot2, 0, MPI_COMM_WORLD);
			enviado = true;
		}
		i++;
	}
	//for (int i = 1; i <= log2(cnt_procesos); ++i) {
	//	pot = pow(2, i);
	//	pot2 = pow(2, i - 1);
	//	if (mid%pot == 0) {
	//		//Recibe
	//		cout << "Recv: " << mid << " de: " << mid+pot2 << endl;
	//		MPI_Recv(temporal, tam, MPI_INT, mid + pot2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//		mitad = j;

	//		for (j; j < tam; ++j) {
	//			local[j] = temporal[j];
	//		}

	//		for (int h = 0; h < tam; ++h) {
	//			cout << "Mid: " << mid << " " << local[h] << endl;
	//		}

	//		inplace_merge(local + cantxProc * mid, local + mitad, local + j);
	//	}
	//	else if (mid > 0) {
	//		//manda
	//		cout << "Send: " << mid << " a: " << mid-pot2 << endl;
	//		MPI_Send(local, tam, MPI_INT, mid - pot2, 0, MPI_COMM_WORLD);
	//		break;
	//	}
	//}
}


/*---------------------------------------------------------------------
* REQ: N/A
* MOD: N/A
* EFE: despliega mensaje indicando cómo ejecutar el programa y pasarle parámetros de entrada.
* ENTRAN:
*		nombre_prog:  nombre del programa
* SALEN: N/A
*/
void uso(string nombre_prog /* in */) {
	cerr << nombre_prog.c_str() << " secuencia de parámetros de entrada" << endl;
	exit(0);
}  /* uso */

   /*---------------------------------------------------------------------
   * REQ: N/A
   * MOD: dato_salida
   * EFE: obtiene los valores de los argumentos pasados por "línea de comandos".
   * ENTRAN:
   *		nombre_prog:  nombre del programa
   * SALEN:
   *		dato_salida: un dato de salida con un valor de argumento pasado por "línea de comandos".
   */
void obt_args(
	char*    argv[]        /* in  */,
	int&     dato_salida  /* out */) {

	dato_salida = strtol(argv[1], NULL, 10); // se obtiene valor del argumento 1 pasado por "línea de comandos".

#  ifdef DEBUG
	cout << "dato_salida = " << dato_salida << endl;
#  endif
}  /* obt_args */
