#include "pa2m.h"
#include "src/csv.h"
#include "src/pokedex.h"
#include <string.h>

// ------- FFUNCIONES EXTRAS PARA LOS TESTS -------

bool leer_int(const char *str, void *ctx)
{
	return sscanf(str, "%d", (int *)ctx) == 1;
}

bool crear_string_nuevo(const char *str, void *ctx)
{
	char *nuevo = malloc(strlen(str) + 1);
	if (nuevo == NULL)
		return false;
	strcpy(nuevo, str);
	*(char **)ctx = nuevo;
	return true;
}

bool leer_caracter(const char *str, void *ctx)
{
	*(char *)ctx = *(char *)str;
	return true;
}

bool mostrar_pokemon(struct pokemon *pokemon, void *nada)
{
	printf("%s\n", pokemon->nombre);
	return true;
}

bool buscar_pokemon_que_inicie_con(struct pokemon *pokemon, void *letra)
{
	return !(pokemon->nombre[0] == *(char *)letra);
}

void agregar_valores(struct pokemon *pokemon, const char *nombre)
{
	pokemon->nombre = malloc((strlen(nombre) + 1) * sizeof(char));
	if (!pokemon->nombre) {
		return;
	}
	strcpy(pokemon->nombre, nombre);
	pokemon->tipo = 'X';
	pokemon->fuerza = 0;
	pokemon->destreza = 0;
	pokemon->resistencia = 0;
}

// ------- INICIO DE LOS TESTS ---------

void abrirUnArchivoInexistenteDebeRetornarNull()
{
	struct archivo_csv *archivo =
		abrir_archivo_csv("noexiste/noexiste/noexiste/no", ';');
	pa2m_afirmar(archivo == NULL, "El archivo no existe y se retorna NULL");
}

void abrirArchivoExistente()
{
	struct archivo_csv *archivo =
		abrir_archivo_csv("ejemplos/correcto.txt", ';');
	pa2m_afirmar(
		archivo != NULL,
		"El archivo existe y la estructura apunta al archivo abierto");
	cerrar_archivo_csv(archivo);
}

void parametrosNulosRetornanCeroEnLecturaDeLineas()
{
	struct archivo_csv *archivo =
		abrir_archivo_csv("ejemplos/pokedex.csv", ';');
	bool (*funciones[])(const char *, void *) = { crear_string_nuevo };
	char *nombre = NULL;
	void *punteros[] = { &nombre };
	pa2m_afirmar(leer_linea_csv(NULL, 1, funciones, punteros) == 0,
		     "Parametro del arhivo nulo retorna 0");
	pa2m_afirmar(leer_linea_csv(archivo, 1, NULL, punteros) == 0,
		     "Parametro de las funciones nulo retorna 0");
	pa2m_afirmar(leer_linea_csv(archivo, 1, funciones, NULL) == 0,
		     "Parametro de los punteros nulo retorna 0");
	cerrar_archivo_csv(archivo);
}

void abrirArchivoYParsearLosNombres()
{
	struct archivo_csv *archivo =
		abrir_archivo_csv("ejemplos/correcto.txt", ';');
	const char *resultado_esperado[6] = { "Pikachu",  "Charmander",
					      "Larvitar", "Cacnea",
					      "Togepi",	  "Floatzel" };
	// Se toma en consideración la primera columna para ver si hay errores al toman n columnas de m
	int cantidad_total = 0;
	char *nombre = NULL;
	char *nombres[6];
	bool (*funciones[])(const char *, void *) = { crear_string_nuevo };
	void *punteros[] = { &nombre };
	int posicion = 0;

	while (leer_linea_csv(archivo, 1, funciones, punteros) == 1) {
		nombres[posicion] = malloc((strlen(nombre) + 1) * sizeof(char));
		if (!nombres[posicion]) {
			for (int i = 0; i < posicion; i++) {
				free(nombres[i]);
			}
			cerrar_archivo_csv(archivo);
			return;
		}
		strcpy(nombres[posicion], nombre);
		free(nombre);
		posicion++;
	}
	for (int i = 0; i < 6; i++) {
		if (strcmp(nombres[i], resultado_esperado[i]) == 0) {
			free(nombres[i]);
			cantidad_total++;
		}
	}
	pa2m_afirmar(cantidad_total == 6,
		     "Todos los nombres agregados exitosamente");
	cerrar_archivo_csv(archivo);
}

void crearUnaPokedexValoresIniciales()
{
	struct pokedex *pokedex = pokedex_crear();
	pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 0,
		     "Se crea una pokedex y hay 0 pokemones");
	pokedex_destruir(pokedex);
}

void agregarValoresNulosPokedex()
{
	struct pokedex *pokedex = pokedex_crear();
	struct pokemon pokemon;
	pokemon.nombre = NULL;
	pa2m_afirmar(
		pokedex_agregar_pokemon(pokedex, pokemon) == false,
		"Ingresar un pokemon con nombre de direccion Nulo, es falso");
	char *nombre_pokemon = "Pikachu";
	agregar_valores(&pokemon, nombre_pokemon);
	pa2m_afirmar(pokedex_agregar_pokemon(NULL, pokemon) == false,
		     "Parametro Nulo pokedex, retorna falso");
	free(pokemon.nombre);
	pokedex_destruir(pokedex);
}

void agregarUnPokemon()
{
	struct pokedex *pokedex = pokedex_crear();
	char *nombre_pokemon = "Pikachu";
	struct pokemon pokemon;
	pokemon.nombre = NULL;
	agregar_valores(&pokemon, nombre_pokemon);
	if (!pokemon.nombre) {
		pokedex_destruir(pokedex);
		return;
	}
	pa2m_afirmar(
		pokedex_agregar_pokemon(pokedex, pokemon) == true,
		"Se agregó exitosamente el pokemon Pikachu, devuelve true");
	pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 1,
		     "Hay 1 pokemon almacenado");
	free(pokemon.nombre);
	pokedex_destruir(pokedex);
}

void agregarVariosPokemones()
{
	struct pokedex *pokedex = pokedex_crear();
	char *pokemones[] = { "Onix", "Mew", "Snorlax", "Eevee" };
	struct pokemon pokemon;
	pokemon.nombre = NULL;
	for (int i = 0; i < 4; i++) {
		pokemon.nombre =
			malloc((strlen(pokemones[i]) + 1) * sizeof(char));
		agregar_valores(&pokemon, pokemones[i]);
		(pokedex_agregar_pokemon(pokedex, pokemon));
		free(pokemon.nombre);
	}
	pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 4,
		     "Se agregaron exitosamente 4 pokemones");
	pokedex_destruir(pokedex);
}

void agregarPokemonesRepetidos()
{
	struct pokedex *pokedex = pokedex_crear();
	char *pokemones[] = { "Charmande", "Charmander", "Charmander",
			      "Charmander", "Charmander" };
	struct pokemon pokemon;
	pokemon.nombre = NULL;
	for (size_t i = 0; i < 5; i++) {
		pokemon.nombre =
			malloc((strlen(pokemones[i]) + 1) * sizeof(char));
		agregar_valores(&pokemon, pokemones[i]);
		pokedex_agregar_pokemon(pokedex, pokemon);
		free(pokemon.nombre);
	}

	pa2m_afirmar(
		pokedex_cantidad_pokemones(pokedex) == 5,
		"Se agregan 5 pokemones con mismo nombre, hay 5 pokemones en la pokedex");
	pokedex_destruir(pokedex);
}

void buscarUnPokemonEnPokedexVacio()
{
	struct pokedex *pokedex = pokedex_crear();
	char *nombre_pokemon = "Bulbasaur";
	pa2m_afirmar(pokedex_buscar_pokemon(NULL, nombre_pokemon) == NULL,
		     "Pasar NULL en el parametro de la pokedex, retorna NULL");
	pa2m_afirmar(pokedex_buscar_pokemon(pokedex, nombre_pokemon) == NULL,
		     "Pokedex vacio, buscar pokemon retorna NULL");
	pokedex_destruir(pokedex);
}

void agregarPokemonYBuscar()
{
	struct pokedex *pokedex = pokedex_crear();
	char *nombres_pokemon[] = { "Torchic", "Bulbasaur", "Lucario",
				    "Mario Bros" };
	struct pokemon pokemon;

	// Agrega primer pokemon
	agregar_valores(&pokemon, nombres_pokemon[0]);
	if (!pokemon.nombre) {
		pokedex_destruir(pokedex);
		return;
	}
	pokedex_agregar_pokemon(pokedex, pokemon);
	free(pokemon.nombre);
	pa2m_afirmar(
		strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[0])
			       ->nombre,
		       nombres_pokemon[0]) == 0,
		"Agrear un pokemon y buscar su nombre, devuelve el puntero al pokemon guardado");

	// Agrega segundo pokemon
	agregar_valores(&pokemon, nombres_pokemon[1]);
	if (!pokemon.nombre) {
		pokedex_destruir(pokedex);
		return;
	}
	pokedex_agregar_pokemon(pokedex, pokemon);
	free(pokemon.nombre);
	pa2m_afirmar(
		strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[0])
			       ->nombre,
		       nombres_pokemon[0]) == 0,
		"Agregar un segundo pokemon y buscar el primero que ingresé, retorna el puntero al pokemon buscado");
	pa2m_afirmar(
		strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[1])
			       ->nombre,
		       nombres_pokemon[1]) == 0,
		"Buscar el segundo pokemon que ingresé, devuelve puntero al pokemon");

	// Agrega un tercer pokemon
	agregar_valores(&pokemon, nombres_pokemon[2]);
	if (!pokemon.nombre) {
		pokedex_destruir(pokedex);
		return;
	}
	pokedex_agregar_pokemon(pokedex, pokemon);
	free(pokemon.nombre);
	pa2m_afirmar(
		strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[0])
			       ->nombre,
		       nombres_pokemon[0]) == 0,
		"Agregar un tercer pokemon y busca al primer que ingresé, devuelve el puntero");
	pa2m_afirmar(
		strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[1])
			       ->nombre,
		       nombres_pokemon[1]) == 0,
		"Agregar un tercer pokemon y busca el segundo que ingresé, devuele su puntero");
	pa2m_afirmar(strcmp(pokedex_buscar_pokemon(pokedex, nombres_pokemon[2])
				    ->nombre,
			    nombres_pokemon[2]) == 0,
		     "Buscar el tercer pokemon, devuelve puntero al pokemon");
	pa2m_afirmar(pokedex_buscar_pokemon(pokedex, nombres_pokemon[3]) ==
			     NULL,
		     "Buscar un pokemon que no ingresé, devuelve NULL");
	pokedex_destruir(pokedex);
}

void agregarPokemonesRepetidosYBuscar()
{
	struct pokedex *pokedex = pokedex_crear();
	char *nombres_pokemones[] = { "Onix", "Snorlax", "Snorlax", "Ratata" };
	size_t posicion__nombre_repetido = 2;
	struct pokemon pokemon;
	for (size_t i = 0; i < 4; i++) {
		agregar_valores(&pokemon, nombres_pokemones[i]);
		if (!pokemon.nombre) {
			pokedex_destruir(pokedex);
			return;
		}
		if (i == posicion__nombre_repetido) {
			pokemon.fuerza = 100;
		}
		pokedex_agregar_pokemon(pokedex, pokemon);
		free(pokemon.nombre);
	}

	pa2m_afirmar(
		pokedex_buscar_pokemon(pokedex, nombres_pokemones[1])->fuerza ==
			100,
		"Cuando se ingresa un pokemon con nombre repetido, siempre devolverémos el puntero al último");
	pokedex_destruir(pokedex);
}

void iterarPokedexVacia()
{
	struct pokedex *pokedex = pokedex_crear();
	pa2m_afirmar(pokedex_iterar_pokemones(NULL, mostrar_pokemon, NULL) == 0,
		     "No pasar la pokedex, retorna 0 iteraciones");
	pa2m_afirmar(
		pokedex_iterar_pokemones(pokedex, mostrar_pokemon, NULL) == 0,
		"Crear una pokedex, al no tener pokemones, da 0 iteraciones");
	pa2m_afirmar(pokedex_iterar_pokemones(pokedex, NULL, NULL) == 0,
		     "Pasar puntero a NULL, da 0 iteraciones");
	pokedex_destruir(pokedex);
}

void iteraYSePuedeDetener()
{
	struct pokedex *pokedex = pokedex_crear();
	char *nombres_pokemon[] = { "Mimiau", "Onix", "Ratata", "Snorlax" };
	struct pokemon pokemon;
	pokemon.nombre = NULL;
	for (size_t i = 0; i < 4; i++) {
		agregar_valores(&pokemon, nombres_pokemon[i]);
		if (!pokemon.nombre) {
			pokedex_destruir(pokedex);
			return;
		}
		pokedex_agregar_pokemon(pokedex, pokemon);
		free(pokemon.nombre);
	}
	char nombre_iniciada_con_R = 'R';
	char nombre_iniciada_con_A = 'A';
	pa2m_afirmar(
		pokedex_iterar_pokemones(pokedex, buscar_pokemon_que_inicie_con,
					 &nombre_iniciada_con_R) == 3,
		"Buscar un pokemon y que corte por su condicion, retorna la cantidad de pokemones iterados");
	pa2m_afirmar(
		pokedex_iterar_pokemones(pokedex, buscar_pokemon_que_inicie_con,
					 &nombre_iniciada_con_A) ==
			pokedex_cantidad_pokemones(pokedex),
		"Buscar un pokemon y que no corte por su condicion, itera todos los pokemones");
	pokedex_destruir(pokedex);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas de archivos CSV");
	abrirUnArchivoInexistenteDebeRetornarNull();
	abrirArchivoExistente();
	pa2m_nuevo_grupo("Pruebas de lectura en archivos CSV");
	parametrosNulosRetornanCeroEnLecturaDeLineas();
	abrirArchivoYParsearLosNombres();
	pa2m_nuevo_grupo("Pruebas de Pokedex");
	crearUnaPokedexValoresIniciales();
	pa2m_nuevo_grupo("Pruebas agregar Pokemones con parametros NULL");
	agregarValoresNulosPokedex();
	pa2m_nuevo_grupo("Prueba se agrega un pokemon a la pokedex");
	agregarUnPokemon();
	pa2m_nuevo_grupo("Prueba se agregan varios pokemones");
	agregarVariosPokemones();
	agregarPokemonesRepetidos();
	pa2m_nuevo_grupo("Pruebas con buscar pokemones");
	buscarUnPokemonEnPokedexVacio();
	agregarPokemonYBuscar();
	agregarPokemonesRepetidosYBuscar();
	pa2m_nuevo_grupo("Pruebas con el iterador");
	iterarPokedexVacia();
	iteraYSePuedeDetener();

	return pa2m_mostrar_reporte();
}
