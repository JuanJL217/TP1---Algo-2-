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

Teniendo en cuenta que cada vez que hago uso de un `malloc()` significa que pido bloque(s) de memoria al heap de manera dinamica, lo cual es O(1), por lo que si pido, tengo que liberar usando `free()`. inicializar\declarar una variable, if, break, operadores matematico son `O(1)`. Las funciones `strcpy` y `strcmp` son O(c), ya que se está comparando caracter por caracter.


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
<img width="70%" src="img/inicializar_archivo_csv.png">
</div>

---

## `leer_linea_csv` O(c²)

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

En la funcion de `dividir_partes`, va a iterar toda la cadena de caracteres en busca de separadores que, como el nombre de la función indica, dividirá en partes, o sea, sabremos la cantidad de columnas, lo cual es `O(c)`. Luego iteraremos nuevamente la cadena de caracteres, y donde veamos un separador, podremos el bloque de caracteres anterior al caracter separador almacenado dinamicamente en el heap, porque la estructura tiene un `char** string`, lo que es un vector de punteros a `char*`, osea, cada posicion del vector, va a apuntar a una direcion de memoria donde inicia cada parte del string. Hay 2 casos en esta funcion, cuando itera todo y no encuentra separador y cuando itera y encuentra muchos separadores, ¿cuál es el peor caso?.
Si iteramos y no vemos un caracter separador dentro del while, significa que haremos un llamado a `agregar_partes` una sola ves, o sea, tendremos una sola columna, lo que es `O(c)`, pero si hay varios separadores, seria `O(c²)`, y ese es el caso menos favorable.

```c
	struct Partes *partes = dividir_string(texto, archivo->separador);
```

De ahí entramos en un ciclo while, la cual va a estar usando puntero a funciones y punteros a void, esta parte de la función. Al ser un ciclo que dependa de la cantidad de columnas, decimos que el ciclo while es `O(m)`, siendo `m` la cantidad de columnas que tiene cada texto (cada columna es definido por el separador), pero adentro de la funcion, tenemos lo que son las funciones de parseo, en el peor de los casos, todos van a ser string, y al hacer `strcpy` y dependerán de la cantidad de caracteres que haya, siendo entonces O(c), entonces nos queda que el ciclo es `m(O(c))` => `O(m*c)` (3) No seria solo `O(c)`, porque estariamos ignorando el impacto del peor caso, que sean muchas columnas para procesar. Si salimos del ciclo while por su condición, sabemos que iteró todas las columnas, por eso al final hay un `return columas`. Si en alguna función es NULL e iba en la tercera iteracion, decimos que itero solo 2 veces, pero si en la tercera iteracion dio falso, decimos que iteró 3 veces.

```c
	while (columna_posicion < columnas) {
		if (!funciones[columna_posicion]) {
			liberar_partes(partes);
```

Mencionar que, hacer free(partes), nos cuesta `O(m)` (4), ya que vamos a liberar cada columna que tengamos almacenado en el heap.

```c
	for (int i = 0; i < partes->cantidad; ++i) {
		free(partes->string[i]);
	}
```

Entonces nos quedaria en total que la función queda como `O(c²)`(1) + `O(c²)`(2) + `O(m*c)`(3) + `O(m)`(4) => `O(2c² + m*c + m)`, `2` al ser una constante, se toma como `1`; ahora bien, tenemos estas 3 variables `c²`, `m*c` y `c`. En el peor de los casos, la division y copias de subcadenas tienes una complejidad de `O(c²)`, y en ese peor caso, la `m` puede ser proporcional a `c`, o sea, peude ser `O(c)`, o sea, el peor caso es que cada caracter tengas un seprador, que puede llegar a ser tan grande como `c`, por ejemplo: ";;;;;;", con serparador ';', aquí podemos ver que la cantidad de columnas `m`, será proporcional a `c`, porque habrá `c+1` columnas, o sea, `c` columnas. Entonces recorreremos el arreglo `c` veces y agregagremos las partes, aunque sean vacias, `c` veces, entonces llegamos a la conclusión de que, `m` puede llegar a `c`, `m -> c` => `O(c² + c*c + c)`, lo que conlleva a que esta función sea `O(c²)` en el peor de los casos.

## `cerrar_archivo_csv` O(1)

Cerraremos el archivo que abrimos desde un comienzo, y liberaremos la memoria que pedimos al inicializar nuestra estructura. Tanto cerrar el archivo, como liberar la estructura cuestan `O(1)`.

# pokedex.c

### n = cantidad de pokemones

## `pokedex_crear` O(1)

Inicializamos la estructura pokedex donde almacenaremos los pokemones, será un vector dinamico con una capacidad inicial. Al ser una inicializacion de la estructura, es O(1);

```c
	inicializar_pokedex->vector_pokemones =
		malloc(CAPACIDAD_INICAL_POKEDEX * sizeof(struct pokemon));
```

<div align="center">
<img width="70%" src="img/inicializar_pokedex.png">
</div>


## `pokedex_iterar_pokemones` O(n * g(h))

En este caso prefiero mencionar la funcion que itera a los pokemons, porque es lo que usaré en 2 funciones de la pokedex. Esta funcion, al ser una iteracion, va a iterar, en el peor de los casos, a todos los pokemones que seria un costo de `O(n)`, o salir cuando la funcion que se pasa por parametro de `false`. Si iteramos 4 pokemones y al siguiente pokemon, que seria el 5to, da `falso`, retornamos 5, porque fue la cantidad de pokemones que nuestra función pudo iterar. Si sale del ciclo for, sabemos que iteró a todos los pokemones, o sea, retornamos la cantidad de pokemones que hay en la pokedex. Cabe destacar que, si aun no hemos agregado a un pokemon, siempre retornamos 0, porque no hay pokemon para iterar.

```c
	    pokedex->cantidad_pokemones == 0) {
		return 0;
	}
```

Pero hay un problema con la complejidad algoritmica, al ser puntero a funciones, nosotros podemos hacer cualquier cosa con esa función, y de eso dependerá cuanto sea el coste de esta función: Desde hacer `printf` a cada pokemons que sería `(O(1)` => `O(n)` o iterar el nombre de cada pokemon, que sería `O(c)`, por eso digo que el coste algoritmico de esta función es `O(n * g(h))` con `g(h)` la complejidad temporal de la función que le pasemos.

## `pokedex_agregar_pokemon` O(n * c)

Primero explicar que, cuando agregamos un pokemon, la variable de tipo `struct pokemon`, va a tener un `char* nombre`, pero nosotros no podemos simplemente hacer `vector[posicion] = pokemon`, porque por afuera de la función, así como dimos memoria en el heap para el nombre del pokemon, por afuera también lo vamos a liberar, ya que por afuera, la variable nombre de la estructura pokemon, va a estar actualizandose y tendrá distintas direcciones de memoria, por eso hacemos que adentro de nuestra función, se cree un nuebo bloque de memoria para guardar ahí el nombre del pokemon y no sea liberaro y nos dé problemas con que se perdió dicha memoria.

```c
	char *nueva_ubicacion_nombre =
		malloc((strlen(pokemon.nombre) + 1) * sizeof(char));
	if (!nueva_ubicacion_nombre) {
		return false;
	}

	strcpy(nueva_ubicacion_nombre, pokemon.nombre);
	pokemon.nombre = nueva_ubicacion_nombre;
```

Ahora, aquí viene la magia de la funcion `pokedex_iterar_pokemones`. Como sabemos, dicha función iterará hasta encontrar un punto de quiebre, para buscar la posición donde debe estar el pokemon, el cual, en forma textual seria: "Si la posicion de este pokemon en el vector, es `mayor` que el nombre del pokemon que quiero agregar, devuelve false", o sea, que ahí encontré el pokemon donde debe estar posicionado, hacer esto, cuesta `O(c)`.

```c
	bool buscar_posicion(struct pokemon *pokemon_actual, void *nombre_pokemon)
	{
		return !(strcmp(pokemon_actual->nombre, (char *)nombre_pokemon) > 0);
	}
```
Entonces hacemos las distintas comparaciones para saber donde se posiciona el pokemon, hallas la posicion me cuesta `O(n*c)`, porque itero n pokemones y companos nombres. Aquí algo para aclarar, hay 2 posibles escenarios. Si el nombre del nuevo pokemon es mayor que todos, se posiciona al último, claro está, pero hay 2 problemas: Cuando `pokedex_iterar_pokemones` devuelve la cantidad de pokemones que tenemos, puede ser porque, en ninguna posicion del vector dio `false`, o sea, que puso salir bien del ciclo for, o que el último pokemon dio `false` de ahí porque el uso de este `if`:

```c
	if (no_hay_pokemones(pokemones_iterados) ||
	    (es_el_ultimo_pokemon(pokemones_iterados,
				  pokedex->cantidad_pokemones) &&
	     nombre_pokemon_en_el_vector_es_menor(
		     pokedex->vector_pokemones[pokemones_iterados - 1].nombre,
		     pokemon.nombre))) {
		pokedex->vector_pokemones[pokemones_iterados] = pokemon;
```

La manera de insertar es: encuentro la posicion, y corro todo un espacio a la derecha, siguiendo esta lógica, eso me cuesta `O(n)`, entonces tengo que las operaciones relevantes son `O(n*c + n)`, como `n*c` es dominante ante `O(n)`, nos quedamos con `O(n*c)`

## `pokedex_cantidad_pokemones` O(1)

Devuelve la cantidad de pokemones almacenado, y si algún parametro es NULL, entonces devolvemos 0;

## `pokedex_buscar_pokemon` O(n * c)

Esta es otra función que también usé la función `pokedex_iterar_pokemones`, busca el pokemon y rompe donde: "Si el nombre del pokemon donde estoy parado en el vector, es igual al que estoy buscando, retorna `false`. Al ser una funcion que compara la cadena de caracteres, eso cuesta `O(c)`, por lo tanto, la función es `O(n * c)`;

## `pokedex_destruir` O(n)

Para destruir la pokedex, debemos liberar la memoria que hemos pedido, por lo que vamos a ir liberando desde lo más profundo, para afuera, así como en `liberar_partes` de `split.c`. Vamos a iterar cada posicion del vector de pokemones, y como mencioné cuando agregué pokemones, yo asigné memoria para el nombre, entonces debo liberar la memoria de cada nombre de todos los pokemones, una vez que hago eso, puedo liberar la memoria del vector que almacena pokemones, y luego la estructura de la pokedex, lo que pesa en esta función es la iteracion de los `n` pokemones, por eso es `O(n)`

```c
	for (size_t i = 0; i < pokedex->cantidad_pokemones;
			i++) {
		free(pokedex->vector_pokemones[i].nombre);
	}
```