#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void llenar_matrices(int *matrix1, int *matrix2, int n);
void memory_storage(int **matrix, int n);
void release_memory(int *matrix);

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv); // Inicializar el entorno MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtener el rango del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtener el tamaño del comunicador

    clock_t inicio, fin;
    double tiempo;

    inicio = clock(); // Tomar el tiempo de inicio

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

    int local_n = n / size; // Tamaño local de las matrices
    int *local_matrix1 = NULL;
    int *local_matrix2 = NULL;
    int *local_result = NULL;

    memory_storage(&local_matrix1, local_n * n);
    memory_storage(&local_matrix2, local_n * n);
    memory_storage(&local_result, local_n * n);

    // Llenar matrices en cada proceso
    llenar_matrices(local_matrix1, local_matrix2, local_n * n);

    // Realizar la multiplicación local de las matrices
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                local_result[i * n + j] += local_matrix1[i * n + k] * local_matrix2[k * n + j];
            }
        }
    }

    // Recopilar los resultados locales en el proceso raíz
    int *result = NULL;
    if (rank == 0) {
        result = (int *)malloc(n * n * sizeof(int));
    }
    MPI_Gather(local_result, local_n * n, MPI_INT, result, local_n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Liberar memoria en cada proceso
    release_memory(local_matrix1);
    release_memory(local_matrix2);
    release_memory(local_result);

    // Liberar memoria en el proceso raíz
    if (rank == 0) {
        release_memory(result);
    }

    fin = clock(); // Tomar el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calcular el tiempo transcurrido

    if (rank == 0) {
        printf("Tiempo transcurrido: %f segundos\n", tiempo);
    }

    MPI_Finalize(); // Finalizar el entorno MPI
    return 0;
}

void llenar_matrices(int *matrix1, int *matrix2, int n) {
    int i;

    // Llenar las matrices con valores aleatorios
    srand(time(NULL)); // Inicializar la semilla para generar valores aleatorios
    for (i = 0; i < n; i++) {
        matrix1[i] = rand() % 100;
        matrix2[i] = rand() % 100;
    }
}

void memory_storage(int **matrix, int n) {
    *matrix = (int *)malloc(n * sizeof(int));
}

void release_memory(int *matrix) {
    free(matrix);
}
