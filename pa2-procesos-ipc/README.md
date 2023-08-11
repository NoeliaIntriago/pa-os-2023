# pa2: COmunicación entre Procesos IPC

## Objetivos
- Usar las funciones para creación y terminación de procesos de la libreria POSIX
- Sincronizar la ejecución de un proceso hijo desde el padre usando la syscall waitd
- Usar las syscall para gestionar comunicación entre procesos con los modelos de comunicación Shared Memory y Messages Queues

## Descripción
Escriba dos programas en C llamados timeshmm.cy timepipe.c que determinen el tiempo para ejecutar un comando desde la linea de comandos. Este programa se ejecutará como:
``` 
./time <comando>
``` 
y mostrará la cantidad de tiempo transcurrido a la hora de ejecutar el comando especificado. Esto implicará usar `fork()` y funciones `exec()`, así como la función `gettimeofday ()` para determinar el tiempo transcurrido.
El programa debe permitir ejecutar comandos de unix/linux con o sin parámetros. Algunos ejemplos, separados por coma, a continuación: `ls -ax, ps -ax, grep -in Maria names.data` 

## Estrategia
La estrategia general es crear un proceso hijo que ejecutará el comando especificado. Antes de que el hijo ejecute el comando, deberá obtener una marca de tiempo de la hora actual (principio). El proceso padre esperará a que finalice el proceso hijo. Una vez que el hijo termine, el padre registrará la marca de tiempo actual (final). Luego, debe obtener la diferencia entre el principio y el final que representa el tiempo transcurrido para ejecutar el comando.

La salida a continuación muestra la cantidad de tiempo para ejecutar el comando ls: 
```
./timexxxx ls
time.c
time
Elapsed time: 0.25422
```
Como el padre y el hijo son procesos separados, necesitarán compartir el tiempo de inicio entre ellos por lo que deberá implementar dos versiones de IPC.
1. En el programa timeshmm.c, el proceso hijo escribirá el tiempo de inicio en una región de memoria compartida antes de que llame a `exec()`. Una vez que finalice el proceso hijo, el padreleerála hora de inicio de la memoria compartida. Consulte el código del repositorio sobre share memory para obtener detalles sobre el uso. La ubicación del ejemplo está en `repo/unit2/share_memory`.
2. En el programa timepipe, se usará la syscall `pipe()`. El hijo escribirá el comienzo tiempo en la cola del pipe, y el padre leerá después de la terminación del proceso hijo. Consulte el código del repositorio sobre use pipe para obtener detalles sobre el uso. La ubicación del ejemplo está en `repo/unit2/use_pipe`.

Utilizará la función `gettimeofday()` para registrar la marca de tiempo actual. A esta función se le pasa un puntero a un objeto `struct timeval`, que contiene dos miembros: `tv_sec` y `t_usec`.
El siguiente ejemplo ilustra cómo se puede usar esta función: 
```
struct timeval current; gettimeofday(¤t, NULL);
// current.tv sec represents seconds
// current.tv usec represents microseconds
```

## Recomendaciones
Para IPC entre los procesos hijo y padre, el contenido del puntero de memoria compartida puede asignarse la `struct timeval`. Cuando se utilice pipes se puede escribir y leer desde el pipe un puntero a la `struct timeval`. 