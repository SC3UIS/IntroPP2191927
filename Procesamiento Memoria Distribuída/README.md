# Solución de la Ecuación del Calor con MPI

Esta solución es un programa en C que resuelve la Ecuación del Calor en un dominio 2D utilizando el modelo de programación paralela MPI (Message Passing Interface). La Ecuación del Calor se utiliza para simular la distribución de temperatura en una placa 2D a lo largo del tiempo.

## Estructura de los Códigos

La solución consta de varios archivos fuente, cada uno con una función específica:

- `core.c`: Implementa la lógica principal para resolver la Ecuación del Calor.
- `heat.h`: Archivo de cabecera que define estructuras y prototipos de funciones.
- `io.c`: Contiene funciones para leer y escribir datos.
- `main.c`: El programa principal que configura y controla la ejecución.
- `pngwriter.c` y `pngwriter.h`: Proporcionan funciones para escribir resultados en archivos PNG.
- `setup.c`: Contiene funciones para la inicialización del programa.
- `utilities.c`: Funciones utilitarias para la gestión de campos de temperatura y matrices.

## Código Principal

El código principal de esta solución se encuentra en `main.c`. Este archivo configura el entorno MPI, inicializa los campos de temperatura, realiza las iteraciones de la simulación y gestiona la comunicación entre procesadores.

## Instrucciones de Compilación

A continuación, se proporciona un ejemplo de cómo compilar la solución en un entorno MPI:

```bash
mpicc -o heat_solver core.c main.c io.c setup.c utilities.c pngwriter.c -lm -lpng
