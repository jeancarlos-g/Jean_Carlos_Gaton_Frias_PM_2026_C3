#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Duplica una cadena (memoria dinamica, liberar con g_free). */
char *dup_str(const char *s);

/* Formatea un monto como "$1,234.56" (memoria dinamica, liberar con g_free). */
char *fmt_money(double v);

/* Formatea un monto sin simbolo: "1,234.56". */
char *fmt_money_plain(double v);

/* Convierte texto a numero aceptando coma o punto decimal.
 * Devuelve 1 si pudo convertir todo el texto, 0 si no. */
int parse_double_str(const char *s, double *out);

/* Convierte texto a entero. Devuelve 1 si pudo. */
int parse_int_str(const char *s, int *out);

/* Busqueda de subcadena sin distinguir mayusculas/minusculas. */
int texto_contiene_ci(const char *texto, const char *aguja);

/* Valida una fecha con formato YYYY-MM-DD. */
int fecha_valida(const char *fecha);

#endif
