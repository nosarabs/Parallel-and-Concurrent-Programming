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
#include <stdlib.h>
#include <vector>
#include <string>
#include <time.h>
#include <omp.h>
using namespace std;

//#define DEBUG

void uso(string nombre_prog);

void gen_data(
	float   min_meas    /* in  */,
	float   max_meas    /* in  */,
	vector<float>&   data /* out */,
	int     data_count  /* in  */);

void gen_bins(
	float min_meas      /* in  */,
	float max_meas      /* in  */,
	vector<float>& bin_maxes   /* out */,
	vector<int>&   bin_counts  /* out */,
	int   bin_count     /* in  */);

int which_bin(
	float    data         /* in */,
	vector<float>&    bin_maxes  /* in */,
	int      bin_count    /* in */,
	float    min_meas     /* in */);

void print_histo(
	vector<float>    bin_maxes   /* in */,
	int* total   /* in */,
	int      bin_count     /* in */,
	float    min_meas      /* in */);

void get_input(int, int, int, int, float, float);

void obt_args(
	char*    argv[]        /* in  */,
	int&     bin_count_p   /* out */,
	float&   min_meas_p    /* out */,
	float&   max_meas_p    /* out */,
	int&     data_count_p  /* out */);


int main(int argc, char* argv[]) {

	double local_start, local_finish, local_elapsed, elapsed;
	int bin_count, bin;          // cantidad de bins, bin actual, bin == rango
	float min_meas, max_meas;	// valor inferior de datos, valor superior de datos
	int data_count;              // cantidad de datos
	vector<float> bin_maxes;	// vector de m�ximos por bin
	vector<int> bin_counts;		// vector para contar valores por bin
	vector<int> bin_countsTotal;
	vector<float> data;          // vector de datos

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
	obt_args(argv, bin_count, min_meas, max_meas, data_count);

	local_start = MPI_Wtime();

	if (!mid) {
		if (argc != 5) uso(argv[0]);
	}

	data_count = data_count / cnt_proc;
	bin_maxes.resize(bin_count);
	bin_counts.resize(bin_count);
	data.resize(data_count);
	int* local = new int[bin_count];
	int* total = new int[bin_count];

	gen_data(min_meas, max_meas, data, data_count);
	gen_bins(min_meas, max_meas, bin_maxes, bin_counts, bin_count);


	for (int i = 0; i < data_count; ++i) {
		bin = which_bin(data[i], bin_maxes, bin_count, min_meas);
		bin_counts[bin]++;
		local[bin] = bin_counts[bin];
	}

	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	MPI_Reduce(local, total, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	local_finish = MPI_Wtime();
	local_elapsed = local_finish - local_start;
	MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (mid == 0)
		cout << "Tiempo transcurrido = " << elapsed << endl;

	if (!mid) {
		print_histo(bin_maxes, total, bin_count, min_meas);
	}

	/* finalización de la ejecución paralela */
	if (mid == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos

	MPI_Finalize();
	return 0;
}  /* main */

   /*------------------------------------------------------------------------
   * Function:  gen_data
   * Purpose:   Generate random floats in the range min_meas <= x < max_meas
   * In args:   min_meas:    the minimum possible value for the data
   *            max_meas:    the maximum possible value for the data
   *            data_count:  the number of measurements
   * Out arg:   data:        the actual measurements
   */
void gen_data(
	float   min_meas    /* in  */,
	float   max_meas    /* in  */,
	vector<float>&   data /* out */,
	int     data_count  /* in  */) {

	srand(0);

	for (int i = 0; i < data_count; i++)
		data[i] = min_meas + (max_meas - min_meas)*rand() / ((double)RAND_MAX);

}  /* gen_data */

   /*---------------------------------------------------------------------
   * Function:  gen_bins
   * Purpose:   Compute max value for each bin, and store 0 as the
   *            number of values in each bin
   * In args:   min_meas:   the minimum possible measurement
   *            max_meas:   the maximum possible measurement
   *            bin_count:  the number of bins
   * Out args:  bin_maxes:  the maximum possible value for each bin
   *            bin_counts: the number of data values in each bin
   */
void gen_bins(
	float min_meas      /* in  */,
	float max_meas      /* in  */,
	vector<float>& bin_maxes   /* out */,
	vector<int>&   bin_counts  /* out */,
	int   bin_count     /* in  */) {
	float bin_width;

	bin_width = (max_meas - min_meas) / bin_count;

	for (int i = 0; i < bin_count; i++) {
		bin_maxes[i] = min_meas + (i + 1)*bin_width;
		bin_counts[i] = 0;
	}

#  ifdef DEBUG
	cout << "bin_maxes = ";
	for (int i = 0; i < bin_count; i++)
		cout << " " << bin_maxes[i];
	cout << endl;
#  endif
}  /* gen_bins */

   /*---------------------------------------------------------------------
   * Function:  which_bin
   * Purpose:   Use binary search to determine which bin a measurement
   *            belongs to
   * In args:   data:       the current measurement
   *            bin_maxes:  list of max bin values
   *            bin_count:  number of bins
   *            min_meas:   the minimum possible measurement
   * Return:    the number of the bin to which data belongs
   * Notes:
   * 1.  The bin to which data belongs satisfies
   *
   *            bin_maxes[i-1] <= data < bin_maxes[i]
   *
   *     where, bin_maxes[-1] = min_meas
   * 2.  If the search fails, the function prints a message and exits
   */
int which_bin(
	float    data         /* in */,
	vector<float>&    bin_maxes  /* in */,
	int      bin_count    /* in */,
	float    min_meas     /* in */) {

	int bottom = 0, top = bin_count - 1;
	int mid;
	float bin_max, bin_min;

	while (bottom <= top) {
		mid = (bottom + top) / 2;
		bin_max = bin_maxes[mid];
		bin_min = (mid == 0) ? min_meas : bin_maxes[mid - 1];
		if (data >= bin_max)
			bottom = mid + 1;
		else if (data < bin_min)
			top = mid - 1;
		else
			return mid;
	}
	/* Whoops! */
	cerr << "Data = " << data << " doesn't belong to a bin!" << endl;
	cerr << "Quitting" << endl;
	exit(-1);
}  /* which_bin */

   /*---------------------------------------------------------------------
   * Function:  print_histo
   * Purpose:   Print a histogram.  The number of elements in each
   *            bin is shown by an array of X's.
   * In args:   bin_maxes:   the max value for each bin
   *            bin_counts:  the number of elements in each bin
   *            bin_count:   the number of bins
   *            min_meas:    the minimum possible measurment
   */
void print_histo(
	vector<float>    bin_maxes   /* in */,
	int * total   /* in */,
	int      bin_count     /* in */,
	float    min_meas      /* in */) {
	float bin_max, bin_min;

	for (int i = 0; i < bin_count; i++) {
		bin_max = bin_maxes[i];
		bin_min = (i == 0) ? min_meas : bin_maxes[i - 1];
		printf("%.3f-%.3f:\t", bin_min, bin_max);
		for (int j = 0; j < total[i]; j++)
			cout << "X";
		cout << endl;
	}
}  /* print_histo */


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
	int&     bin_count_p   /* out */,
	float&   min_meas_p    /* out */,
	float&   max_meas_p    /* out */,
	int&     data_count_p  /* out */) {

	bin_count_p = strtol(argv[1], NULL, 10);
	min_meas_p = strtof(argv[2], NULL);
	max_meas_p = strtof(argv[3], NULL);
	data_count_p = strtol(argv[4], NULL, 10);

}  /* get_args */
