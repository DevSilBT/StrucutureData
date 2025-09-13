#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función recursiva para calcular factorial
int factRecursion(int n) {
    if (n < 0) {
        return 0; // Factorial indefinido para números negativos
    } else if (n == 0 || n == 1) {
        return 1;
    } else {
        return n * factRecursion(n - 1);
    }
}

// Función envoltorio para mayor legibilidad
int factorial(int n) {
    return factRecursion(n);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <numero_entero>\n", argv[0]);
        return 1;
    }

    char *num = argv[1];
    int n = atoi(num);

    fprintf(stdout, "Argumento recibido: %s\n", num);

    // Mostrar cada dígito del argumento (característica del primer código)
    for (int i = 0; i < (int)strlen(num); i++) {
        fprintf(stdout, "num[%d] = %c\n", i, num[i]);
    }

    if (n < 0) {
        fprintf(stderr, "Error: El factorial no está definido para números negativos.\n");
        return 1;
    }

    int resultado = factorial(n);
    fprintf(stdout, "Factorial de %d = %d\n", n, resultado);

    return 0;
}

