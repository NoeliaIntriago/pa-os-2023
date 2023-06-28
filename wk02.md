# wk02: Multi-Thread programming
## Objetivos
- Usar las funciones para creación y terminación de hilos de la librería `POSIX`
- Sincronizar la finalización de hilos con el uso de la función `pthread_join()`
- Trabajar sobre una región compartida entre los hilos sin problemas de competencia de carrera
## Descripción
Escriba un programa en `C` para validar un tablero de sudoku de 9x9, de manera que, cada columna, cada fila, así como cada sub-matriz de 3x3 debe contener los dígitos del 1 al 9, 
ver la Tabla 1 para referencia. Usted deberá diseñar e implementar una aplicación multihilos que determine si la solución es válida.

```
6	2	4	5	3	9	1	8	7
5	1	9	7	2	8	6	3	4
8	3	7	6	1	4	2	9	5
1	4	3	8	6	5	7	2	9
9	5	8	2	4	7	3	6	1
7	6	2	3	9	1	4	5	8
3	7	1	9	5	6	8	4	2
4	9	6	1	8	2	5	7	3
2	8	5	4	7	3	9	1	6
```
Table: Tabla 1

Hay varias formas diseñar esta aplicación multihilos. Una estrategia sugerida es crear hilos que verifiquen los siguientes criterios:

- Un hilo para verificar que cada columna contenga los dígitos del 1 al 9
- Un hilo para verificar que cada fila contenga los dígitos del 1 al 9
- Nueve hilos para verificar que cada una de las sub-matrices de 3 × 3 contengan los dígitos del 1 al 9
Esto daría como resultado un total de 11 hilos separados para validar un tablero de Sudoku. Sin embargo, usted puede crear aún más hilos. Por ejemplo, en lugar de crear un hilo que verifique las nueve columnas, puede crear nueve hilos separados y hacer que cada uno de ellos verifique una columna.

### Pasando parámetros a cada hilo
El hilo principal (`main`) creará los múltiples hilos de trabajo (workers), pasando a cada worker la ubicación que debe verificar en la matriz. 
Este paso requerirá pasar varios parámetros a cada hilo. El enfoque más fácil es crear una estructura de datos utilizando un struct. 
Por ejemplo, una estructura para pasar la fila y la columna donde un subproceso debe comenzar a validarse aparecería de la siguiente manera:

```
/* structure for passing data to threads */ 
typedef struct 
{ 
    int row; 
    int column; 
} parameters; 
/* Now create the thread passing it data as a parameter */ 
parameters *data = (parameters *) malloc(sizeof(parameters)); 
data->row = 1; 
data->column = 1;
```

El puntero a `parameters` se pasará a la función `pthread_create()`, que a su vez lo pasará como un parámetro a la función que se ejecutará como un hilo separado.

### Retorno de valores al hilo principal
A cada hilo de trabajo se le asigna la tarea de determinar la validez de una región particular del tablero de Sudoku. 
Una vez que un worker ha realizado esta verificación, debe pasar sus resultados al padre.

Una buena manera de manejar esto, es crear un arreglo de valores enteros que sea compartido para cada worker. 
Se sugiere utilizar el arreglo `region_isvalid`. El i-ésimo índice de este arreglo corresponde al i-ésimo worker de trabajo. 
Si un worker establece su valor correspondiente en 1, está indicando que su región del rompecabezas de Sudoku es válida. Un valor de 0 indica lo contrario. 
Cuando se han completado todos los workers, el hilo principal comprueba cada entrada en la matriz de resultados para determinar si el tablero de Sudoku es válido.

Puede revisar el código en el repositorio `unit2/use_thread/multi_matrix` para tener una referencia para compartir matrices entre varios hilos, además de enviar y recibir parámetros y valores de retorno de los sub-hilos.

### Programa principal
Utilice únicamente el archivo `main.c` para escribir su programa. 
El programa recibe un argumento de línea de comando `el nombre de un archivo de texto`, el cual contiene la matrriz de 9x9 que será validada. 
No escriba una función para leer el archivo de texto. Use la función `loadBoard(namefile, board)` para cargar el archivo de texto en la matriz del tablero de sudoku `board`.

### Realizar lo siguiente:

1. Escriba cada una de las funciones `worker` (o runner) que seran usadas para validar las regiones de la matriz.
Estas funciones, se deben invocar usando como argumentos la ubicación a validar, como se indica arriba en pasando parametros a cada hilo.
Luego de verificar la region a validar, debera establecer si el hilo ha pasado la verificacion en la entrada que le corresponde del arreglo de verificacion `region_isvalid`.

2. Use dos arreglos paralelos uno de hilos y otro de los atributos de ejecucion de estos.

3. Use la funcion `pthread_attr_init()` para iniciar los parametros de ejecucion de cada hilo

4. Utilice `pthread_create()` para lanzar cada hilo y asociarlo con su correspondiente funcion worker

5. Use `pthread_join()` para esperar a que finalicen todos los hilos lanzados

Verifique cada entrada del arreglo `region_isvalid` y determine si el tablero es valido o no.

**Nota:** Recuerde verificar que las funciones usadas incluyan los archivos de cabecera correctos usando el comando `man` o la ayuda en linea.
