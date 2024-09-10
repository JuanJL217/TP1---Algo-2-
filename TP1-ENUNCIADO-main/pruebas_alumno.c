#include "pa2m.h"
#include "src/csv.h"
#include "src/pokedex.h"
#include "string.h"

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
	printf("Se mota en consideración la primera columna para ver si hay errores al toman n columnas de m\n");
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
				free(nombres[posicion]);
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
			printf("%s\n", nombres[i]);
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
	pa2m_afirmar(pokedex_agregar_pokemon(NULL, pokemon) == false,
		     "Parametro Nulo pokedex, retorna falso");
	pokemon.nombre = NULL;
	pa2m_afirmar(
		pokedex_agregar_pokemon(pokedex, pokemon) == false,
		"Ingresar un pokemon con nombre de direccion Nulo, es falso");
	pokedex_destruir(pokedex);
}

void agregarUnPokemon()
{
	struct pokedex *pokedex = pokedex_crear();
	struct pokemon pokemon;
	char *nombre_pokemon = "Pikachu";
	pokemon.nombre = malloc(8 * sizeof(char));
	if (!pokemon.nombre) {
		pokedex_destruir(pokedex);
		return;
	}
	strcpy(pokemon.nombre, nombre_pokemon);
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
	for (int i = 0; i < 4; i++) {
		pokemon.nombre =
			malloc(strlen(pokemones[i]) + 1 * sizeof(char));
		if (!pokemon.nombre) {
			pokedex_destruir(pokedex);
			return;
		}
		strcpy(pokemon.nombre, pokemones[i]);
		if (pokedex_agregar_pokemon(pokedex, pokemon)) {
			printf("Se agregó el pokemon numero %d\n", i + 1);
		}
		free(pokemon.nombre);
	}
	pa2m_afirmar(pokedex_cantidad_pokemones(pokedex) == 4,
		     "Se agregaron exitosamente 4 pokemones");
	pokedex_destruir(pokedex);
}

int main()
{
	pa2m_nuevo_grupo("Pruebas de archivos CSV");
	abrirUnArchivoInexistenteDebeRetornarNull();
	abrirArchivoExistente();
	pa2m_nuevo_grupo("Pruebas de lectura en archivos CSV");
	//pa2m_nuevo_grupo("Pruebas de lectura de archivo con parametros NULL");
	parametrosNulosRetornanCeroEnLecturaDeLineas();
	pa2m_nuevo_grupo("Pruebas de lectura en archivos CSV");
	abrirArchivoYParsearLosNombres();
	pa2m_nuevo_grupo("Pruebas de Pokedex");
	crearUnaPokedexValoresIniciales();
	pa2m_nuevo_grupo("Pruebas agregar Pokemones con parametros NULL");
	agregarValoresNulosPokedex();
	pa2m_nuevo_grupo("Prueba se agrega un pokemon a la pokedex");
	agregarUnPokemon();
	pa2m_nuevo_grupo("Prueba se agregan varios pokemones");
	agregarVariosPokemones();
	pa2m_nuevo_grupo("Prueba con buscador de pokemon");
	// Datos NULL
	// Se busca pokemon inexistente
	// Se busca pokemon existente
	pa2m_nuevo_grupo("Pruebas con el iterador");

	return pa2m_mostrar_reporte();
}
