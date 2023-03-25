#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "timing.h"

typedef struct {
    int start;  // Índice del primer elemento de la parte del vector a procesar
    int end;    // Índice del último elemento de la parte del vector a procesar
    int n;      // Número de elementos del vector
    int nsweeps;
    double* u;  // Vector que contiene la solución
    double* f;  // Vector que contiene los términos fuente
    pthread_barrier_t* barrier;  // Barrera para sincronizar los hilos
} thread_data_t;

/* --
 * Do nsweeps sweeps of Jacobi iteration on a 1D Poisson problem
 * 
 *    -u'' = f
 *
 * discretized by n+1 equally spaced mesh points on [0,1].
 * u is subject to Dirichlet boundary conditions specified in
 * the u[0] and u[n] entries of the initial vector.
 */
void* jacobi_thread(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;
    int start = data->start;
    int end = data->end;
    int n = data->n;
    double* u = data->u;
    double* f = data->f;
    pthread_barrier_t* barrier = data->barrier;

    double h = 1.0 / n;
    double h2 = h*h;
    double* utmp = (double*) malloc((end-start+1) * sizeof(double));

    for (int sweep = 0; sweep < data->nsweeps; sweep += 2) {
        // Old data in u; new data in utmp
        for (int i = start; i <= end; ++i) {
            utmp[i-start] = (u[i-1] + u[i+1] + h2*f[i])/2;
        }
        pthread_barrier_wait(barrier);  // Esperar a que todos los hilos hayan terminado

        // Old data in utmp; new data in u
        for (int i = start; i <= end; ++i) {
            u[i] = (utmp[i-start-1] + utmp[i-start+1] + h2*f[i])/2;
        }
        pthread_barrier_wait(barrier);  // Esperar a que todos los hilos hayan terminado
    }

    free(utmp);
    pthread_exit(NULL);
}


void write_solution(int n, double* u, const char* fname)
{
    int i;
    double h = 1.0 / n;
    FILE* fp = fopen(fname, "w+");
    for (i = 0; i <= n; ++i)
        fprintf(fp, "%g %g\n", i*h, u[i]);
    fclose(fp);
}


int main(int argc, char** argv)
{
    int i;
    int n, nsteps;
    double* u;
    double* f;
    double h;
    timing_t tstart, tend;
    char* fname;
    
    //Varibles para el cambio a hilos
    int nthreads;
    pthread_t* threads;
    thread_data_t* thread_data;


    /* Process arguments */
    n      = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    fname  = (argc > 3) ? argv[3] : NULL;
    nthreads = (argc > 4) ? atoi(argv[4]) : 2; // numero de hilos, por defecto 4
    h      = 1.0/n;

    /* Allocate and initialize arrays */
    u = (double*) malloc( (n+1) * sizeof(double) );
    f = (double*) malloc( (n+1) * sizeof(double) );
    memset(u, 0, (n+1) * sizeof(double));
    for (i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Run the solver */
    get_time(&tstart);
    //jacobi(nsteps, n, u, f);

    threads = (pthread_t*) malloc(nthreads * sizeof(pthread_t));
    thread_data = (thread_data_t*) malloc(nthreads * sizeof(thread_data_t));
    
    pthread_barrier_t barrier;
    // Inicializar la barrera
    pthread_barrier_init(&barrier, NULL, nthreads);

    // Crear los hilos y asignarles su parte del vector a procesar
    for (int i = 0; i < nthreads; ++i) {
        thread_data[i].start = i*n/nthreads + 1;
        thread_data[i].end = (i+1)*n/nthreads;
        thread_data[i].n = n;
        thread_data[i].u = u;
        thread_data[i].f = f;
        thread_data[i].nsweeps = nsteps;
        thread_data[i].barrier = &barrier;
        pthread_create(&threads[i], NULL, jacobi_thread, &thread_data[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(thread_data);
    free(threads);

    get_time(&tend);

    /* Print info */    
    printf("n: %d\n"
           "nsteps: %d\n"
           "threads: %d\n"
           "Elapsed time: %g s\n", 
           n, nsteps, nthreads, timespec_diff(tstart, tend));

    /* Write the results */
    if (fname)
        write_solution(n, u, fname);

    free(f);
    free(u);
    return 0;
}
