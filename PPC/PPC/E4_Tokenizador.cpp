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
bool insertado = false;
queue_lk<string> cola;


int main(int argc, char* argv[]) {

    while (!validaCntHilos(hilosP)) {
        cout << "Hilos Productores: ";
        cin >> hilosP;

        cout << "Hilos Consumidores: ";
        cin >> hilosC;

    }

#pragma omp parallel num_threads(5)
    {
        if (omp_get_thread_num() < hilosP) {
            producir();
        }
        else {
            while (insertado) {}
            consumir();
        }
    }

    cout << endl;
    int l;
    cin >> l;
    return 0;
}

bool validaCntHilos(int ch) {
    return ch >= 1;
}

void producir() {
    string line;
    ifstream myfile;
    string dir = "C:/Users/b62456/source/repos/Project1/";
    dir += to_string(omp_get_thread_num());
    dir += ".txt";
    cout << dir << endl;

    myfile.open(dir);
    while (getline(myfile, line)) {
        cola.set_lock();
        cola.push(queue_lk<string>::msg_t<string>(omp_get_thread_num(), line));
        cout << "Hilo " << omp_get_thread_num() << " encolando: " << line << endl;
        cola.unset_lock();
        insertado = true;
    }

}

void consumir() {
    while(!cola.empty()) { // significa que ya almenos un hilo escribio en la cola.

        cola.set_lock();
        queue_lk<string>::msg_t<string> s = cola.front();
        cola.pop();
        cola.unset_lock();
        int i = 0;

#pragma omp critical
        {
            cout << "Hilo " << omp_get_thread_num() << ": " << endl;
            while (i <= s.msg.length()) {
                if (s.msg[i] != ' ') {
                    cout << s.msg[i];
                }
                else {
                    cout << endl;
                }
                ++i;
            }
            cout << endl;
        }
    }
}
