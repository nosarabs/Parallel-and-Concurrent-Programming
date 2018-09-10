#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "queue_lk.h"
using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
#pragma warning(disable : 4996)

bool validaCntHilos(int);
void producir();
void consumir();

int hilosP = 3;
int hilosC = 2;
queue_lk<string> cola;


int main(int argc, char* argv[]) {

    while (!validaCntHilos(hilosP)) {
        cout << "Hilos Productores: ";
        cin >> hilosP;

        cout << "Hilos Consumidores: ";
        cin >> hilosC;

    }
    
#pragma omp parallel num_threads(hilosP)
    {
        producir();
    }
    
    
#pragma omp parallel num_threads(hilosC)
    {
        consumir();
    }

    cout << endl;
    return 0;
}

bool validaCntHilos(int ch) {
    return ch >= 1;
}

void producir() {
    string line;
    ifstream myfile;
    string dir = "/Users/Berta/Documents/GitHub/Parallel-and-Concurrent-Programming/PPC/PPC/";
    dir += to_string(omp_get_thread_num());
    dir += ".txt";

    myfile.open(dir);
    while(getline(myfile,line)){
        cola.set_lock();
        cola.push(line);
        cout << "Hilo " << omp_get_thread_num() << " encolando: " << line << endl;
        cola.unset_lock();
    }

}

void consumir() {
    if (!cola.empty()) { // significa que ya almenos un hilo escribio en la cola.
        
//        cola.set_lock();
//        string s = cola.back().msg;
//        cola.pop();
//        cola.unset_lock();
       
        string s = "Hola amigos como estan";
        int i = 0;
        #pragma omp critical
        {
            cout << "Hilo " << omp_get_thread_num() << ": " << endl;
            while (i <= s.length()) {
                if (s[i] != ' ') {
                    cout << s[i];
                } else {
                    cout << endl;
                }
                ++i;
            }
            cout << endl;
        }
    }
}

