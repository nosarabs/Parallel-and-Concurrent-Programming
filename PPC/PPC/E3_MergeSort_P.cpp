#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
#pragma warning(disable : 4996)

void genNums();
void print();
void mergeSort();
void mergeParalelo();
bool validaCntHilos(int);
int hilos;
int nHilos;
int elem;
int tam = 32;
vector<int> vec;
vector<int> copia(tam);



int main(int argc, char* argv[]) {
    srand (time(NULL));
    while (!validaCntHilos(hilos)) {
        cout << "Digite la cantidad de hilos: ";
        cin >> hilos;
    }
    elem = tam/hilos;
    genNums();
    cout << "Vector Original: " << endl;
    print();
    
    
#pragma omp parallel num_threads(hilos)
    {
        mergeSort();
    }
    cout << "Vector Ordenado por Secciones:" << endl;
    print();
    
    mergeParalelo();

    cout << "Vector Ordenado Completo:" << endl;
    print();
    
    
    return 0;
}

void genNums () {
    for (int i = 0; i < tam; ++i) {
        vec.push_back(rand() % 11);
    }
}

void print () {
    for (int i = 0; i < tam; ++i) {
        cout << vec[i] << " | ";
    }
    cout << endl << endl;
}

bool validaCntHilos(int ch) {
    return ch >= 1;
}

void mergeSort() {
    int max = ((omp_get_thread_num() + 1) * (elem))-1;
    int min = max - ((elem)-1);
    sort(vec.begin()+min, vec.begin()+max+1);
#pragma omp critical
    {
        cout << "Hilo " << omp_get_thread_num() << " ordena desde la posición " << min << " hasta " << max << endl << endl;
    }
}

void mergeParalelo(){
    nHilos = hilos;
    while(log(nHilos)) {
        nHilos = nHilos/2;
        #pragma omp parallel num_threads(nHilos)
        {
            int min = (tam/nHilos)*omp_get_thread_num();
            int max = min + (tam/nHilos);
            //inplace_merge(vec.begin()+min, vec.begin()+ (max-min)/2, vec.begin()+max);
            
            #pragma omp critical
            {
                cout << "Hilo " << omp_get_thread_num() << " ordena desde la posición " << min << " hasta " << max << endl << endl;
            }
        }
        
    }
}











