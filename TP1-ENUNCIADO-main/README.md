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

En el archivo `csv.c` la función `abrir_archivo_csv` inicializo la estructura `struct archivo_csv` con la cual tendre la direcion
de memoria del archivo, dicha dirección estará alojada en el heap, debido a que la funcion `malloc()` me da un bloque de memoria
en el heap, para su uso dinamico. Si hay un error en asignar memoria, retorno NULL

```c
struct archivo_csv *archivo_csv = malloc(sizeof(struct archivo_csv));
if (!archivo_csv) {
	fclose(archivo);
	return NULL;
}
```
<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Respuestas a las preguntas teóricas
Incluír acá las respuestas a las preguntas del enunciado (si aplica).
