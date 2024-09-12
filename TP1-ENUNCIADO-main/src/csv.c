#include "csv.h"
#include <stdio.h>
#include <string.h>
#include "split.h"

const size_t CAPACIDAD_INICIAL_CSV = 12;
const size_t FACTOR_CRECIMIENTO_LINEA = 2;

struct archivo_csv {
	FILE *archivo;
	char separador;
};

bool redimencionar_linea_texto(char **texto, size_t *capacidad)
{
	*capacidad *= FACTOR_CRECIMIENTO_LINEA;
	char *nuevo_bloque_texto = realloc(*texto, (*capacidad) * sizeof(char));
	if (!nuevo_bloque_texto) {
		return false;
	}
	*texto = nuevo_bloque_texto;
	return true;
}

struct archivo_csv *abrir_archivo_csv(const char *nombre_archivo,
				      char separador)
{
	struct archivo_csv *inicializar_archivo =
		malloc(sizeof(struct archivo_csv));
	if (!inicializar_archivo) {
		return NULL;
	}

	FILE *archivo = fopen(nombre_archivo, "r");
	if (!archivo) {
		free(inicializar_archivo);
		return NULL;
	}

	inicializar_archivo->archivo = archivo;
	inicializar_archivo->separador = separador;
	return inicializar_archivo;
}

size_t leer_linea_csv(struct archivo_csv *archivo, size_t columnas,
		      bool (*funciones[])(const char *, void *), void *ctx[])
{
	if (!archivo || !archivo->archivo || !funciones || !ctx) {
		return 0;
	}

	size_t columna_posicion = 0;

	size_t capacidad_linea = CAPACIDAD_INICIAL_CSV;
	size_t tamaño_del_texto = 0;
	char *texto = malloc(capacidad_linea * sizeof(char));
	if (!texto) {
		return columna_posicion;
	}

	int valor_ascii;

	while ((valor_ascii = fgetc(archivo->archivo)) != EOF &&
	       valor_ascii != '\n') {
		if (tamaño_del_texto >= (capacidad_linea * 75) / 100) {
			if (!redimencionar_linea_texto(&texto,
						       &capacidad_linea)) {
				free(texto);
				return columna_posicion;
			}
		}
		texto[tamaño_del_texto++] = (char)valor_ascii;
	}

	if (tamaño_del_texto == 0 && valor_ascii == EOF) {
		free(texto);
		return columna_posicion;
	}

	texto[tamaño_del_texto] = '\0';
	struct Partes *partes = dividir_string(texto, archivo->separador);
	free(texto);

	if (!partes) {
		return columna_posicion;
	}

	if (partes->cantidad == 0 || partes->cantidad < columnas) {
		liberar_partes(partes);
		return columna_posicion;
	}

	while (columna_posicion < columnas) {
		if (!funciones[columna_posicion]) {
			liberar_partes(partes);
			return columna_posicion;
		}

		if (!funciones[columna_posicion](
			    partes->string[columna_posicion],
			    ctx[columna_posicion])) {
			liberar_partes(partes);
			return columna_posicion + 1;
		}
		columna_posicion++;
	}

	liberar_partes(partes);
	return columnas;
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
