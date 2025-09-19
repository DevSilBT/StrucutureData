#include <stdio.h>

#define MAX_FILAS 10
#define MAX_COLUMNAS 10

void llenarMatriz(int matriz[MAX_FILAS][MAX_COLUMNAS], int n, int m) {
    printf("Ingrese los elementos de la matriz %dx%d:\n", n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("Elemento [%d][%d]: ", i, j);
            scanf("%d", &matriz[i][j]);
        }
    }
}

void mostrarMatriz(int matriz[MAX_FILAS][MAX_COLUMNAS], int n, int m) {
    printf("\nMatriz ingresada:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }
}

void recorridoPorFilas(int matriz[MAX_FILAS][MAX_COLUMNAS], int n, int m) {
    printf("\n1. Recorrido por filas (00, 01, 02..., 10, 11..., nm):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", matriz[i][j]);
        }
    }
    printf("\n");
}

void recorridoPorColumnas(int matriz[MAX_FILAS][MAX_COLUMNAS], int n, int m) {
    printf("\n2. Recorrido por columnas (nm, n-1m, n-2m...0m, nm-1, n-1m-1...00):\n");
    for (int j = m - 1; j >= 0; j--) {
        for (int i = n - 1; i >= 0; i--) {
            printf("%d ", matriz[i][j]);
        }
    }
    printf("\n");
}

void recorridoDiagonal(int matriz[MAX_FILAS][MAX_COLUMNAS], int n, int m) {
    printf("\n3. Recorrido diagonal (00, 11, 22, 33...):\n");
    
    int min_dim = (n < m) ? n : m;
    for (int i = 0; i < min_dim; i++) {
        printf("%d ", matriz[i][i]);
    }
    
    if (n != m) {
        printf("\nNota: La matriz no es cuadrada (%dx%d). ", n, m);
        printf("Solo se muestran las primeras %d diagonales.\n", min_dim);
    }
    printf("\n");
}

int main() {
    int n, m;
    int matriz[MAX_FILAS][MAX_COLUMNAS];
    int opcion;

    // Solicitar dimensiones
    printf("Ingrese el numero de filas n (max %d): ", MAX_FILAS);
    scanf("%d", &n);
    printf("Ingrese el numero de columnas m (max %d): ", MAX_COLUMNAS);
    scanf("%d", &m);

    // Validar dimensiones
    if (n <= 0 || n > MAX_FILAS || m <= 0 || m > MAX_COLUMNAS) {
        printf("Dimensiones invalidas. Deben ser entre 1 y %d.\n", MAX_FILAS);
        return 1;
    }

    llenarMatriz(matriz, n, m);
    mostrarMatriz(matriz, n, m);

    // Menú de opciones (mejora del programa 3)
    do {
        printf("\n--- MENU DE RECORRIDOS ---\n");
        printf("1. Recorrido por filas\n");
        printf("2. Recorrido por columnas\n");
        printf("3. Recorrido diagonal\n");
        printf("4. Mostrar todos los recorridos\n");
        printf("5. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: 
                recorridoPorFilas(matriz, n, m);
                break;
            case 2: 
                recorridoPorColumnas(matriz, n, m);
                break;
            case 3: 
                recorridoDiagonal(matriz, n, m);
                break;
            case 4:
                recorridoPorFilas(matriz, n, m);
                recorridoPorColumnas(matriz, n, m);
                recorridoDiagonal(matriz, n, m);
                break;
            case 5: 
                printf("Fin del programa.\n");
                break;
            default: 
                printf("Opcion invalida.\n");
        }
    } while (opcion != 5);

    return 0;
}
