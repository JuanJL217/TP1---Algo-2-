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

bool redimensionar_vector_pokemones(struct pokedex *pokedex)
{
	size_t nueva_capacidad =
		pokedex->capacidad * FACTOR_CRECIMIENTO_POKEDEX;
	struct pokemon *nuevo_bloque =
		realloc(pokedex->vector_pokemones,
			nueva_capacidad * sizeof(struct pokemon));
	if (!nuevo_bloque) {
		return false;
	}
	pokedex->vector_pokemones = nuevo_bloque;
	pokedex->capacidad = nueva_capacidad;
	return true;
}

bool buscar_posicion(struct pokemon *pokemon_actual, void *nombre_pokemon)
{
	return !(strcmp(pokemon_actual->nombre, (char *)nombre_pokemon) > 0);
}

bool buscar_pokemon(struct pokemon *pokemon_actual,
		    void *nombre_pokemon_a_buscar)
{
	return !(strcmp(pokemon_actual->nombre,
			(const char *)nombre_pokemon_a_buscar) == 0);
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

bool pokedex_agregar_pokemon(struct pokedex *pokedex, struct pokemon pokemon)
{
	if (!pokedex || !pokemon.nombre) {
		return false;
	}

	if (pokedex->cantidad_pokemones >= (pokedex->capacidad * 75 / 100)) {
		if (!redimensionar_vector_pokemones(pokedex)) {
			return false;
		}
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

	if (pokedex->cantidad_pokemones == 0) {
		pokedex->vector_pokemones[posicion_del_pokemon] = pokemon;
		pokedex->cantidad_pokemones++;
		return true;
	}

	if (posicion_del_pokemon == pokedex->cantidad_pokemones) {
		if (strcmp(pokedex->vector_pokemones[posicion_del_pokemon - 1]
				   .nombre,
			   pokemon.nombre) < 0) {
			pokedex->vector_pokemones[posicion_del_pokemon] =
				pokemon;
		} else {
			pokedex->vector_pokemones[posicion_del_pokemon] =
				pokedex->vector_pokemones[posicion_del_pokemon -
							  1];
			pokedex->vector_pokemones[posicion_del_pokemon - 1] =
				pokemon;
		}
	} else {
		if (posicion_del_pokemon == 1) {
			for (size_t i = pokedex->cantidad_pokemones; i > 0;
			     i--) {
				pokedex->vector_pokemones[i] =
					pokedex->vector_pokemones[i - 1];
			}
			pokedex->vector_pokemones[0] = pokemon;
		} else {
			for (size_t i = pokedex->cantidad_pokemones;
			     i >= posicion_del_pokemon; i--) {
				pokedex->vector_pokemones[i] =
					pokedex->vector_pokemones[i - 1];
			}
			pokedex->vector_pokemones[posicion_del_pokemon - 1] =
				pokemon;
		}
	}

	pokedex->cantidad_pokemones++;
	return true;
}

size_t pokedex_cantidad_pokemones(struct pokedex *pokedex)
{
	return (!pokedex || !pokedex->cantidad_pokemones) ?
		       0 :
		       pokedex->cantidad_pokemones;
}

const struct pokemon *pokedex_buscar_pokemon(struct pokedex *pokedex,
					     const char *nombre)
{
	if (!pokedex || !nombre || !pokedex->vector_pokemones ||
	    pokedex->cantidad_pokemones == 0) {
		return NULL;
	}

	size_t posicion_del_pokemon = pokedex_iterar_pokemones(
		pokedex, buscar_pokemon, (void *)nombre);

	if (posicion_del_pokemon == pokedex->cantidad_pokemones &&
	    strcmp(pokedex->vector_pokemones[posicion_del_pokemon - 1].nombre,
		   nombre) != 0) {
		return NULL;
	}

	return &pokedex->vector_pokemones[posicion_del_pokemon - 1];
}

size_t pokedex_iterar_pokemones(struct pokedex *pokedex,
				bool (*funcion)(struct pokemon *, void *),
				void *ctx)
{
	if (!pokedex || !pokedex->vector_pokemones || !funcion ||
	    pokedex->cantidad_pokemones == 0) {
		return 0;
	}

	for (size_t i = 0; i < pokedex->cantidad_pokemones; i++) {
		if (!(funcion(&pokedex->vector_pokemones[i], ctx))) {
			return i + 1;
		}
	}

	return pokedex->cantidad_pokemones;
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
