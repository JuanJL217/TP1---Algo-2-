#include "src/csv.h"
#include "src/pokedex.h"
#include <stdio.h>
#include <string.h>

#define CANT_TIPOS 7
const size_t CANT_COLUMNAS = 5;

// ---- Structura para llevar contabilidad ------
// ----------- de los tipos de pokemon ----------

struct tipos_pokemon {
	const char* tipos[CANT_TIPOS];
	const char tipos_caracteres[CANT_TIPOS];
	size_t vector_tipos_cantidades[CANT_TIPOS];
};

// ----- Funciones para parsear datos de la lectura -------

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

// ------- Funcion para usar la iteracion de la pokedex --------

bool mostrar_pokemones_y_contar_tipos(struct pokemon *pokemon, void *ctx)
{	
	struct tipos_pokemon* tipos_totales = (struct tipos_pokemon*) ctx;

	for (int i = 0; i < CANT_TIPOS; i++) {
		if (pokemon->tipo == tipos_totales->tipos_caracteres[i]) {
			tipos_totales->vector_tipos_cantidades[i]++;
			break;
		}
	}
	printf("NOMBRE: %s, TIPO: %c, FUERZA: %d, DESTREZA: %d, RESISTENCIA: %d\n",
	       pokemon->nombre, pokemon->tipo, pokemon->fuerza,
	       pokemon->destreza, pokemon->resistencia);
	return true;
}

// ------- Funciones extras -------

struct tipos_pokemon inicializar_tipos() {
    struct tipos_pokemon tipos_totales = {
        .tipos = { "Agua", "Fuego", "Planta", "Roca", "Electrico", "Normal", "Lucha" },
        .tipos_caracteres = { TIPO_AGUA, TIPO_FUEGO, TIPO_PLANTA, TIPO_ROCA, TIPO_ELECTRICO, TIPO_NORMAL, TIPO_LUCHA },
        .vector_tipos_cantidades = { 0, 0, 0, 0, 0, 0, 0 },
    };
    return tipos_totales;
}

void procesar_informacion_pokemon(struct pokemon* pokemon, char* nombre, char tipo, int fuerza, int destreza, int resistencia) {
		strcpy(pokemon->nombre, nombre);
		pokemon->tipo = tipo;
		pokemon->fuerza = fuerza;
		pokemon->destreza = destreza;
		pokemon->resistencia = resistencia;
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

	bool (*funciones[])(const char *,
			     void *) = { agregar_nombre, agregar_tipo,
					 agregar_numero, agregar_numero,
					 agregar_numero };
	
	char *nombre = NULL;
	char tipo;
	int fuerza;
	int destreza;
	int resistencia;
	void *punteros[] = { &nombre, &tipo, &fuerza, &destreza,
					  &resistencia };
	struct pokemon pokemon;
	while (leer_linea_csv(archivo, CANT_COLUMNAS, funciones, punteros) ==
	       CANT_COLUMNAS) {
		pokemon.nombre = malloc((strlen(nombre) + 1) * sizeof(char));
		if (!pokemon.nombre) {
			printf("Pokemon %s no pudo ser asignado a la pokedex, problema con asignar memoria",
			       nombre);
		} else {
			procesar_informacion_pokemon(&pokemon, nombre, tipo, fuerza, destreza, resistencia);
			pokedex_agregar_pokemon(pokedex, pokemon);
			free(pokemon.nombre);
			free(nombre);
		}
	};

	struct tipos_pokemon tipos_totales = inicializar_tipos();
	
	pokedex_iterar_pokemones(pokedex, mostrar_pokemones_y_contar_tipos, &tipos_totales);

	printf("Cantidad de pokemones de cada tipo:\n");

	for (int i = 0; i < CANT_TIPOS; i++) {
		printf("%s: %ld\n", tipos_totales.tipos[i], tipos_totales.vector_tipos_cantidades[i]);
	}

	finalizar_programa(pokedex, archivo);
	return 0;
}
