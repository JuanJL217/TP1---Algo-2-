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

## Consideraciones

Teniendo en cuenta que cada vez que hago uso de un `malloc()` significa que pido bloque(s) de memoria al heap de manera dinamica, lo cual es O(1), por lo que si pido, tengo que liberar usando `free()`. inicializar\declarar una variable, if, break, operadores matematico son `O(1)`. Las funciones `strcpy` y `strcmp` son O(c).


### Estructura para la lectura de archivo

## csv.c

### c = cantidad de caracteres
### m = cantidad de columnas

### `abrir_archivo_csv` O(1)

Inicializa la estructura `struct archivo_csv` y devuelvo su dirección de memoria. Esta estructura inicializada, está ubicada en el heap, en un bloque de memoria de manera dinamica debido al uso de `malloc()`. 
Esta estructura guarda un puntero al tipo de dato FILE*, donde se encuentra nuestro archivo. En qué parte de la memoria se situa el archivo, va a varias del sistema operativo, pero apriori, diremos que en el heap.
Al ser inicializaciones de malloc, inicializar variables, podemos decir que esta funcion es `O(1)`.

```c
struct archivo_csv *archivo_tp1 = abrir_archivo_csv(argv[1], ';');
```
<div align="center">
<img width="70%" src="img/abrir_archivo_csv.png">
</div>

---

### `leer_linea_csv` O(c² + m)

En primer lugar, dentro de la función, como no sabemos cuantos caracteres van a ser, y no queremos tener un limite de caracteres,vamos a crear una variable `char* texto` de forma dinamica con `malloc()`, donde almacenaremos la cadena de caracteres de cada linea del archivo. esta parte de la función es se ejecuta `c` veces. De esta manera tenermos la linea de texto en una varible. En el peor de los casos, vamos a tener que redimencionar ese vector `k` veces, el costo de usar `realloc()` es `O(c)`, porque estamos creando un nuevo bloque, con mayor capacidad, esto es un coste exponencial, siendo  donde copiamos los caracteres del anterior bloque, esto hace que el ciclo quede: `n(O(k*c))`, pero como sabemos que `k` va a ser mucho menos que `c`, nos queda que este ciclo nos queda `c(O(c))` => `O(c²)`.
```c
	while ((valor_ascii = fgetc(archivo->archivo)) != EOF &&
	       valor_ascii != '\n') {
		if (tamaño_del_texto >= (capacidad_linea * 75) / 100) {
			if (!redimencionar_linea_texto(&texto,
						       &capacidad_linea)) {
				free(texto);
				return columna_posicion;
```

En la funcion de `dividir_partes`, sin entrar en tantos detalles, va a iterar la linea de texto que guardamos y separar palabra por palabra cuando haya una separador presente, lo cual, lo hace tambien `O(c)`.

```c
struct Partes *partes = dividir_string(texto, archivo->separador);
```

De ahí entramos en un ciclo while, la cual va a estar usando puntero a funciones y punteros a void, esta parte de la función. Al ser un ciclo que dependa de que sea menor que columna, y siendo columna una variable que puede ser cualquier valor, decimos que el ciclo while es `O(m)`, siendo `m` la cantidad de columnas que tiene cada texto (cada columna es definido por el separador). Si salimos del ciclo while por su condición, sabemos que iteró todas las columnas, por eso al final hay un `return columas`. Si en alguna función es NULL e iba en la tercera iteracion, decimos que itero solo 2 veces, pero si en la tercera iteracion dio falso, decimos que iteró 3 veces. Dentro del cilo, va a haber funciones, pero como está destinado al parseo de datos, decimos que adentro del ciclo es `O(1)`, entonces este ciclo no queda como `O(m)`.

```c
	while (columna_posicion < columnas) {
		if (!funciones[columna_posicion]) {
			liberar_partes(partes);
```

Entonces nos quedaria en total que la función queda como `O(c²) + O(c) + O(m)` => `O(c² + m)`.

### `cerrar_archivo_csv` O(m)

Como pedimos memoria por medio de lo `malloc()`, es nuestro deber tener que liberarla, para que no haya fugas de memoria.
Así como liberarmos memoria en al función anterior, fue memoria que se utilizó en ese momento, pero también hay memoria que pedimos y que seguirá usandose mientras no lo liberemos, como es el caso de inicializar la estructura en `abrir_archivo_csv()`, y tambien cerrar el archivo que aun tenemos ligado. Como `free()` y `fclose()` son `O(1)`, entonces la función es `O(1)`.


## pokedex.c

### n = cantidad de pokemones

### `pokedex_crear` O(1)

Inicializamos la estructura pokedex donde almacenaremos los pokemones, será un vector dinamico con una capacidad inicial. Al ser una inicializacion de la estructura, es O(1);

```c
	inicializar_pokedex->vector_pokemones =
		malloc(CAPACIDAD_INICAL_POKEDEX * sizeof(struct pokemon));
```

### `pokedex_iterar_pokemones` O(n)

En este caso prefiero mencionar la funcion que itera a los pokemons, porque es lo que usaré en 2 funciones de la pokedex. Esta funcion, al ser una iteracion, va a iterar, en el peor de los casos, a todos los pokemones que seria un costo de `O(n)`, o salir cuando la funcion que se pasa por parametro de `false`. Si iteramos 4 pokemones y al siguiente pokemon, que seria el 5to, da `falso`, retornamos 5, porque fue la cantidad de pokemones que nuestra función pudo iterar. Si sale del ciclo for, sabemos que iteró a todos los pokemones, o sea, retornamos la cantidad de pokemones que hay en la pokedex. Cabe destacar que, si aun no hemos agregado a un pokemon, siempre retornamos 0, porque no hay pokemon para iterar.

```c
	    pokedex->cantidad_pokemones == 0) {
		return 0;
	}
```

### `pokedex_agregar_pokemon`

Primero explicar que, cuando agregamos un pokemon, la variable de tipo `struct pokemon`, va a tener un `char* nombre`, pero nosotros no podemos simplemente hacer `vector[posicion_del_pokemon] = pokemon`, porque por afuera de la función, así como dimos memoria en el heap para el nombre del pokemon, por afuera también lo vamos a liberar, ya que por afuera, la variable nombre de la estructura pokemon, va a estar actualizandose y tendrá distintas direcciones de memoria, por eso hacemos que adentro de nuestra función, se cree un nuebo bloque de memoria para guardar ahí el nombre del pokemon.

```c
	char *nueva_ubicacion_nombre =
		malloc((strlen(pokemon.nombre) + 1) * sizeof(char));
	if (!nueva_ubicacion_nombre) {
		return false;
	}

	strcpy(nueva_ubicacion_nombre, pokemon.nombre);
	pokemon.nombre = nueva_ubicacion_nombre;
```

Ahora, aquí viene la magia de la funcion `pokedex_iterar_pokemones`, porque vamos a tener 3 tipos de opciones:
1) Si no hay pokemones en la pokedex, posicionará al pokemon en la primera posicion.
2) Si la funcion iterar dio como resultado la cantidad de pokemones, puede ser por 2 motivos, si el último pokemon, la funcion de iteración dio `false` o si iteró todo sin dar `false`, de ahí la comparación si el último pokemon, su nombre es menor o no, si es menor, el nuevo pokemon se posicion a la derecha, caso contrario, el pokemon del vector se mueve a la derecha y el nuevo pokemon entra en esa posicion "vacia".
3) Si el pokemons se encuentra antes del último pokemon, donde está el pokemon con un nombre mayor al que queremos guardar, desde ahí, en adelante, se moverán un espacio y en el espacio vacio, pondré el nuevo pokemon.

