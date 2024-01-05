# ﻿pa4-u6: Simulador de Virtual Memory parte 1

Este proyecto consiste en escribir un programa paging demand.c que traduzca direcciones lógicas a fisicas para un espacio de direcciones virtuales de tamaño 2^16 = 65.536 bytes. El programa leerá de un archivo que contiene direcciones lógicas y, utilizando una tabla de páginas, traducirá cada dirección lógica a su dirección fisica correspondiente y obtendrá como salida el valor en bytes almacenado en la dirección fisica traducida.

El objetivo de aprendizaje es utilizar la simulación para comprender los pasos necesarios para traducir direcciones lógicas a fisicas. Esto incluirá la resolución de fallas de página mediante la paginación por demanda, sin incluir la administración de un TLB y ni la implementación de un algoritmo de reemplazo de página.

## Especificaciones

El programa leerá un archivo que contiene varios números enteros de 32 bits que representan direcciones lógicas. Sin embargo, la traducción debe considerar los 16 bits más a la derecha de cada dirección lógica. Estos 16 bits se dividen en (1) un número de página de 8 bits y (2) un desplazamiento de página de 8 bits. Por lo tanto, las direcciones se estructuran como se muestra a continuación:

- 16 bits
- VPN 8 bits
- Offset 8 bits

![image](https://github.com/NoeliaIntriago/pa-os-2023/assets/67925801/94a4f010-15e7-4496-9bb0-78efa1ab4557)


La parte en rojo no se debe considerar.

- Otros detalles incluyen lo siguiente: • 2^8 entradas en la tabla de páginas
- Tamaño de página de 2^8 bytes • 256 frames
- Memoria fisica de 65.536 bytes (256 frames × 256 bytes de tamaño del frame)
  
Además, el programa solo debe preocuparse por leer direcciones lógicas y traducirlas a sus direcciones fisicas correspondientes. No es necesario admitir la escritura en el espacio de direcciones lógicas.

### ﻿Traducción de direcciones 

Se recomienda elegir un tipo de dato sin signo y apropiado para almacenar 32 bits. Luego, para dividir el número de 32 bits y obtener el número de página y el offset debe utilizar los operadores para enmascaramiento y desplazamiento de bits. 

El programa traducirá direcciones lógicas a fisicas utilizando solo una tabla de páginas. De manera que, el número de frame se obtiene de la tabla de páginas o se produce un fallo de página (page fault). El diagrama de arquitectura de traducción de direcciones muestra este esquema. (ver slides9 diapositiva 29). 

### Manejo de fallos de página 

El programa implementará la paginación por demanda. El almacén de respaldo está representado por el archivo BACKING_STORE.bin, un archivo binario de tamaño 65,536 bytes. Cuando ocurre una falla de página, leerá una página de 256 bytes del archivo BACKING STORE y la almacenará en un frame (marco de página) disponible en la memoria fisica. Por ejemplo, si una dirección lógica con el número de página 15 resultó en un fallo de página, su programa deberia leer la página 15 del BACKING_STORE (recuerde que las páginas comienzan en 0 y tienen un tamaño de 256 bytes) y la almacenaria en un frame en memoria fisica. Una vez que se almacena este frame se debe actualizar la tabla de páginas, los accesos siguientes a la página 15 se resolverán mediante la tabla de páginas. Tanto para la tabla de páginas como la memoria fisica se recomienda utilizar arreglos de 256. 

Deberá tratar BACKING STORE.bin como un archivo de acceso aleatorio para que pueda buscar aleatoriamente ciertas posiciones del archivo para su lectura. Se recomienda, para acceder al archivo el uso de un puntero a file, y el uso de las funciones de la biblioteca estándar de C para realizar operaciones de E/S, incluidas: `fopen()`, `fread()`, `fseek()` y `fclose()`.

﻿El tamaño de la memoria fisica es el mismo que el tamaño del espacio de direcciones virtuales (65,536 bytes), por lo que no debe preocuparse por los reemplazos de página durante un fallo de página. Recuerde que en sistemas reales hay una menor cantidad de memoria fisica pero no resolveremos esto en este proyecto.

### Archivo de prueba

Se proporciona el archivo addresses.txt, que contiene valores enteros que representan direcciones lógicas que van de 0 a 65535 (el tamaño del espacio de direcciones virtuales). El programa debe abrir este archivo, leerá cada dirección lógica y la traducirá a su dirección fisica correspondiente, y generará el valor del byte almacenado en la dirección fisica.

## Especificaciones de las salidas

El programa deberia ejecutarse de la siguiente manera:

```
./pagingdemand addresses.txt data.txt
```

Donde address.txt es el nombre del archivo que contiene las direcciones lógicas y data.txt es el nombre del archivo de salida que contiene lo que se describe más abajo.

El programa leerá el archivo address.txt, que contiene N direcciones lógicas que van de 0 a 65535. También, debe traducir cada dirección lógica a una dirección física y determinar el contenido del byte almacenado en la dirección fisica traducida. (Recuerde que en el lenguaje C, el tipo de dato char ocupa un byte de almacenamiento, por lo que se sugiere usar este tipo). Este programa deberá generar la salida en un archivo de texto (en el ejemplo el archivo es de nombre data.txt). El formato del archivo de salida es el siguiente: cada linea es la traducción de una dirección virtual (véase la figura la figura), y contiene:

```
Virtual address: 16916 Physical address: 20 Value: 0
Virtual address: 62493 Physical address: 285 Value: 0
Virtual address: 30198 Physical address: 758 Value: 29
Virtual address: 53683 Physical address: 947 Value: 108
﻿Virtual address: 40185 Physical address: 1273 Value: 0
...
```

1. La dirección lógica que se está traduciendo (el valor entero que se lee de address.txt).
2. La dirección fisica traducida (la traducida por el programa).
3. El valor del byte almacenado en la memoria fisica en la dirección traducida.

En repositorio de teams en la carpeta unité encontrará el archivo correct.txt, que contiene los valores de salida correctos para el archivo address.txt, que también estará disponible. Debe utilizar este archivo para determinar si su programa está traduciendo correctamente direcciones lógicas a fisicas.
