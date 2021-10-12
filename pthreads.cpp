/*****************************************************************************
* FILE: test.cpp
* DESCRIPTION:
*	This program determinates the matricial product of a matrix of size 
*	n, where n is a random number between 3 and 8. This is done with a
*	serial implementation which can be understanded as a "standard"
*	programming and with a paralel implementation using threads.

*	This program also determinates a pi approximation value using the 
*	Gregory-Leibniz series (reference at the end of the file). This is 
*	realized by two approaches: using serial implementation (standard 
*	programming) and paralel implementation with pthreads.
*	Finally, we compare the results of both implementations with a
*	a certain decimal precision to determinate if the results are equal.

*	[Authors]:
		Darieth Fonseca.
		Jonathan Ramírez.
		
******************************************************************************/

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define num_threads 10000
#define iterations 10000

/* --------------------------------- Structure of parameters -----------------------------------*/
struct loop_param
{
	/* Serial */
	/* Matrix */
	int N_s;
	int *vect_s;
	int **matriz_s;
	int *vect_resuls;
	/* Pi */
	double pi_ser;
	
	/* Paralel */
	/* Matrix */
	int N_p;
	int *vect_p;
	int **matriz_p;
	int *vect_resulp;
	/* Pi */
	double pi_par;
};

/* ----------------------------------- Global variables ---------------------------------------*/
struct loop_param thread_data;
pthread_mutex_t mutexloop;

/* ---------------------------------------- Methods -------------------------------------------*/

/* ---------------------------------------- Matrix ---------------------------------------------*/
void mult_matrices_serial(int* vector, int **matriz, int N, int* matriz_resul){
    //Valores iniciales
    int i = 0;
    int j = 0;
    //se recorren las filas
    while(i < N){
        j = 0;
        //se recorren las columnas dentro de cada fila
        while(j < N){
            matriz_resul[i] = matriz_resul[i] + vector[j]*matriz[i][j];
            j = j + 1;
        }
        i = i + 1;
    }
}


void *mult_matrix(void *arg){
	/* Internal parameter */
	long param;
	param = (long)arg;
	int i = 0;
	pthread_mutex_lock (&mutexloop);
	while(i < thread_data.N_p){
		thread_data.vect_resulp[param] = thread_data.vect_resulp[param] + thread_data.vect_p[i]*thread_data.matriz_p[param][i];
		i = i + 1;
	}
	pthread_mutex_unlock (&mutexloop);
	
	pthread_exit((void *)0);
}

void rev_matrix(int N){
	int i = 0;
	int j = 0;
	int bandera = 0;
	while(i < N){
		if(thread_data.vect_resulp[i] != thread_data.vect_resuls[i]){
			bandera = 1;
		}
		i = i + 1;
	}
	if(bandera == 1){
		printf("Hubo un error al realizar la multiplicacion de matrices.\n");
	}
	else{
		printf("\n\nLa multiplicacion de matrices con ambos metodos se realizo de forma satisfactoria.\n\n");
	}
}

/* ---------------------------------------- Pi appproximation ---------------------------------------------*/

/* 
	* Caculates the approximation of pi using the Leibniz- series.
	* This is the serial implementation.
	* [input] iter: it's the number of sum terms; the number of iterations.
	* [output] void: the method has no outputs.
*/

void pi_serial_approx (int iter)
{
	/* Set pi_ser to cero */
	thread_data.pi_ser = 0;
	/* Loop for the sum */
	for(int i = 0; i < iter; i++)
	{
		thread_data.pi_ser += 4*((pow(-1,i))/(2*i+1));
	}
}

/* 
	* This method is used by a single thread. 
	* Calculates a single term of the sum and adds it to the variable
	* who contains the value of pi.
	* [input] arg: a void pointer which is the index value of the sum term.
	* [output] void: there's no output.
*/

void *calculation(void *arg)
{
	/* Internal parameter */
	long iter;
	iter = (long)arg;
	double sum;
	
	sum = 0;
	sum = 4*((pow(-1,iter))/(2*iter+1));
	
	pthread_mutex_lock (&mutexloop);
	thread_data.pi_par += sum; 
	pthread_mutex_unlock (&mutexloop);

	pthread_exit((void *)0);
}

/*
	*Print the results of the pi values obtained by the serial and paralel implementations.
	* [input] serial_pi, paralel_pi: the values of pi determinated by the serial and paralel executions.
	* [output] void: there's no output.
*/

void pi_print(double serial_pi, double paralel_pi)
{	
	/* Print results */
	printf("-------------------Pi approximation value-------------------\n\n");
	printf("Pi value approximation for serial implementation: %f \n", serial_pi);
	printf("Pi value approximation for paralel implementation: %f \n", paralel_pi);		
}

/*---------------------------------------END-------------------------------------------*/

using namespace std;

int main(int argc, char *argv[])
{
	/*------------------------------- Pthread variables ------------------------------------*/
	void *status;
	pthread_mutex_init(&mutexloop, NULL);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	/*---------------------------------------END-------------------------------------------*/
	
	/*---------------------------------------Matricial product-------------------------------------------*/
	
	/*Creación de las matrices*/
	pthread_attr_t attr1;
	pthread_attr_init(&attr1);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_JOINABLE);
	int i = 0;
    	int j = 0;
	srand (time(NULL)); 
    	thread_data.N_p = rand() % 6 +3; //numero aleatorio entre 3 y 8
   	 thread_data.N_s = thread_data.N_p;
	int N = thread_data.N_s;
	
	//Creación de las matrices y los vectores
	//Vectores
    	thread_data.vect_p = (int *) malloc (thread_data.N_p*sizeof(int));
	thread_data.vect_s = (int *) malloc (thread_data.N_s*sizeof(int));
	thread_data.vect_resuls = (int *) malloc (thread_data.N_p*sizeof(int));
	thread_data.vect_resulp = (int *) malloc (thread_data.N_s*sizeof(int));
	while(i < N){
       	thread_data.vect_s[i] = rand() % 101; //valores aleatorios entre 0 y 100
       	thread_data.vect_p[i] = thread_data.vect_s[i];
		thread_data.vect_resulp[i] = 0; //se inicializan en 0 los valores del vector de resultados
       	thread_data.vect_resuls[i] = 0;
		i = i + 1;
  	  }
	printf("\n\n\n\n");
	
	//Matrices
    	i = 0;
   	thread_data.matriz_p = (int **)malloc (N*sizeof(int *));
	thread_data.matriz_s = (int **)malloc (N*sizeof(int *));
    	for (i=0;i<N;i++){
        	thread_data.matriz_p[i] = (int *) malloc (N*sizeof(int));
		thread_data.matriz_s[i] = (int *) malloc (N*sizeof(int));
    	}
   	i = 0;
    	while(i < N){
        	j = 0;
        	while(j < N){
           		thread_data.matriz_p[i][j] = rand() % 101;
            		thread_data.matriz_s[i][j] = thread_data.matriz_p[i][j];
			j = j + 1;
        	}
        i = i + 1;
    	}
	/*---------------------------------------END-------------------------------------------*/
	//Calculo de matriz resultados con la funcion serial
	mult_matrices_serial(thread_data.vect_s,thread_data.matriz_s,thread_data.N_s,thread_data.vect_resuls);
	printf("-------------------Producto matricial-------------------\n\n");
	printf("MATRIZ SERIAL\n\n");
	i = 0;
    	while(i < N){
        	printf("El valor de la multiplicacion es  %d en la fila %d\n", thread_data.vect_resuls[i], i);
        	i = i + 1;
    	}
    	
	//calculo de matriz resultados con funcion paralela
	/* Array of threads */
	pthread_t threads_m[N];
	/* threads rutine. */
	long fi = 0;
	int rc1;
	for(int t=0;t<N;t++)
	{	
		fi = t;
		rc1 = pthread_create(&threads_m[t], NULL, mult_matrix, (void *)fi);
		if(rc1)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc1);
			exit(-1);
		}			
	}
	pthread_attr_destroy(&attr1);
	/* Join threads */
	for(int i=0;i<N;i++) 
	{
		pthread_join(threads_m[i], &status);
  	}
	/*-----------------------------------------END------------------------------------*/	
	
	//Impresion de matrices
	printf("\n\nMATRIZ PARALELA\n\n");
	i = 0;
    	while(i < N){
        	printf("El valor de la multiplicacion es  %d en la fila %d\n", thread_data.vect_resulp[i], i);
        	i = i + 1;
    	}
	rev_matrix(N);
	/*---------------------------------------END-------------------------------------------*/
	
	/*---------------------------------------Pi value-------------------------------------------*/
	

	/*-----------------------------Pi approximation (Serial)-------------------------------*/
	pi_serial_approx(iterations);
	/*---------------------------------------END-------------------------------------------*/
	
	/*-----------------------------Pi approximation (Paralel)-------------------------------*/
	/* Array of threads */
	pthread_t threads[num_threads];
	
	/* threads rutine: 
		each thread will do a single iteration of the algorithm.*/
	int rc;
	long t;
	for(t=0;t<num_threads;t++)
	{
		rc = pthread_create(&threads[t], &attr, calculation, (void *)t);
		if(rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}			
	}
	pthread_attr_destroy(&attr);
	
	/* Join threads */
	for(int i=0;i<num_threads;i++) 
	{
  		pthread_join(threads[i], &status);
  	}
	/*---------------------------------------END-------------------------------------------*/
		
	/* ----------------------Final value: pi; print results -------------------------------*/
	
	pi_print(thread_data.pi_ser, thread_data.pi_par);
	
	/*---------------------------------------END-------------------------------------------*/
	
	/* ------------------------------ Validations ------------------------------------------*/
	
	/* ------------------------------ Pi approximation -------------------------------------*/
	
	if(abs(thread_data.pi_ser - thread_data.pi_par)<0.000001)
	{
		printf(" The pi values for serial and paralel implementation are the same. \n");
		printf("-----------------------------------END-----------------------------------\n\n");
		return 0;
	}
	else
	{
		printf(" The pi values for serial and paralel implementation aren't the same. \n");
		printf("-----------------------------------END-----------------------------------\n\n");
		return 1;
	}
	/*---------------------------------------END-------------------------------------------*/
	
	pthread_mutex_destroy(&mutexloop);
	pthread_exit(NULL);
}


/*----------------------------------- References -----------------------------------
*
*	https://mathworld.wolfram.com/LeibnizSeries.html
*
----------------------------------------------------------------------------------*/
