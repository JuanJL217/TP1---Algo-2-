#include "split.h"
#include <stdlib.h>
#include <string.h>

const char FINAL_DEL_STRING = '\0';

void agregar_partes(struct Partes *resultado, const char *inicio_string,
		    const char *string, size_t posicion) // ====> O(c)
{
	size_t longitud = (size_t)string - (size_t)inicio_string;
	resultado->string[posicion] = malloc((longitud + 1) * sizeof(char));
	if (!resultado->string[posicion]) {
		return;
	}
	strncpy(resultado->string[posicion], inicio_string, longitud); // O(c)
	resultado->string[posicion][longitud] = FINAL_DEL_STRING;
}

size_t cantidad_de_partes(const char *string, char separador) // =====> O(c)
{
	size_t cantidad = 1;
	while (*string != FINAL_DEL_STRING) { // O(c)
		if (*string == separador) {
			cantidad++;
		}
		string++;
	}
	return cantidad;
}

struct Partes *dividir_string(const char *string, char separador) // =====> O(c²)
{
	struct Partes *resultado = malloc(sizeof(struct Partes));
	if (!resultado) {
		return NULL;
	}
	if (!string) {
		resultado->cantidad = 0;
		resultado->string = NULL;
		return resultado;
	}
	resultado->cantidad = cantidad_de_partes(string, separador); // O(c)
	resultado->string = malloc(resultado->cantidad * sizeof(char *));
	if (!resultado->string) {
		free(resultado);
		return NULL;
	}

	size_t posicion = 0;
	const char *inicio_string = string;
	while (*string != FINAL_DEL_STRING) { // c veces
		if (*string == separador) {
			agregar_partes(resultado, inicio_string, string,
				       posicion); // O(c)
			if (!resultado->string[posicion]) {
				liberar_partes(resultado);
				return NULL;
			}
			posicion++;
			inicio_string = string + 1;
		}
		string++;
	} // ====> O(c²)

	agregar_partes(resultado, inicio_string, string, posicion); // O(c)
	if (!resultado->string[posicion]) {
		liberar_partes(resultado);
		return NULL;
	}
	if (posicion + 1 != resultado->cantidad) {
		liberar_partes(resultado);
		return NULL;
	}

	return resultado;
}

void liberar_partes(struct Partes *partes) // =====> O(m)
{	
	if (partes) {
		for (int i = 0; i < partes->cantidad; ++i) { // O(m)
			free(partes->string[i]);
		}
		free(partes->string);
		free(partes);
	}
}
