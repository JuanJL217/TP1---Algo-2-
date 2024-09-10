#include "csv.h"
#include <stdio.h>
#include <string.h>
#include "lectura_separador/split.c"

#define CANT_LETRAS 50

struct archivo_csv {
	FILE* archivo;
	char separador;
};

struct archivo_csv *abrir_archivo_csv(const char *nombre_archivo,
				      char separador)
{	
	FILE *archivo = fopen(nombre_archivo, "r");
	if (!archivo) {
		return NULL;
	}

	struct archivo_csv* archivo_csv = malloc(sizeof(struct archivo_csv));
	if (!archivo_csv) {
		fclose(archivo);
		return NULL;
	}

	archivo_csv->archivo = archivo;
	archivo_csv->separador = separador;
	return archivo_csv;
}

size_t leer_linea_csv(struct archivo_csv *archivo, size_t columnas,
		      bool (*funciones[])(const char *, void *), void *ctx[])
{	
	if (!archivo || !archivo->archivo || !funciones || !ctx) {
		return 0;
	}

	size_t columna_posicion = 0;
	char* texto = malloc(CANT_LETRAS * sizeof(char));

	if (!fgets(texto, CANT_LETRAS, archivo->archivo)){
		free(texto);
		return columna_posicion;
	}

	struct Partes* partes = dividir_string(texto, archivo->separador);
	if (!partes) {
		free(texto);
		return columna_posicion;
	}

	free(texto);

	while (columna_posicion < columnas) {
		if (!funciones[columna_posicion](partes->string[columna_posicion], ctx[columna_posicion])){
			break;
		}
		columna_posicion++;
	}
	liberar_partes(partes);
	return columna_posicion;
}

void cerrar_archivo_csv(struct archivo_csv *archivo)
{
	if (archivo) {
		if (archivo->archivo) {
			fclose(archivo->archivo);
		}
		free(archivo);
	}
}
