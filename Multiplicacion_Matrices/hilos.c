#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


typedef struct {
    int thread_id;
    int num_threads;
    int **matrix1;
    int **matrix2;
    int **result;
    int N;
} thread_data;

void *matrix_multiply(void *arg);
void llenar_matrices(int **matrix1, int **matrix2, int n);
void memory_storage(int **matrix1, int **matrix2, int **result, int n);
void release_memory(int **matrix1, int **matrix2, int **result, int n);

int main(int argc, char *argv[]) {
    clock_t inicio, fin;
    double tiempo;

    inicio = clock(); // Tomamos el tiempo de inicio
    
    if (argc != 3) {
        printf("Uso: %s n\n", argv[0]);
        return 1;
    }
    
    
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int i, j;
    pthread_t threads[num_threads];
    thread_data thread_args[num_threads];

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

    // Crear los hilos
    for (i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].num_threads = num_threads;
        thread_args[i].matrix1 = matrix1;
        thread_args[i].matrix2 = matrix2;
        thread_args[i].result = result;
        thread_args[i].N = n;
        int rc = pthread_create(&threads[i], NULL, matrix_multiply, (void *)&thread_args[i]);
        if (rc) {
            printf("Error: info enviada de pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Esperar a que los hilos terminen
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberar memoria
    release_memory(matrix1, matrix2, result, n);
    
    fin = clock(); // Tomamos el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calculamos el tiempo transcurrido
    printf("Tiempo transcurrido: %f segundos\n", tiempo);
    pthread_exit(NULL);
    
}


void *matrix_multiply(void *arg) {
    thread_data *data = (thread_data *)arg;
    int i, j, k;
    int chunk_size = data->N / data->num_threads;
    int start_row = data->thread_id * chunk_size;
    int end_row = start_row + chunk_size;

    for (i = start_row; i < end_row; i++) {
        for (j = 0; j < data->N; j++) {
            data->result[i][j] = 0;
            for (k = 0; k < data->N; k++) {
                data->result[i][j] += data->matrix1[i][k] * data->matrix2[k][j];
            }
        }
    }

    pthread_exit(NULL);
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




