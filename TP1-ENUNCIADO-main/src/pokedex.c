#include "pokedex.h"
#include <string.h>

const size_t CAPACIDAD_INICAL = 6;
const size_t FACTOR_CRECIMIENTO = 2;

struct pokedex {
	struct pokemon *vector_pokemones;
	size_t cantidad_pokemones;
	size_t capacidad;
};

//********* FUNCIONES EXRAS ***********

bool redimensionar_vector_pokemones(struct pokedex *pokedex)
{
	size_t nueva_capacidad = pokedex->capacidad * FACTOR_CRECIMIENTO;
	struct pokemon *nuevo_bloque =
		realloc(pokedex->vector_pokemones,
			nueva_capacidad * sizeof(struct pokemon));
	if (!nuevo_bloque) {
		return false;
	}
	pokedex->vector_pokemones = nuevo_bloque;
	return true;
}

bool buscar_posicion(struct pokemon *pokemon_actual, void *nombre_pokemon)
{
	return strcmp(pokemon_actual->nombre, (char *)nombre_pokemon) < 0;
}

bool buscar_pokemon(struct pokemon *pokemon_actual,
		    void *nombre_pokemon_a_buscar)
{
	return strcmp(pokemon_actual->nombre,
		      (const char *)nombre_pokemon_a_buscar) != 0;
}

//*********** FUNCIONES PRINCIPALES **********

struct pokedex *pokedex_crear()
{
	struct pokedex *inicializar_pokedex = malloc(sizeof(struct pokedex));
	if (!inicializar_pokedex) {
		return NULL;
	}
	inicializar_pokedex->cantidad_pokemones = 0;
	inicializar_pokedex->capacidad = CAPACIDAD_INICAL;
	inicializar_pokedex->vector_pokemones = malloc(CAPACIDAD_INICAL * sizeof(struct pokemon));
	if (!inicializar_pokedex->vector_pokemones) {
		free(inicializar_pokedex);
		return NULL;
	}
	return inicializar_pokedex;
}

bool pokedex_agregar_pokemon(struct pokedex *pokedex, struct pokemon pokemon)
{
	if (!pokedex || !pokemon.nombre) {
		return false;
	}
	
	char *nueva_ubicacion_nombre =
		malloc((strlen(pokemon.nombre) + 1) * sizeof(char));
	if (!nueva_ubicacion_nombre) {
		return false;
	}
	
	strcpy(nueva_ubicacion_nombre, pokemon.nombre);
	pokemon.nombre = nueva_ubicacion_nombre;

	size_t posicion_del_pokemon = pokedex_iterar_pokemones(
		pokedex, buscar_posicion, pokemon.nombre);

	if (posicion_del_pokemon < pokedex->cantidad_pokemones &&
	    strcmp(pokemon.nombre,
		   pokedex->vector_pokemones[posicion_del_pokemon].nombre) ==
		    0) {
		free(pokedex->vector_pokemones[posicion_del_pokemon].nombre);
	} else {
		if (pokedex->cantidad_pokemones >= (pokedex->capacidad * 75 / 100)) {
			if(!redimensionar_vector_pokemones(pokedex)) {
				return false;
			}
		}
		for (size_t i = pokedex->cantidad_pokemones;
		     i > posicion_del_pokemon; i--) {
			pokedex->vector_pokemones[i] =
				pokedex->vector_pokemones[i - 1];
		}
		pokedex->cantidad_pokemones++;
	}

	pokedex->vector_pokemones[posicion_del_pokemon] = pokemon;
	return true;
}

size_t pokedex_cantidad_pokemones(struct pokedex *pokedex)
{
	return pokedex->cantidad_pokemones;
}

const struct pokemon *pokedex_buscar_pokemon(struct pokedex *pokedex,
					     const char *nombre)
{
	if (!pokedex || !nombre || !pokedex->vector_pokemones) {
		return NULL;
	}

	size_t posicion_del_pokemon = pokedex_iterar_pokemones(
		pokedex, buscar_pokemon, (void *)nombre);

	if (posicion_del_pokemon == pokedex->cantidad_pokemones) {
		return NULL;
	}

	return &pokedex->vector_pokemones[posicion_del_pokemon];
}

size_t pokedex_iterar_pokemones(struct pokedex *pokedex,
				bool (*funcion)(struct pokemon *, void *),
				void *ctx)
{
	size_t iteracion = 0;
	if (!pokedex || !pokedex->vector_pokemones) {
		return iteracion;
	}
	while (iteracion < pokedex->cantidad_pokemones) {
		if (!(funcion(&pokedex->vector_pokemones[iteracion], ctx))) {
			return iteracion;
		}
		iteracion++;
	}
	return iteracion;
}

void pokedex_destruir(struct pokedex *pokedex)
{
	if (pokedex) {
		if (pokedex->vector_pokemones) {
			for (size_t i = 0; i < pokedex->cantidad_pokemones;
			     i++) {
				free(pokedex->vector_pokemones[i].nombre);
			}
			free(pokedex->vector_pokemones);
		}
		free(pokedex);
	}
}
