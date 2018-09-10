#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
#pragma warning(disable : 4996)

bool validaCntHilos(int);

int hilosP = 0;
int hilosC = 0;


int main(int argc, char* argv[]) {
    srand (time(NULL));
    
    while (!validaCntHilos(hilosP)) {
        cout << "Hilos Productores: ";
        cin >> hilosP;
        
        cout << "Hilos Consumidores: ";
        cin >> hilosC;

    }
    
    
    
    
    return 0;
}

bool validaCntHilos(int ch) {
    return ch >= 1;
}











