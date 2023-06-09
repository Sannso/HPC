#include <stdio.h>

#define N 10  // Longitud de la carretera
#define MAX_ITERATIONS 20  // Número máximo de iteraciones

void actualizarCarretera(int carretera[]) {
    int nuevoCarretera[N];
    
    for (int i = 0; i < N; i++) {
        int izquierda = carretera[(i-1+N) % N];
        int actual = carretera[i];
        int derecha = carretera[(i+1) % N];
        
        if (derecha == 0 && actual == 1) {
            nuevoCarretera[i] = 1;
        } else {
            nuevoCarretera[i] = 0;
        }
    }
    
    // Copiar los nuevos valores al arreglo original
    for (int i = 0; i < N; i++) {
        carretera[i] = nuevoCarretera[i];
    }
}

void imprimirCarretera(int carretera[]) {
    for (int i = 0; i < N; i++) {
        printf("%d ", carretera[i]);
    }
    printf("\n");
}

int main() {
    int carretera[N] = {1, 0, 0, 1, 0, 1, 0, 0, 0, 1};  // Ejemplo de configuración inicial de la carretera
    
    printf("Configuración inicial de la carretera: ");
    imprimirCarretera(carretera);
    
    for (int iteracion = 1; iteracion <= MAX_ITERATIONS; iteracion++) {
        actualizarCarretera(carretera);
        printf("Iteración %d: ", iteracion);
        imprimirCarretera(carretera);
    }
    
    return 0;
}