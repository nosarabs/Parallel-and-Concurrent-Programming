//#include <iostream>
//#include <omp.h>
//#include <math.h>
//#include <stdlib.h>
//#include <time.h>
//using namespace std;
//
//#define _CRT_SECURE_NO_WARNINGS 1 // para deshabilitar errores por uso de funciones deprecated sobre CRT o consola
//#pragma warning(disable : 4996)
//
//void hello();  /* Thread function */
//void calculo(int);
//bool validaCntHilos(int);
//
//int hilos = 0;
//int tiros = 0;
//int numsxhilo = 0;
//double contaTotal = 0;
//
//int main(int argc, char* argv[]) {
//    srand (time(NULL));
//    
//    while (!validaCntHilos(hilos)) {
//        cout << "Digite la cantidad de hilos: ";
//        cin >> hilos;
//        
//        cout << "Digite la cantidad de tiros: ";
//        cin >> tiros;
//    }
//    
//    
//    if (tiros%hilos != 0) {
//        numsxhilo = floor(tiros/hilos);
//        int resto = tiros - (numsxhilo * hilos);
//        cout << "Extra para el Hilo Master: " << resto << endl;
//        cout << "Números por hilo: " << numsxhilo << endl;
//        cout << "Calculo Master:" << endl;
//        calculo(resto);
//    } else {
//        numsxhilo = tiros/hilos;
//    }
//    
//# pragma omp parallel num_threads(hilos)
//    {
//        calculo(numsxhilo);
//    }
//    double pi = (4 * contaTotal) / (tiros);
//    cout << "Estimado: " << pi << endl;
//    cout << "Error: " << M_PI - pi << endl;
//    
//    
//    cin >> hilos;
//    return 0;
//}
//
//
//
//
//bool validaCntHilos(int ch) {
//    return ch >= 1;
//}
//
//
//void calculo(int nums) {
//    int contador = 0;
//    
//    for (int i = 0; i < nums; ++i) {
//        double x = 1 + rand() % (1001);
//        x = x/1000.0;
//        
//        double y = 1 + rand() % (1001);
//        y = y/1000.0;
//        
//        double distancia = (x*x) + (y*y);
//        
//        if (distancia <= 1) {
//            ++contador;
//        }
//    }
//    # pragma omp critical
//    {
//        contaTotal += contador;
//        cout << "Hilo "<< omp_get_thread_num() << " " << "Contador Local: "<< contador << " Contador Total: "<<contaTotal << endl;
//    }
//}










