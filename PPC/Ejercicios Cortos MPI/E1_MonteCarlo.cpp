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
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

//#define DEBUG

void uso(string nombre_prog);

int tiros;
int numsxproc = 0;
double contaTotal = 0;

void calculo(int,int,int);
void get_input(int,int);

void obt_args(
	char*    argv[]        /* in  */,
	int&     dato_salida  /* out */);


int main(int argc, char* argv[]) {
	double local_start, local_finish, local_elapsed, elapsed;
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
	get_input(mid, cnt_proc); // Pregunta al usuario por cantidad de tiros y se las pasa a los procesos

	MPI_Barrier(MPI_COMM_WORLD);
	local_start = MPI_Wtime();

	calculo(numsxproc, mid, cnt_proc);

	local_finish = MPI_Wtime();
	local_elapsed = local_finish - local_start;
	MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (mid == 0)
		cout << "Tiempo transcurrido = " << elapsed << endl;

	/* finalización de la ejecución paralela */
	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	MPI_Finalize();
	return 0;
}  /* main */

void calculo(int nums, int mid, int cnt_proc) {
    int contador = 0;

    for (int i = 0; i < nums; ++i) {
        double x = 1 + rand() % (1001);
		x = x/1000.0;

        double y = 1 + rand() % (1001);
        y = y/1000.0;

        double distancia = (x*x) + (y*y);

        if (distancia <= 1) {
            ++contador;
        }
    }

    if (mid != 0)
		MPI_Send(&contador, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	else {
		contaTotal  = contador;
		for (int i = 1; i < cnt_proc; i++) {
			MPI_Recv(&contador, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			contaTotal += contador;
		}
		double pi = (4 * contaTotal) / (tiros);
		cout << "Estimado: " << pi << endl;
		cout << "Error: " << 4.0*atan(1.0) - pi << endl;
	}
}

void get_input(int my_rank, int comm_sz) {
	if (my_rank == 0) {
		cout << "Ingrese cantidad de tiros" << endl;
		cin >> tiros;
		numsxproc = tiros / comm_sz;
		cin.get(); // por alguna extraña razón queda en el buffer un '\n' que se debe eliminar antes del ignore().
		for (int dest = 1; dest < comm_sz; dest++) {
			MPI_Send(&tiros, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
			MPI_Send(&numsxproc, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
		}
	}
	else { /* my_rank != 0 */
		MPI_Recv(&tiros, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&numsxproc, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}  /* Get_input */
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
