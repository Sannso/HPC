#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "timing.h"

/* --
 * Do nsweeps sweeps of Jacobi iteration on a 1D Poisson problem
 * 
 *    -u'' = f
 *
 * discretized by n+1 equally spaced mesh points on [0,1].
 * u is subject to Dirichlet boundary conditions specified in
 * the u[0] and u[n] entries of the initial vector.
 */
void jacobi(int nsweeps, int n, double* u, double* f)
{
    int i, sweep;
    double h  = 1.0 / n;
    double h2 = h*h;
    double* utmp = (double*) malloc( (n+1) * sizeof(double) );

    /* Fill boundary conditions into utmp */
    utmp[0] = u[0];
    utmp[n] = u[n];

    for (sweep = 0; sweep < nsweeps; sweep += 2) {
        
        /* Old data in u; new data in utmp */
        for (i = 1; i < n; ++i)
            utmp[i] = (u[i-1] + u[i+1] + h2*f[i])/2;
        
        /* Old data in utmp; new data in u */
        for (i = 1; i < n; ++i)
            u[i] = (utmp[i-1] + utmp[i+1] + h2*f[i])/2;
    }

    free(utmp);
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
    int i, j;
    int n, nsteps, num_procs;
    double* u;
    double* f;
    double h;
    timing_t tstart, tend;
    char* fname;

    /* Process arguments */
    n      = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    fname  = (argc > 3) ? argv[3] : NULL;
    num_procs = (argc > 4) ? atoi(argv[4]) : 8; // 8 por defecto
    h      = 1.0/n;

    /* Allocate and initialize arrays */
    u = (double*) malloc( (n+1) * sizeof(double) );
    f = (double*) malloc( (n+1) * sizeof(double) );
    memset(u, 0, (n+1) * sizeof(double)); // Initialize u with zeros
    memset(f, 0, (n+1) * sizeof(double)); // Initialize f with zeros
    for (i = 0; i <= n; ++i) {
        double x = i*h;
        f[i] = x * (1 - x); // Initialize f
        u[i] = 0; // Initialize u to 0
    }
    u[0] = u[n] = 0; // Set boundary conditions



    /* Run the solver */
    get_time(&tstart);
    
    int pid, pid_status;
    int partition_size = n / num_procs; // Divide the domain equally among the processes

    for (i = 0; i < num_procs; ++i) {
        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error: Could not fork process %d\n", i);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) { // Child process
            int start = i * partition_size;
            int end = start + partition_size - 1;

            if (i == 0) { // Leftmost process
                start += 1; // Skip u[0] to avoid boundary condition
            }
            else if (i == num_procs - 1) { // Rightmost process
                end -= 1; // Skip u[n] to avoid boundary condition
            }

            double* f_partition = (double*) malloc( (partition_size+1) * sizeof(double) );
            double* u_partition = (double*) malloc( (partition_size+1) * sizeof(double) );
            memcpy(f_partition, f + start, (partition_size+1) * sizeof(double));
            memcpy(u_partition, u + start, (partition_size+1) * sizeof(double));

            jacobi(nsteps, partition_size, u_partition, f_partition);

            memcpy(u + start, u_partition, (partition_size+1) * sizeof(double));

            free(f_partition);
            free(u_partition);

            exit(EXIT_SUCCESS);
        }
    }

    for (i = 0; i < num_procs; ++i) { // Wait for child processes to finish
        pid = wait(&pid_status);
        if (!WIFEXITED(pid_status)) {
            printf("Process %d exited abnormally\n", pid);
        }
    }


    get_time(&tend);

    /* Print time */    
    printf("n: %d\n"
           "nsteps: %d\n"
           "processes: %d\n"
           "Elapsed time: %g s\n", 
           n, nsteps, num_procs, timespec_diff(tstart, tend));

    /* Write the results */
    if (fname)
        write_solution(n, u, fname);

    free(f);
    free(u);
    return 0;
}
