#include <mpi.h>
 #include <iostream>
 #include <stdlib.h>
 #include <iostream>
 #include <vector>
 #include <string>
 #include <chrono> // para medir el tiempo de ejecución
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
                  vector<int>     bin_counts   /* in */,
                  int      bin_count     /* in */,
                  float    min_meas      /* in */);

 void obt_args(
 	char*    argv[]        /* in  */,
 	int&     dato_salida  /* out */);
 @@ -29,7 +59,14 @@ int main(int argc, char* argv[]) {
 	int cnt_proc; // cantidad de procesos
 	MPI_Status mpi_status; // para capturar estado al finalizar invocación de funciones MPI

 	/* Arrancar ambiente MPI */
 	int bin_count, bin;          // cantidad de bins, bin actual, bin == rango
 	float min_meas, max_meas; // valor inferior de datos, valor superior de datos
 	vector<float> bin_maxes;  // vector de m�ximos por bin
 	vector<int> bin_counts;   // vector para contar valores por bin
 	int data_count;              // cantidad de datos
 	vector<float> data;          // vector de datos

 						   /* Arrancar ambiente MPI */
 	MPI_Init(&argc, &argv);             		/* Arranca ambiente MPI */
 	MPI_Comm_rank(MPI_COMM_WORLD, &mid); 		/* El comunicador le da valor a id (rank del proceso) */
 	MPI_Comm_size(MPI_COMM_WORLD, &cnt_proc);  /* El comunicador le da valor a p (número de procesos) */
 @@ -42,6 +79,24 @@ int main(int argc, char* argv[]) {

 	/* ejecución del proceso principal */

 	/* Allocate arrays needed */

 	//bin_maxes.resize(bin_count);
 	//bin_counts.resize(bin_count);
 	//data.resize(data_count);

 	///* Generate the data */
 	//gen_data(min_meas, max_meas, data, data_count);

 	///* Create bins for storing counts */
 	//gen_bins(min_meas, max_meas, bin_maxes, bin_counts, bin_count);

 	///* Count number of values in each bin */
 	//for (int i = 0; i < data_count; i++) {
 	//	bin = which_bin(data[i], bin_maxes, bin_count, min_meas);
 	//	bin_counts[bin]++;
 	//}

 	/* finalización de la ejecución paralela */
 	if (mid == 0)
 		cin.ignore();
 @@ -83,3 +138,132 @@ void obt_args(
 	cout << "dato_salida = " << dato_salida << endl;
 #  endif
 }  /* obt_args */

    /*---------------------------------------------------------------------
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

    #pragma omp parallel for num_threads(hilos)
        {
            for (int i = 0; i < data_count; i++)
                data[i] = min_meas + (max_meas - min_meas)*rand() / ((double)RAND_MAX);
        }



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
        cerr << "Data = "<< data << " doesn't belong to a bin!" << endl;
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
 	   vector<int>     bin_counts   /* in */,
 	   int      bin_count     /* in */,
 	   float    min_meas      /* in */) {
 	   float bin_max, bin_min;

 	   for (int i = 0; i < bin_count; i++) {
 		   bin_max = bin_maxes[i];
 		   bin_min = (i == 0) ? min_meas : bin_maxes[i - 1];
 		   printf("%.3f-%.3f:\t", bin_min, bin_max);
 		   for (int j = 0; j < bin_counts[i]; j++)
 			   cout << "X";
 		   cout << endl;
 	   }
    }
