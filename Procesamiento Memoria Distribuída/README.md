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

## Explicación del Código (main.c)

El código proporcionado resuelve la Ecuación de Calor en 2D utilizando el modelo de programación paralela MPI (Message Passing Interface). A continuación, se explica en lenguaje sencillo lo que hace el código paso a paso:

1. **Inicialización y Declaración de Variables**:
   - Se definen varias variables, como la constante de difusión `a`, dos campos `current` y `previous` que representan la temperatura actual y previa, y otras variables para controlar el tiempo y la paralelización.

2. **Inicialización de MPI**:
   - Se inicia MPI (Message Passing Interface) para habilitar la comunicación y coordinación entre múltiples procesos que trabajan en paralelo.

3. **Inicialización de Campos y Parámetros**:
   - Se inicializan los campos de temperatura `current` y `previous` y se establece el número de pasos de tiempo `nsteps` y otros parámetros relevantes. También se inicializa información sobre la paralelización.

4. **Salida Inicial**:
   - Se genera una salida que representa el campo de temperatura inicial y se escribe en un archivo.

5. **Cálculo del Paso de Tiempo**:
   - Se calcula el tamaño del paso de tiempo `dt` basado en las dimensiones de la malla y la constante de difusión `a`. Esto asegura que el proceso sea estable.

6. **Evolución Temporal**:
   - Se inicia un bucle que ejecuta la evolución de la temperatura a lo largo del tiempo.
   - Durante cada iteración, se realiza el siguiente proceso:
     - Se inician las operaciones de intercambio de información con los procesos vecinos para manejar las fronteras entre dominios.
     - Se evoluciona el interior del dominio en función de la ecuación de calor utilizando los campos `current` y `previous`, y se actualiza la temperatura.
     - Se finalizan las operaciones de intercambio para sincronizar la información entre procesos.
     - Se evoluciona el borde del dominio.
     - Se guarda el campo de temperatura actual en archivos de imagen a intervalos regulares.
     - Se guarda un "checkpoint" del estado actual para facilitar la reanudación de la simulación en el futuro.
     - Se intercambian los campos `current` y `previous` para que el campo actual sea el anterior en la siguiente iteración.

7. **Medición del Tiempo**:
   - Se mide el tiempo que ha tardado la iteración de la simulación.

8. **Salida de Resultados Finales**:
   - Se imprime el tiempo que tomó la simulación y se muestra un valor de referencia en la posición (5, 5) del campo de temperatura final.

9. **Finalización y Limpieza**:
   - Se escribe el campo de temperatura final en un archivo y se realiza una limpieza de recursos.
   - MPI se cierra con `MPI_Finalize()`.
