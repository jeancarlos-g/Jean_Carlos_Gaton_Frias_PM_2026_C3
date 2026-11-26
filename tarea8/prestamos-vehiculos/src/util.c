#include "util.h"

#include <glib.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *dup_str(const char *s)
{
    return g_strdup(s ? s : "");
}

static char *formatear(double v, int con_simbolo)
{
    int negativo = v < 0.0;
    double absoluto = fabs(v);
    long long centavos = (long long)llround(absoluto * 100.0);
    long long enteros = centavos / 100;
    long long fraccion = centavos % 100;
    char crudo[32];
    GString *salida = g_string_new(NULL);

    g_snprintf(crudo, sizeof crudo, "%lld", enteros);

    if (negativo)
        g_string_append_c(salida, '-');
    if (con_simbolo)
        g_string_append_c(salida, '$');

    size_t largo = strlen(crudo);
    for (size_t i = 0; i < largo; i++) {
        if (i > 0 && (largo - i) % 3 == 0)
            g_string_append_c(salida, ',');
        g_string_append_c(salida, crudo[i]);
    }
    g_string_append_printf(salida, ".%02lld", fraccion);

    return g_string_free(salida, FALSE);
}

char *fmt_money(double v)
{
    return formatear(v, 1);
}

char *fmt_money_plain(double v)
{
    return formatear(v, 0);
}

int parse_double_str(const char *s, double *out)
{
    if (!s || !out)
        return 0;

    char *tmp = g_strdup(s);
    g_strstrip(tmp);
    if (!*tmp) {
        g_free(tmp);
        return 0;
    }

    /* Quitar separadores de miles. Si aparecen coma y punto, el que está
     * más a la derecha es el separador decimal. */
    char *punto = strrchr(tmp, '.');
    char *coma = strrchr(tmp, ',');
    if (punto && coma) {
        int coma_decimal = coma > punto;
        char *src = tmp, *dst = tmp;
        while (*src) {
            if (coma_decimal) {
                if (*src == '.') {
                    src++;
                    continue;
                }
                *dst++ = (*src == ',') ? '.' : *src;
            } else {
                if (*src == ',') {
                    src++;
                    continue;
                }
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0';
    } else if (coma) {
        for (char *p = tmp; *p; p++)
            if (*p == ',')
                *p = '.';
    }

    char *fin = NULL;
    double valor = g_ascii_strtod(tmp, &fin);
    if (fin == tmp) {
        g_free(tmp);
        return 0;
    }
    while (*fin && isspace((unsigned char)*fin))
        fin++;
    if (*fin || !isfinite(valor)) {
        g_free(tmp);
        return 0;
    }

    *out = valor;
    g_free(tmp);
    return 1;
}

int parse_int_str(const char *s, int *out)
{
    double v;
    if (!parse_double_str(s, &v))
        return 0;
    if (v != floor(v) || v < -2147483647.0 || v > 2147483647.0)
        return 0;
    *out = (int)v;
    return 1;
}

int texto_contiene_ci(const char *texto, const char *aguja)
{
    if (!texto || !aguja)
        return 0;
    if (!*aguja)
        return 1;

    size_t n = strlen(aguja);
    for (const char *p = texto; *p; p++) {
        size_t i = 0;
        while (i < n && p[i] &&
               tolower((unsigned char)p[i]) == tolower((unsigned char)aguja[i]))
            i++;
        if (i == n)
            return 1;
    }
    return 0;
}

int fecha_valida(const char *fecha)
{
    if (!fecha || strlen(fecha) != 10)
        return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (fecha[i] != '-')
                return 0;
        } else if (!isdigit((unsigned char)fecha[i])) {
            return 0;
        }
    }
    int anio = atoi(fecha);
    int mes = atoi(fecha + 5);
    int dia = atoi(fecha + 8);
    if (anio < 1900 || anio > 2200)
        return 0;
    if (mes < 1 || mes > 12)
        return 0;
    if (dia < 1 || dia > 31)
        return 0;
    return 1;
}
