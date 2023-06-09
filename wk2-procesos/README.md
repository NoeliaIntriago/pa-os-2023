# wk01: Comprobando el estado de terminación de los procesos
## Objetivos
- Usar las funciones `wait()` de sincronización básica para esperar a un proceso
- Usar las funciones `exec()` para cargar programas externos y reemplazar el espacio de direcciones del proceso hijo
- Verificar el estado de retorno del proceso hijo
## Descripción
En el ejemplo `unit2/use_fork/waitpid_status.c` del repositorio del curso, se verifica el estado de terminación del proceso hijo (linea 26). 
Por convención, cuando un proceso Unix termina sin errores, regresa o sale con el estado 0. 
Cuando el estado de terminación es diferente de 0, esta convención indica que surgió algún tipo de condición de error. 
Se usa la llamada a `exit(value)` para indicar en value el estado de terminación.

Se practicará exactamente eso en este taller y también se aprenderá a crear procesos que no sean clones idénticos de su padre con el uso de la llamada a `exec`.

## Programa Principal
El programa principal recibirá un argumento de la linea de comando, la ubicación de un archivo de texto. 
El programa ejecutará los comandos de Unix `cat` y `wc` enviando como argumento la ubicación que se recibe. 
Utilice solo el archivo `main.c` para escribir su programa. Además, se provee de un archivo de texto de nombre `names.dat`.

Para ejecutar el programa, luego de presionar el boton Run, desde la tab `Shell` ingrese lo siguiente:

```
./main names.dat
```

## Instrucciones
1. Desde el programa principal cree un proceso hijo.
2. En el codigo del hijo hace la llamada al sistema `execvp` para ejecutar `/bin/cat` con el argumento de la línea de comandos que recibió.
Revise el ejemplo `unit2/use_exec/example2.c` para entender el paso de argumentos.
Esta llamada al sistema reemplaza las instrucciones del proceso hijo y asi no es un clone del padre.
3. En el proceso padre haga la llamada a `wait` de manera que éste se bloquea hasta que el hijo termine y devuelva su estado de terminación.
4. Verifique el estado de terminación del hijo. Si el proceso hijo termina sin errores, el padre debe lanzar un nuevo hijo,
pero esta vez ejecuta `/usr/bin/wc` con el mismo argumento que recibió de la línea de comandos.
5. Desde el padre se vuelve a verificar el estado de terminación del segundo hijo.
6. Si todos los estado de terminación son sin errores, haga la llamada a exit(0).
Si ocurrió un error en algun punto de la ejecución del hijo retorne el estado de terminación con error con exit(status).

**Nota:** antes de usar alguna función asegúrese de incluir las librerías de cabecera. Para saber que librerías incluir use la utilidad man y el nombre de la función para obtener la documentación. Ejemplo:

```
man fork
```
