#include "pokedex.h"
#include <string.h>

const size_t CAPACIDAD_INICAL_POKEDEX = 6;
const size_t FACTOR_CRECIMIENTO_POKEDEX = 2;

struct pokedex {
	struct pokemon *vector_pokemones;
	size_t cantidad_pokemones;
	size_t capacidad;
};

//********* FUNCIONES EXRAS ***********

bool redimensionar_vector_pokemones(struct pokedex *pokedex) // =====> O(n)
{
	size_t nueva_capacidad =
		pokedex->capacidad * FACTOR_CRECIMIENTO_POKEDEX;
	struct pokemon *nuevo_bloque =
		realloc(pokedex->vector_pokemones,
			nueva_capacidad * sizeof(struct pokemon)); // O(n)
	if (!nuevo_bloque) {
		return false;
	}
	pokedex->vector_pokemones = nuevo_bloque;
	pokedex->capacidad = nueva_capacidad;
	return true;
}

bool buscar_posicion(struct pokemon *pokemon_actual, void *nombre_pokemon) // =====> O(c)
{
	return !(strcmp(pokemon_actual->nombre, (char *)nombre_pokemon) > 0);
}

bool buscar_pokemon(struct pokemon *pokemon_actual, // =====> O(c)
		    void *nombre_pokemon_a_buscar)
{
	return !(strcmp(pokemon_actual->nombre,
			(const char *)nombre_pokemon_a_buscar) == 0);
}

bool nombre_pokemon_en_el_vector_es_menor(char *nombre_pokemon_en_vector, // =====> O(c)
					  char *nombre_pokemon_nuevo)
{
	return strcmp(nombre_pokemon_en_vector, nombre_pokemon_nuevo) < 0;
}

bool es_el_ultimo_pokemon(size_t pokemones_iterados, size_t cantidad_pokemones) // ====> O(1)
{
	return pokemones_iterados == cantidad_pokemones;
}

bool pokemon_no_encontrado(size_t cant_pokemones_iterados, size_t cantidad_pokemones, struct pokemon* vector_pokemones, const char* nombre_a_buscar)
{
	return cant_pokemones_iterados == cantidad_pokemones && strcmp(vector_pokemones[cantidad_pokemones - 1].nombre, nombre_a_buscar) != 0;
}

//*********** FUNCIONES PRINCIPALES **********

struct pokedex *pokedex_crear()
{
	struct pokedex *inicializar_pokedex = malloc(sizeof(struct pokedex));
	if (!inicializar_pokedex) {
		return NULL;
	}
	inicializar_pokedex->cantidad_pokemones = 0;
	inicializar_pokedex->capacidad = CAPACIDAD_INICAL_POKEDEX;
	inicializar_pokedex->vector_pokemones =
		malloc(CAPACIDAD_INICAL_POKEDEX * sizeof(struct pokemon));
	if (!inicializar_pokedex->vector_pokemones) {
		free(inicializar_pokedex);
		return NULL;
	}
	return inicializar_pokedex;
}

bool pokedex_agregar_pokemon(struct pokedex *pokedex, struct pokemon pokemon) // ====>
{ 
	if (!pokedex || !pokemon.nombre) {
		return false;
	}

	if (pokedex->cantidad_pokemones >= (pokedex->capacidad * 75 / 100)) {
		if (!redimensionar_vector_pokemones(pokedex)) { // K veces O(n)
			return false;
		}
	}

	char *nueva_ubicacion_nombre =
		malloc((strlen(pokemon.nombre) + 1) * sizeof(char));
	if (!nueva_ubicacion_nombre) {
		return false;
	}

	strcpy(nueva_ubicacion_nombre, pokemon.nombre); // O(c)
	pokemon.nombre = nueva_ubicacion_nombre;

	size_t cantidad_iterados = pokedex_iterar_pokemones(
		pokedex, buscar_posicion, pokemon.nombre); // O(n*c)

	if (cantidad_iterados == 0 ||
	    (es_el_ultimo_pokemon(cantidad_iterados,
				  pokedex->cantidad_pokemones) &&
	     nombre_pokemon_en_el_vector_es_menor(
		     pokedex->vector_pokemones[cantidad_iterados - 1].nombre,
		     pokemon.nombre))) {
		pokedex->vector_pokemones[cantidad_iterados] = pokemon;
	} else {
		for (size_t i = pokedex->cantidad_pokemones; // O(n)
		     i > cantidad_iterados - 1; i--) {
			pokedex->vector_pokemones[i] =
				pokedex->vector_pokemones[i - 1];
		}
		pokedex->vector_pokemones[cantidad_iterados - 1] = pokemon;
	}

	pokedex->cantidad_pokemones++;
	return true;
}

size_t pokedex_cantidad_pokemones(struct pokedex *pokedex) // =====> O(1)
{
	return (!pokedex || !pokedex->cantidad_pokemones) ?
		       0 :
		       pokedex->cantidad_pokemones;
}

const struct pokemon *pokedex_buscar_pokemon(struct pokedex *pokedex,
					     const char *nombre) // O(n*c)
{
	if (!pokedex || !nombre || !pokedex->vector_pokemones ||
	    pokedex->cantidad_pokemones == 0) {
		return NULL;
	}

	size_t cantidad_iterados = pokedex_iterar_pokemones(
		pokedex, buscar_pokemon, (void *)nombre);

	if (pokemon_no_encontrado(cantidad_iterados, pokedex->cantidad_pokemones, pokedex->vector_pokemones, nombre)) {
		return NULL; // O(c)
	}

	return &pokedex->vector_pokemones[cantidad_iterados - 1];
}

size_t pokedex_iterar_pokemones(struct pokedex *pokedex,
				bool (*funcion)(struct pokemon *, void *),
				void *ctx) // ===== >O(n)
{
	if (!pokedex || !pokedex->vector_pokemones || !funcion ||
	    pokedex->cantidad_pokemones == 0) {
		return 0;
	}

	for (size_t i = 0; i < pokedex->cantidad_pokemones; i++) { // n veces
		if (!(funcion(
			    &pokedex->vector_pokemones[i],
			    ctx))) { // una funcion puede hacer algo k veces con el pokemon
			return i + 1;
		}
	} // =====> O(n)

	return pokedex->cantidad_pokemones;
}

void pokedex_destruir(struct pokedex *pokedex) // ====> O(n)
{
	if (pokedex) {
		if (pokedex->vector_pokemones) { // n veces
			for (size_t i = 0; i < pokedex->cantidad_pokemones;
			     i++) {
				free(pokedex->vector_pokemones[i].nombre);
			}
			free(pokedex->vector_pokemones);
		} // =====> O(n)
		free(pokedex);
	}
}
