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

## Instrucciones de Compilación

A continuación se describen los pasos necesarios para compilar el código proporcionado en el archivo `main.c`. 


### 1. Acceder al Entorno de MPI

Primero, debemos acceder a un entorno que admita MPI. Esto generalmente implica utilizar comandos como `srun` para iniciar procesos MPI. En el ejemplo siguiente, se inicia un entorno de 24 procesos:

```bash
srun -n 24 --pty /bin/bash
```	

### 2. Cargar los Módulos Necesarios

A continuación, se deben cargar los módulos necesarios. En el siguiente ejemplo, se carga el módulo OpenMPI:

```bash
module load devtools/mpi/openmpi/3.1.4
```

### 3. Limpieza y Compilación

Asegúrate de que estás en el directorio donde se encuentra el código fuente. Utiliza el archivo Makefile proporcionado para compilar el programa. Para limpiar los archivos temporales previamente generados, ejecuta:

```bash
make clean
```

Después de limpiar, compila el código utilizando el siguiente comando:

```bash
make
```

O también, si queremos compilar el programa sin utilizar el archivo Makefile, podemos hacerlo directamente utilizando el comando mpicc:

```bash
mpicc -O3 -Wall -o heat_mpi main.c core.c setup.c utilities.c io.c pngwriter.c -lpng -lm
```

Este comando compilará todos los archivos fuente y generará un ejecutable llamado ``` heat_mpi. ``` Los argumentos ``` -O3 ``` y ``` -Wall ``` habilitan las optimizaciones y muestran advertencias, respectivamente. Las opciones ``` -lpng ``` y ``` -lm ``` se utilizan para vincular las bibliotecas necesarias.

## Ejecución Interactiva

Podemos ejecutar el programa interactivamente utilizando el comando `mpirun`. A continuación se describen varias opciones para ejecutar el programa con diferentes condiciones iniciales y parámetros de tiempo. Asegúrate de haber compilado el programa siguiendo las instrucciones previamente proporcionadas.

### 1. Valores Predeterminados

Para ejecutar el programa con los valores predeterminados, utiliza el siguiente comando:

```bash
mpirun -np 8 ./heat_mpi
```

Esto ejecutará el programa con valores predeterminados para el campo inicial y los parámetros de tiempo.

### 2. Campo Inicial desde un Archivo

Puedes utilizar un archivo de entrada para definir el campo inicial de temperatura. Por ejemplo, si tienes un archivo llamado botella.dat, ejecuta el programa de la siguiente manera:

```bash
mpirun -np 8 ./heat_mpi botella.dat
```

Esto tomará el campo inicial del archivo botella.dat como punto de partida.

### 3. Campo Inicial desde un Archivo y Pasos de Tiempo

Para especificar tanto el campo inicial desde un archivo como el número de pasos de tiempo, utilizamos el siguiente formato:

```bash
mpirun -np 8 ./heat_mpi botella.dat 1000
```

Esto cargará el campo inicial desde botella.dat y realizará 1000 pasos de tiempo.

### 4. Dimensiones y Pasos de Tiempo Personalizados

Para definir dimensiones personalizadas (ancho y alto) y el número de pasos de tiempo, podemos hacerlo de la siguiente manera:

```bash
mpirun -np 8 ./heat_mpi [ANCHO] [ALTO] [PASOS]
```

Por ejemplo:

```bash
mpirun -np 8 ./heat_mpi 800 800 1000
```

Todos estos comandos, generará una serie de archivos heat_NUM_figura.png que representan el desarrollo temporal del campo de temperatura. Podemos utilizar cualquier visor de gráficos para visualizar estos resultados.

## Ejecución Pasiva

Para ejecutar el programa en modo pasivo utilizando sbatch y garantizar que se cargue el módulo MPI recomendado antes de la ejecución, debemos seguir estos pasos:

### 1. Crear un archivo de script de trabajo

Crear un archivo de script de trabajo, por ejemplo, run_heat_mpi.sh, utilizando un editor de texto. Similar al siguiente contenido en el archivo:
```bash
#!/bin/bash
#SBATCH --job-name=heat_mpi_job   # Nombre del trabajo
#SBATCH --ntasks=8               # Número total de tareas MPI
#SBATCH --nodes=2                # Número de nodos
#SBATCH --cpus-per-task=1        # Número de hilos de CPU por tarea
#SBATCH --output=heat_mpi_output.txt

# Carga del módulo MPI recomendado
module load devtools/mpi/openmpi/3.1.4

# Ruta al ejecutable y argumentos
EXECUTABLE=./heat_mpi
INPUT_FILE=botella.dat
NUM_STEPS=1000

# Ejecución del programa con MPI
mpirun -np ./heat_mpi
```

### 2. Enviar el trabajo a Slurm

Utilizando el comando sbatch para enviar el trabajo a Slurm. El script se someterá y ejecutará según las opciones especificadas en el script de trabajo. Asegúrate de estar en el directorio donde se encuentra el script:

```bash 
sbatch run_heat_mpi.sh
```

# Ejecución de pruebas Simulación de la Ecuación de Calor 2D

Los resultados de la simulación de la ecuación de calor 2D utilizando el programa `heat_mpi` compilado con las banderas por defecto `-O3 -Wall` pero con diferentes configuraciones. A continuación, se presentan los resultados, las configuraciones y las conclusiones de las pruebas.

### Prueba 1: Ejecución con archivo 'bottle.dat'

Ejecución:
```bash
mpirun -np 8 ./heat_mpi bottle.dat
```

Resultados:

![Texto alternativo](https://github.com/SC3UIS/IntroPP2191927/blob/main/Procesamiento%20Memoria%20Distribu%C3%ADda/imgs/1280%201280.png)

- Uso de descomposición de dominio 4x2
- Tamaño local del dominio: 500x1000
- Tiempo de iteración: 63.527 segundos

### Prueba 2: Ejecución con archivo 'bottle.dat' y 1000 pasos

Ejecución:
```bash
mpirun -np 8 ./heat_mpi bottle.dat 1000
```

Resultados:

![Texto alternativo](https://github.com/SC3UIS/IntroPP2191927/blob/main/Procesamiento%20Memoria%20Distribu%C3%ADda/imgs/mpirun%20-np%208%201000.png)

- Uso de descomposición de dominio 4x2
- Tamaño local del dominio: 500x1000
- Reinicio desde un punto de control guardado en la iteración 400
- Tiempo de iteración: 157.694 segundos
- Valor de referencia en (5,5): 52.780686

### Prueba 3: Ejecución con dimensiones 1280x1280 y 1000 pasos

Ejecución:
```bash 
mpirun -np 8 ./heat_mpi 1280 1280 1000
```

Resultados:

![Texto alternativo](https://github.com/SC3UIS/IntroPP2191927/blob/main/Procesamiento%20Memoria%20Distribu%C3%ADda/imgs/1280%201280.png)

- Uso de descomposición de dominio 4x2
- Tamaño local del dominio: 500x1000
- Reinicio desde un punto de control guardado en la iteración 1400
- Tiempo de iteración: 157.315 segundos
- Valor de referencia en (5,5): 52.664584

## Conclusiones
**La ejecución con el archivo 'bottle.dat' tomó menos tiempo que la ejecución con dimensiones 1280x1280 y 1000 pasos, lo que sugiere que el tamaño del dominio y la cantidad de pasos afectan significativamente el tiempo de ejecución.**

**En la Prueba 2, se reinició desde un punto de control guardado, lo que permitió continuar desde una iteración anterior. Esto puede ser útil para reducir el tiempo de ejecución cuando se requieren ejecuciones largas.**

**Los valores de referencia en la Prueba 2 y Prueba 3 muestran la temperatura en el punto (5,5), lo que puede ser importante para evaluar la precisión del resultado.**
 