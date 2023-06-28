# wk04: Concurrency simple problem
## Instrucciones
Su programa deberá hacer lo siguiente:

- Leer N y M que son pasados como argumentos desde la línea de comandos
- Crear N Threads de Hidrógeno donde cada hilo que genera una molécula de H.
- Crear M Threads de Oxígeno donde cada hilo que genera una molécula de O.
- Crear 1 hilo de agua que espera hasta que se generen dos moléculas de hidrógeno y una de oxígeno y pueda crear una molécula de agua.

Considere que las moléculas son contadores en el programa, los hilos de Hidrógeno y Oxígeno son productores y el el hilo de agua es el consumidor.

El programa deberá mostrar las siguientes salidas de pantalla:

1. Cada vez que se genere una molécula de agua, hidrógeno u oxígeno.
2. Al final del programa un recuento del número de moleculas generadas y restantes.