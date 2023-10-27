# Heat Equation Solver

Este proyecto consiste en un programa que resuelve la ecuación del calor en un dominio bidimensional y paraleliza el cálculo utilizando MPI (Message Passing Interface). El programa incluye un código principal y varios archivos de soporte.

## Estructura de los Códigos

El proyecto consta de varios archivos fuente y encabezado:

- `heat.c` y `heat.h`: Implementan las funciones y estructuras para resolver la ecuación del calor. El archivo `heat.c` contiene el código principal del solucionador.

- `pngwriter.c` y `pngwriter.h`: Contienen funciones para guardar los resultados en archivos PNG y para generar una representación visual de la solución.

- `setup.c`: Define las funciones para inicializar el programa, incluyendo la generación del campo de temperatura inicial.

- `utilities.c`: Contiene utilidades para alojar y gestionar la memoria de los campos de temperatura y copiar datos entre ellos.

- `Makefile`: Archivo para compilar el proyecto.

## Código Principal

El código principal se encuentra en `heat.c`. Es responsable de iniciar el solucionador de la ecuación del calor y coordinar la ejecución paralela utilizando MPI. El archivo contiene la función `main` que controla la simulación.

## Instrucciones de Compilación

Para compilar el proyecto, se proporciona un archivo `Makefile`. Simplemente ejecuta el siguiente comando en tu terminal:

```shell
make
