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

#include <vector>

using namespace std;



//#define DEBUG



void uso(string nombre_prog);



void obt_args(

	char*    argv[]        /* in  */,

	int&     dato_salida  /* out */);



int main(int argc, char* argv[]) {

	int mid; // id de cada proceso

	int cnt_proc; // cantidad de procesos

	MPI_Status mpi_status; // para capturar estado al finalizar invocación de funciones MPI

	vector<int> vec; // Vector de numeros

	int n;





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

	if (!mid) {

		cout << "N: " << endl;
		cin >> n;
		for (int c = 1; c < cnt_proc; c++) {
			MPI_Send(&n, 1, MPI_INT, c, 0, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	bool primo = true;
	int x = 1;
	vec.resize(n);

	int nxProc = (n - 5) / cnt_proc;

	vec[0] = 2;
	int* local = new int[nxProc*3];

	for (int i = 3; i <= n; i += 2) {
		primo = true;
		for (int j = 3; j <= sqrt(i); j += 2) {
			if (i % j == 0) {
				primo = false;
			}
		}
		if (primo) {
			vec[x] = i;
			++x;
		}
	}

	int suma = 0;
	int indice = 0;
	int cont = 0;
	int j = 0;

	for (int i = 6+mid*nxProc; i <= 5+mid*nxProc + nxProc; ++i) {
		suma = 0;
		j = x - 1;
		if (i % 2 == 0) {
			local[indice] = 0;
			++indice;
			cont = 2;
		}
		else {
			cont = 3;
		}
		while (cont > 0 && j >= 0) {
			if (vec[j]<i - 1) {
				if (suma + vec[j] <= i) {
					if (i - (suma + vec[j]) != 1) {
						local[indice] = vec[j];
						++indice;
						suma += vec[j];
						--cont;
					}
					else {
						--j;
					}
				}
				else {
					--j;
				}
			}
			else {
				--j;
			}
		}
		if (cont >= 1) {
			local[indice] = 0;
			++indice;
		}
	}





	int g = 0;

	for (int y = 0; y < nxProc; ++y) {

		cout << "Numeros Primos para: " << 6+nxProc*mid+y << ": ";

		cout << local[g] << "," << local[g + 1] << "," << local[g + 2] << endl;

		g += 3;

	}



	//int* total = new int[((n - 5) * 3)];

	//MPI_Gather(local, ((n - 5) * 3)/cnt_proc, MPI_INT, total, ((n - 5) * 3), MPI_INT, 0, MPI_COMM_WORLD);



	//if (mid == 0) {

	//	int g = 0;

	//	for (int y = 6; y <= n; ++y) {

	//		cout << "Numeros Primos para: " << y << ": ";

	//		cout << total[g] << "," << total[g + 1] << "," << total[g + 2] << endl;

	//		g += 3;

	//	}

	//}





	cin >> n;



	/* finalización de la ejecución paralela */

	if (mid == 0)

		cin.ignore();

	MPI_Barrier(MPI_COMM_WORLD); // para sincronizar la finalización de los procesos



	MPI_Finalize();

	return 0;

}  /* main */



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
