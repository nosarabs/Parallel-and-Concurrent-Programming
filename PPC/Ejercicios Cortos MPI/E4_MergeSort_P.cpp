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
void print(int* vec, int mid, int tam);
void mergeSort(int* vec, int mid, int cantxProc);
void mergeParalelo(int* vec, int mid);

int cantxProc;

void obt_args(
	char*    argv[]        /* in  */, int& tam);

int main(int argc, char* argv[]) {
	double local_start, local_finish, local_elapsed, elapsed;
	int tam;
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

	obt_args(argv, tam);

	/* ejecución del proceso principal */
	local_start = MPI_Wtime();

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
	MPI_Reduce(local, total, tam, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	local_finish = MPI_Wtime();
	local_elapsed = local_finish - local_start;
	MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (mid == 0) {

		for (int i = 0; i < cnt_proc - 1; ++i) {
			inplace_merge(total, total + (cantxProc * (i + 2)) - cantxProc, total + (cantxProc * (i + 2)));
		}
		print(total, mid, tam);

		cout << "Tiempo transcurrido = " << elapsed << endl;
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

void print(int* vec, int mid, int tam) {
	cout << " Imprimiendo vec de: " << mid << endl;
	for (int i = 0; i < tam; i++) {
		cout << vec[i] << ",";
	}
}

void mergeParalelo(int* vec, int mid) {

	//if (mid == 0) {
	//	//MPI_Recv(&tiros, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//}
	//else {

	//}
	////MPI_Send(&tiros, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
	////MPI_Recv(&tiros, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
	char*    argv[]        /* in  */, int& tam) {

	tam = strtol(argv[1], NULL, 10);

#  ifdef DEBUG
	cout << "dato_salida = " << dato_salida << endl;
#  endif
}  /* obt_args */
