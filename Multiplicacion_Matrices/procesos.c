#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


void matrix_multiply(int start, int end, int size, int **matrix1, int **matrix2, int **result);
void llenar_matrices(int **matrix1, int **matrix2, int n);
void memory_storage(int **matrix1, int **matrix2, int **result, int n);
void release_memory(int **matrix1, int **matrix2, int **result, int n);


// Recibe 2 argumentos: 1- tamaño de la matriz 2- numero de procesos
int main(int argc, char *argv[]) {
    clock_t inicio, fin;
    double tiempo;

    inicio = clock(); // Tomamos el tiempo de inicio
    
    if (argc != 3) {
        printf("Uso: %s n\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    int num_processes = atoi(argv[2]);
    int i, j, k, size, start, end;


     //memory storage
    int **matrix1 = (int **) malloc(n * sizeof(int *));
    int **matrix2 = (int **) malloc(n * sizeof(int *));
    int **result = (int **) malloc(n * sizeof(int *));
    memory_storage(matrix1, matrix2, result, n);

    // Llenar matrices
    llenar_matrices(matrix1, matrix2, n);
    
    // Inicializar la matriz resultado con ceros
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            result[i][j] = 0;
        }
    }
    
    // Inicializan los args que necesita cada proceso y crearlo
    int chunk_size = n / num_processes;
    for (i = 0; i < num_processes; i++) {
        start = i * chunk_size;
        end = (i == num_processes - 1) ? n : (i + 1) * chunk_size;
        pid_t pid = fork();
        if (pid == 0) {
            matrix_multiply(start, end, n, matrix1, matrix2, result);
            exit(0);
        }
    }

    for (i = 0; i < num_processes; i++) {
        wait(NULL);
    }


    // Liberar memoria
    release_memory(matrix1, matrix2, result, n);
    
    fin = clock(); // Tomamos el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calculamos el tiempo transcurrido
    printf("Tiempo transcurrido: %f segundos\n", tiempo);

    return 0;
}


void matrix_multiply(int start, int end, int size, int **matrix1, int **matrix2, int **result) {
    int i, j, k;
    for (i = start; i < end; i++) {
        for (j = 0; j < size; j++) {
            result[i][j] = 0;
            for (k = 0; k < size; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}



void memory_storage(int **matrix1, int **matrix2, int **result,int n){
    int i, j, k;
    for (i = 0; i < n; i++) {
        matrix1[i] = (int *) malloc(n * sizeof(int));
        matrix2[i] = (int *) malloc(n * sizeof(int));
        result[i] = (int *) malloc(n * sizeof(int));
    }
}

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

