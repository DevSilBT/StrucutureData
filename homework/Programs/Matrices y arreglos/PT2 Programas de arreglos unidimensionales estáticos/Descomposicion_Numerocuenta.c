#include <stdio.h>
#define N 10


void llenarPares(int pares[]) {
    for (int i = 0; i < N; i++) {
        pares[i] = (i + 1) * 2;
    }
}


void imprimirInverso(const int arr[]) {
    printf("\n--- Arreglo en orden inverso ---\n");
    for (int i = N - 1; i >= 0; i--) {
        printf("|%d| ", arr[i]);
    }
    printf("\n");
}


void sustituirDigito(int arr[], int indice) {
    if (indice >= 0 && indice < N) {
        arr[indice] = -1;
    }
    printf("\n--- Arreglo tras sustituir el último dígito por -1 ---\n");
    for (int i = 0; i < N; i++) {
        printf("|%d| ", arr[i]);
    }
    printf("\n");
}

int main() {
    int pares[N];
    int cuenta, ultimo;

    llenarPares(pares);

    printf("Ingrese su número de cuenta: ");
    scanf("%d", &cuenta);

    ultimo = cuenta % 10;

    imprimirInverso(pares);
    sustituirDigito(pares, ultimo);

    return 0;
}

