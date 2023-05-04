#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void calcular_area_circulo(int **result, int n);
void llenar_matrices(int **matrix1, int **matrix2, int n);
void memory_storage(int **matrix1, int **matrix2, int **result,int n);
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
    int impr = atoi(argv[2]);
    int i, j, k;
    
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
    
    // Realizar la multiplicación de las matrices de forma paralela
    #pragma omp parallel shared(matrix1, matrix2, result, n) private(i, j, k)
    {
        #pragma omp for schedule(dynamic)
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (k = 0; k < n; k++) {
                    result[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }
    }
    
    // Imprimir la matriz resultado
    if(impr > 0){
        calcular_area_circulo(result, n);
    }
    
    // Liberar memoria
    release_memory(matrix1, matrix2, result, n);
    
    fin = clock(); // Tomamos el tiempo de finalización
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC; // Calculamos el tiempo transcurrido
    printf("Tiempo transcurrido: %f segundos\n", tiempo);
    return 0;
}


//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

// Imprimir la matriz resultado
void calcular_area_circulo(int **result, int n){
    int i, j;
    
    printf("La matriz resultado es:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
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




