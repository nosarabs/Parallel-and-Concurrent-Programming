#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
#pragma warning(disable : 4996)

void hello();  /* Thread function */
void inicio();
void calculo(int);
bool validaCntHilos(int);

int hilos = 0;
int tiros = 0;

int main(int argc, char* argv[]) {
    srand (time(NULL));
    
    while (!validaCntHilos(hilos)) {
        cout << "Digite la cantidad de hilos: ";
        cin >> hilos;
        
        cout << "Digite la cantidad de tiros: ";
        cin >> tiros;
    }
    
    inicio();
    
    
    
    
# pragma omp parallel num_threads(hilos)
    {
        # pragma omp single
        {
            if (tiros%hilos != 0) {
                int numsxhilo = floor(tiros/hilos);
                int resto = tiros - (numsxhilo * hilos);
                cout << "Extra para el Hilo Master: " << resto << endl;
                calculo(resto);
                cout << "Números por hilo: " << numsxhilo << endl;
            }
        }
    }
    
    cin >> hilos;
    return 0;
}  /* main */




bool validaCntHilos(int ch) {
    return ch >= 1;
}


void calculo(int nums) {
    for (int i = 0; i < nums; ++i) {
        float x = ((float) rand()) / (float) RAND_MAX;
        x = (-1) + x * 2;
        
        float y = ((float) rand()) / (float) RAND_MAX;
        y = (-1) + x * 2;
        
        float distancia = (x*x) + (y*y);
    }
}

void inicio() {
    int numsxhilo = 0;
    if (tiros%hilos != 0) {
        numsxhilo = floor(tiros/hilos);
        int resto = tiros - (numsxhilo * hilos);
        cout << "Extra para el Hilo Master: " << resto << endl;
        cout << "Números por hilo: " << numsxhilo << endl;
    }
}









