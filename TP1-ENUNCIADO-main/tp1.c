#include "src/csv.h"
#include "src/pokedex.h"
#include <stdio.h>
#include <string.h>

#define CANT_TIPOS 7
#define CANT_COLUMNAS 5

bool agregar_nombre(const char *str, void *ctx)
{
	char *nuevo = malloc(strlen(str) + 1);
	if (nuevo == NULL)
		return false;
	strcpy(nuevo, str);
	*(char **)ctx = nuevo;
	return true;
}

bool agregar_tipo(const char *str, void *ctx)
{
	*(char *)ctx = *(char *)str;
	return true;
}

bool agregar_numero(const char *str, void *ctx)
{
	return sscanf(str, "%d", (int *)ctx) == 1;
}

bool mostrar_pokemons(struct pokemon *pokemon, void *ctx)
{
	printf("NOMBRE: %s, TIPO: %c, FUERZA: %d, DESTREZA: %d, RESISTENCIA: %d\n",
	       pokemon->nombre, pokemon->tipo, pokemon->fuerza,
	       pokemon->destreza, pokemon->resistencia);
	return true;
}

bool contar_tipos(struct pokemon *pokemon, void *ctx)
{
	int *contador_tipos = (int *)ctx;
	const char tipos_pokemon_caracter[CANT_TIPOS] = {
		TIPO_AGUA,	TIPO_FUEGO,  TIPO_PLANTA, TIPO_ROCA,
		TIPO_ELECTRICO, TIPO_NORMAL, TIPO_LUCHA
	};
	for (int i = 0; i < CANT_TIPOS; i++) {
		if (pokemon->tipo == tipos_pokemon_caracter[i]) {
			contador_tipos[i]++;
			break;
		}
	}
	return true;
}

void finalizar_programa(struct pokedex *pokedex, struct archivo_csv *archivo)
{
	pokedex_destruir(pokedex);
	cerrar_archivo_csv(archivo);
}

int main(int argc, char const *argv[])
{
	if (argc < 2) {
		perror("No hay archivo");
		return -1;
	}

	struct archivo_csv *archivo = abrir_archivo_csv(argv[1], ';');
	if (!archivo) {
		perror("No se pudo inicializar el archivo");
		return -2;
	}

	struct pokedex *pokedex = pokedex_crear();
	if (!pokedex) {
		cerrar_archivo_csv(archivo);
		perror("No se pudo inicializar la pokedex");
		return -3;
	}

	bool (*funciones[5])(const char *,
			     void *) = { agregar_nombre, agregar_tipo,
					 agregar_numero, agregar_numero,
					 agregar_numero };
	
	char *nombre = NULL;
	char tipo;
	int fuerza;
	int destreza;
	int resistencia;
	void *punteros[CANT_COLUMNAS] = { &nombre, &tipo, &fuerza, &destreza,
					  &resistencia };

	while (leer_linea_csv(archivo, CANT_COLUMNAS, funciones, punteros) ==
	       CANT_COLUMNAS) {
		struct pokemon pokemon;
		pokemon.nombre = malloc((strlen(nombre) + 1) * sizeof(char));
		if (!pokemon.nombre) {
			printf("Pokemon %s no pudo ser asignado a la pokedex, problema con asignar memoria",
			       nombre);
		} else {
			strcpy(pokemon.nombre, nombre);
			pokemon.tipo = tipo;
			pokemon.fuerza = fuerza;
			pokemon.destreza = destreza;
			pokemon.resistencia = resistencia;
			pokedex_agregar_pokemon(pokedex, pokemon);
			free(pokemon.nombre);
			free(nombre);
		}
	};

	const char *tipos_pokemon[CANT_TIPOS] = { "Agua",      "Fuego",
						  "Planta",    "Rroca",
						  "Electrico", "Normal",
						  "Lucha" };
	int contador_tipos[CANT_TIPOS] = { 0, 0, 0, 0, 0, 0, 0 };

	pokedex_iterar_pokemones(pokedex, mostrar_pokemons, NULL);
	pokedex_iterar_pokemones(pokedex, contar_tipos, contador_tipos);

	printf("Cantidad de pokemones de cada tipo:\n");

	for (int i = 0; i < CANT_TIPOS; i++) {
		printf("%s: %d\n", tipos_pokemon[i], contador_tipos[i]);
	}

	finalizar_programa(pokedex, archivo);
	return 0;
}
