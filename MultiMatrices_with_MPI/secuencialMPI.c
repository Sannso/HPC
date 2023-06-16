#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void calcular_area_circulo(int **result, int n);
void llenar_matrices(int **matrix1, int **matrix2, int n);
void memory_storage(int ***matrix, int n);
void release_memory(int **matrix, int n);

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv); // Inicializar el entorno MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtener el rango del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtener el tamaño del comunicador

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

    int local_n = n / size; // Tamaño local de las matrices
    int **local_matrix1 = NULL;
    int **local_matrix2 = NULL;
    int **local_result = NULL;

    memory_storage(&local_matrix1, local_n);
    memory_storage(&local_matrix2, local_n);
    memory_storage(&local_result, local_n);

    // Llenar matrices en cada proceso
    llenar_matrices(local_matrix1, local_matrix2, local_n);

    // Realizar la multiplicación local de las matrices
    for (i = 0; i < local_n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                local_result[i][j] += local_matrix1[i][k] * local_matrix2[k][j];
            }
        }
    }

    // Recopilar los resultados locales en el proceso raíz
    int **result = NULL;
    if (rank == 0) {
        result = (int **)malloc(n * sizeof(int *));
        memory_storage(&result, n);
    }
    MPI_Gather(local_result[0], local_n * n, MPI_INT, result[0], local_n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir la matriz resultado en el proceso raíz
    if (rank == 0 && impr > 0) {
        calcular_area_circulo(result, n);
    }

    // Liberar memoria en cada proceso
    release_memory(local_matrix1, local_n);
    release_memory(local_matrix2, local_n);
    release_memory(local_result, local_n);

    // Liberar memoria en el proceso raíz
    if (rank == 0) {
        release_memory(result, n);
    }

    fin = clock(); // Tomamos el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calculamos el tiempo transcurrido

    if (rank == 0) {
        printf("Tiempo transcurrido: %f segundos\n", tiempo);
    }

    MPI_Finalize(); // Finalizar el entorno MPI
    return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

// Imprimir la matriz resultado
void calcular_area_circulo(int **result, int n) {
    int i, j;

    printf("La matriz resultado es:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
}

void llenar_matrices(int **matrix1, int **matrix2, int n) {
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

void memory_storage(int ***matrix, int n) {
    *matrix = (int **)malloc(n * sizeof(int *));
    int i;
    for (i = 0; i < n; i++) {
        (*matrix)[i] = (int *)malloc(n * sizeof(int));
    }
}

void release_memory(int **matrix, int n) {
    int i;
    for (i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}