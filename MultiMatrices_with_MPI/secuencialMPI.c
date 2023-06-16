#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void llenar_matrices(int **matrix1, int **matrix2, int n);
void memory_storage(int **matrix1, int **matrix2, int **result, int n);
void release_memory(int **matrix1, int **matrix2, int **result, int n);

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t inicio, fin;
    double tiempo;

    inicio = clock(); // Tomamos el tiempo de inicio

    if (argc != 3) {
        if (rank == 0) {
            printf("Uso: %s n\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    int impr = atoi(argv[2]);
    int i, j, k;

    // Dividir el tamaño de la matriz entre los procesos
    int local_n = n / size;
    int extra = n % size;
    int start_row = rank * local_n;
    int end_row = start_row + local_n;

    if (rank == size - 1) {
        end_row += extra;
    }

    // Memory storage local
    int **local_matrix1 = (int **) malloc(local_n * sizeof(int *));
    int **local_matrix2 = (int **) malloc(n * sizeof(int *));
    int **local_result = (int **) malloc(local_n * sizeof(int *));
    memory_storage(local_matrix1, local_matrix2, local_result, local_n);

    // Llenar las matrices
    llenar_matrices(local_matrix1, local_matrix2, local_n);

    // Inicializar la matriz local del resultado con ceros
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < n; j++) {
            local_result[i][j] = 0;
        }
    }

    // Realizar la multiplicación paralela de las matrices
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                local_result[i][j] += local_matrix1[i][k] * local_matrix2[k][j];
            }
        }
    }

    // Recopilar todos los resultados locales en el proceso raíz
    int **result = NULL;
    if (rank == 0) {
        result = (int **) malloc(n * sizeof(int *));
        memory_storage(NULL, NULL, result, n);
    }
    MPI_Gather(local_result[0], local_n * n, MPI_INT, result[0], local_n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Liberar memoria local
    release_memory(local_matrix1, local_matrix2, local_result, local_n);

    // Liberar memoria en el proceso raíz
    if (rank == 0) {
        release_memory(NULL, NULL, result, n);
    }

    fin = clock(); // Tomamos el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calculamos el tiempo transcurrido

    // Imprimir el tiempo transcurrido en el proceso raíz
    if (rank == 0) {
        printf("Tiempo transcurrido: %f segundos\n", tiempo);
    }

    MPI_Finalize();
    return 0;
}



//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------



void llenar_matrices(int **matrix1, int **matrix2, int n){
    int i, j;
    
    // Llenar la matriz con valores aleatorios
    srand(time(NULL)); // Inicializar la semilla para generar valores aleatorios
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            matrix1[i][j] = rand() % 100;
            matrix2[i][j] = rand() % 100; 
        }
    }
}


void memory_storage(int **matrix1, int **matrix2, int **result,int n){
    int i;
    for (i = 0; i < n; i++) {
        matrix1[i] = (int *) malloc(n * sizeof(int));
        matrix2[i] = (int *) malloc(n * sizeof(int));
        result[i] = (int *) malloc(n * sizeof(int));
    }
}

void release_memory(int **matrix1, int **matrix2, int **result, int n){
    int i;
    for (i = 0; i < n; i++) {
        free(matrix1[i]);
    }
    free(matrix1);
    
    for (i = 0; i < n; i++) {
        free(matrix2[i]);
    }
    free(matrix2);
    
    for (i = 0; i < n; i++) {
        free(result[i]);
    }
    free(result);
}


