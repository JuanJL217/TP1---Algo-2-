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

# csv.c

### c = cantidad de caracteres
### m = cantidad de columnas

## `abrir_archivo_csv` O(1)

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

## `leer_linea_csv` O(c² + m)

En primer lugar, dentro de la función, como no sabemos cuantos caracteres van a ser, y no queremos tener un limite de caracteres,vamos a crear una variable `char* texto` de forma dinamica, donde almacenaremos la cadena de caracteres de una linea del archivo (de manera local dentro d ela función). Esta parte de la función se ejecuta `c` veces. De esta manera tenermos la linea de texto en una varible. En el peor de los casos, vamos a tener que redimencionar ese vector `k` veces, el costo de usar `realloc()` es `O(c)`, porque estamos pasando los caracteres del anterior bloque, esto hace que el ciclo quede: `n(O(k*c))`, pero como sabemos que `k` va a ser más chico que `c` a medida que vayan entrando más caracteres, nos queda `c(O(c))` => `O(c²)` (1).
```c
	while ((valor_ascii = fgetc(archivo->archivo)) != EOF &&
	       valor_ascii != '\n') {
		if (tamaño_del_texto >= (capacidad_linea * 75) / 100) {
			if (!redimencionar_linea_texto(&texto,
						       &capacidad_linea)) {
				free(texto);
				return columna_posicion;
```

En la funcion de `dividir_partes`, sin entrar en tantos detalles, va a iterar la linea de texto y separar palabra por palabra cuando haya una separador presente, lo cual es `O(c)` y por otro lado, dentro de ese ciclo, cuando encuentre separados, copiará los caracteres desde un inicio hasta un final, lo cual lo hace tambien `O(c)`, porque en el peor de los casos, va a tener que copiar toda la linea, siendo así, la función `dividir_string` queda como `O(c²)` (2).

```c
struct Partes *partes = dividir_string(texto, archivo->separador);
```

De ahí entramos en un ciclo while, la cual va a estar usando puntero a funciones y punteros a void, esta parte de la función. Al ser un ciclo que dependa de la cantidad de columnas, decimos que el ciclo while es `O(m)`, siendo `m` la cantidad de columnas que tiene cada texto (cada columna es definido por el separador), pero adentro de la funcion, tenemos lo que son las funciones de parseo, en el peor de los casos, todos van a ser string, y al hacer `strcpy` y dependerán de la cantidad de caracteres que haya, siendo entonces O(c), entonces nos queda que el ciclo es `m(O(c))` => `O(m*c)` (3) No seria solo `O(c)`, porque estariamos ignorando el impacto del peor caso, que sean muchas columnas para procesar. Si salimos del ciclo while por su condición, sabemos que iteró todas las columnas, por eso al final hay un `return columas`. Si en alguna función es NULL e iba en la tercera iteracion, decimos que itero solo 2 veces, pero si en la tercera iteracion dio falso, decimos que iteró 3 veces.

```c
	while (columna_posicion < columnas) {
		if (!funciones[columna_posicion]) {
			liberar_partes(partes);
```

Mencionar que, hacer free(partes), nos cuesta `O(m)` (4), ya que, en el peor de los casos, vamos a liberar cada columna que usamos.

Entonces nos quedaria en total que la función queda como `O(c²)`(1) + `O(c²)`(2) + `O(m*c)`(3) + `O(m)`(4) => `O(c² + m*c + m)`, pero `m*c` > `m`, ya que, el peor de los casos haya 1000 columnas y dentro de cada columna, 1000 caracteres, viendo de esa manera `1000*1000 > 1000`, entonces nos quedamos con: `O(c² + m*c)`, ahora bien, ¿Cuál es más significativo entre esas 2 sumas? Lo mejor es dejarlo de esta manera, así la complejidad está dependiende de columnas y caracteres.

## `cerrar_archivo_csv` O(m)

Cuando cerramos el archivo, tambien liberamos memoria, y vamos a liberar m veces, porque tendremos m partes (columnas), así que la función queda `O(m)`.

```c
		for (int i = 0; i < partes->cantidad; ++i) { // O(m)
			free(partes->string[i]);
		}
```

# pokedex.c

### n = cantidad de pokemones

## `pokedex_crear` O(1)

Inicializamos la estructura pokedex donde almacenaremos los pokemones, será un vector dinamico con una capacidad inicial. Al ser una inicializacion de la estructura, es O(1);

```c
	inicializar_pokedex->vector_pokemones =
		malloc(CAPACIDAD_INICAL_POKEDEX * sizeof(struct pokemon));
```

## `pokedex_iterar_pokemones` O(n)

En este caso prefiero mencionar la funcion que itera a los pokemons, porque es lo que usaré en 2 funciones de la pokedex. Esta funcion, al ser una iteracion, va a iterar, en el peor de los casos, a todos los pokemones que seria un costo de `O(n)`, o salir cuando la funcion que se pasa por parametro de `false`. Si iteramos 4 pokemones y al siguiente pokemon, que seria el 5to, da `falso`, retornamos 5, porque fue la cantidad de pokemones que nuestra función pudo iterar. Si sale del ciclo for, sabemos que iteró a todos los pokemones, o sea, retornamos la cantidad de pokemones que hay en la pokedex. Cabe destacar que, si aun no hemos agregado a un pokemon, siempre retornamos 0, porque no hay pokemon para iterar.

```c
	    pokedex->cantidad_pokemones == 0) {
		return 0;
	}
```

## `pokedex_agregar_pokemon`

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

