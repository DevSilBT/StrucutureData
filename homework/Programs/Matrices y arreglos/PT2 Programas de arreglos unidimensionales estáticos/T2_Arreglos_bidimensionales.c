#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOMBRE 25
#define MAX_ALUMNOS 100

int main() {
    char alumnos[max_alumnos][max_nombre + 10]; 
    int n = 0;
    char respuesta;
    int suma_edad = 0, suma_calificacion = 0;
    
    do {
        printf("\nAlumno %d:\n", n + 1);
        
        // Capturar nombre
        printf("Nombre (max %d caracteres): ", MAX_NOMBRE);
        fgets(alumnos[n], MAX_NOMBRE + 1, stdin);
        alumnos[n][strcspn(alumnos[n], "\n")] = '\0'; // Eliminar salto de línea
        
        // Capturar edad
        printf("Edad: ");
        char edad_str[5];
        fgets(edad_str, sizeof(edad_str), stdin);
        int edad = atoi(edad_str);
        suma_edad += edad;
        
        
        printf("Calificacion: ");
        char calificacion_str[5];
        fgets(calificacion_str, sizeof(calificacion_str), stdin);
        int calificacion = atoi(calificacion_str);
        suma_calificacion += calificacion;
        
        // Almacenar edad y calificacion como caracteres en la misma fila
        alumnos[n][MAX_NOMBRE] = (char)edad;
        alumnos[n][MAX_NOMBRE + 1] = (char)calificacion;
        
        n++;
        
     
        printf("\n¿Desea agregar otro alumno? (s/n): ");
        respuesta = getchar();
        getchar(); // Limpiar el buffer
        
    } while ((respuesta == 's' || respuesta == 'S') && n < MAX_ALUMNOS);
    
    // Calcular promedios
    float promedio_edad = (float)suma_edad / n;
    float promedio_calificacion = (float)suma_calificacion / n;
    
  
    printf("\n\nRESULTADOS\n");
    
    // Imprimir alumnos en orden inverso
    printf("\nLista de alumnos en orden inverso:\n");
    printf("| %-25s | %-4s | %-11s |\n", "Nombre", "Edad", "Calificación");
    printf("|%-27s|%-6s|%-13s|\n", "---------------------------", "------", "-------------");
    
    for (int i = n - 1; i >= 0; i--) {
        printf("| %-25s | %4d | %11d |\n", 
               alumnos[i], 
               (int)alumnos[i][MAX_NOMBRE], 
               (int)alumnos[i][MAX_NOMBRE + 1]);
    }
    
    printf("\nPromedio de edad: %.2f", promedio_edad);
    printf("\nPromedio de calificación: %.2f\n", promedio_calificacion);
    
    return 0;
}
