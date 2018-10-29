//#include <iostream>
//#include <omp.h>
//#include <math.h>
//#include <stdlib.h>
//#include <time.h>
//#include <vector>
//#include <algorithm>
//
//using namespace std;
//
//#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
//#pragma warning(disable : 4996)
//
//void genNums();
//void print();
//void mergeSort();
//bool validaCntHilos(int);
//int hilos;
//int elem;
//int tam = 1024;
//vector<int> vec;
//vector<int> copia(tam);
//
//
//
//int main(int argc, char* argv[]) {
//    srand (time(NULL));
//    while (!validaCntHilos(hilos)) {
//        cout << "Digite la cantidad de hilos: ";
//        cin >> hilos;
//    }
//    elem = tam/hilos;
//    genNums();
//    cout << "Vector Original: " << endl;
//    print();
//
//
//#pragma omp parallel num_threads(hilos)
//    {
//        mergeSort();
//    }
//    cout << "Vector Ordenado por Secciones:" << endl;
//    print();
//
//    using namespace std::chrono;
//    steady_clock::time_point t1 = steady_clock::now();
//
//    for(int i = 0; i <hilos-1; ++i) {
//         inplace_merge(vec.begin(), vec.begin() + (elem * (i+2)) - elem, vec.begin() + (elem * (i+2)));
//    }
//
//    steady_clock::time_point t2 = steady_clock::now();
//    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
//
//    cout << "Vector Ordenado Completo:" << endl;
//    print();
//    
//    std::cout << "Duracion " << time_span.count()  << " segundos." << endl;
//
//
//    return 0;
//}
//
//void genNums () {
//    for (int i = 0; i < tam; ++i) {
//        vec.push_back(rand() % 11);
//    }
//}
//
//void print () {
//    for (int i = 0; i < tam; ++i) {
//        cout << vec[i] << " | ";
//    }
//    cout << endl << endl;
//}
//
//bool validaCntHilos(int ch) {
//    return ch >= 1;
//}
//
//void mergeSort() {
//    int max = ((omp_get_thread_num() + 1) * (elem))-1;
//    int min = max - ((elem)-1);
//    sort(vec.begin()+min, vec.begin()+max+1);
//
//#pragma omp critical
//    {
//        cout << "Hilo " << omp_get_thread_num() << " ordena desde la posición " << min << " hasta " << max << endl << endl;
//    }
//
//
//}
