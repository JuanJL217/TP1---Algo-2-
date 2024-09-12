<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP1

# MANEJOS DE ARCHIVOS Y ESTRUCTURAS

## Alumno: Juan Ernesto Juarez Lezama - 110418 - jjuarez@fi.uba.ar

- Para compilar:

```bash
gcc -g tp1.c src/csv.c src/pokedex.c src/split.c -o tp1
```

- Para ejecutar:

```bash
./tp1 ejemplos/pokedex.csv
```

- Para ejecutar con valgrind:
```bash
valgrind ./tp1 ejemplos/pokedex.csv
```

---

##  Funcionamiento

Para la implementación de este TP nos piden leer un archivo como argumento de un programa compilado.
Tendrémos 2 estructuras, para la lectura de archivo y para almacenar elemento, en este caso, pokemones.

Acá muestro un diagrama de como sería el flujo del TP para que haga lo que nos piden.

<div align="center">
<img width="70%" src="img/flujo_tp1.png">
</div>

## Estructuras

### Estructura para la lectura de archivo

Teniendo en cuenta que cada vez que hago uso de un `malloc()` significa que pido bloque(s) de memoria al heap de manera dinamica, lo cual es O(1), por lo que si pido, tengo que liberar luego con un `free()`. 

### `abrir_archivo_csv` O(1)
En el archivo `csv.c` la función `abrir_archivo_csv` iinicializa la estructura `struct archivo_csv` y devuelvo su puntero. Esta estructura inicializada, se almacena en el heap, en un bloque de memoria de manera dinamica debido al uso de `malloc()`. 
En resumen, cuando inicializo una varible de tipo `struct archivo_csv*`con la funcion `abrir_archivo_csv` retorno un puntero a la estructura almacenada en el heap, que dicha estructura, guarda un puntero, también en el heap, de tipo FILE*, donde se encuentra almacenado el archivo.

```c
struct archivo_csv *archivo_tp1 = abrir_archivo_csv(argv[1], ';');
```
<div align="center">
<img width="70%" src="img/abrir_archivo_csv.png">
</div>

---
