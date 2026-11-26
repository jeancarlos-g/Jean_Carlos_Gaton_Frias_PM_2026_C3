#include "json.h"

#include <glib.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Construccion                                                        */
/* ------------------------------------------------------------------ */

static JsonValue *json_new(JsonType tipo)
{
    JsonValue *v = g_new0(JsonValue, 1);
    v->type = tipo;
    return v;
}

JsonValue *json_null(void)
{
    return json_new(JSON_NULL);
}

JsonValue *json_new_bool(int valor)
{
    JsonValue *v = json_new(JSON_BOOL);
    v->boolean = valor ? 1 : 0;
    return v;
}

JsonValue *json_new_number(double valor)
{
    JsonValue *v = json_new(JSON_NUMBER);
    v->number = valor;
    return v;
}

JsonValue *json_new_string(const char *valor)
{
    JsonValue *v = json_new(JSON_STRING);
    v->string = g_strdup(valor ? valor : "");
    return v;
}

JsonValue *json_new_array(void)
{
    return json_new(JSON_ARRAY);
}

JsonValue *json_new_object(void)
{
    return json_new(JSON_OBJECT);
}

void json_free(JsonValue *v)
{
    if (!v)
        return;
    g_free(v->string);
    for (int i = 0; i < v->count; i++) {
        if (v->keys)
            g_free(v->keys[i]);
        json_free(v->items[i]);
    }
    g_free(v->items);
    g_free(v->keys);
    g_free(v);
}

static void reservar(JsonValue *v, int necesario)
{
    if (v->capacity >= necesario)
        return;
    int capacidad = v->capacity ? v->capacity * 2 : 8;
    while (capacidad < necesario)
        capacidad *= 2;
    v->items = g_renew(JsonValue *, v->items, capacidad);
    if (v->type == JSON_OBJECT)
        v->keys = g_renew(char *, v->keys, capacidad);
    v->capacity = capacidad;
}

void json_array_add(JsonValue *arreglo, JsonValue *valor)
{
    if (!arreglo || arreglo->type != JSON_ARRAY)
        return;
    reservar(arreglo, arreglo->count + 1);
    arreglo->items[arreglo->count++] = valor;
}

void json_object_set(JsonValue *objeto, const char *clave, JsonValue *valor)
{
    if (!objeto || objeto->type != JSON_OBJECT)
        return;

    for (int i = 0; i < objeto->count; i++) {
        if (strcmp(objeto->keys[i], clave) == 0) {
            json_free(objeto->items[i]);
            objeto->items[i] = valor;
            return;
        }
    }

    reservar(objeto, objeto->count + 1);
    objeto->keys[objeto->count] = g_strdup(clave);
    objeto->items[objeto->count] = valor;
    objeto->count++;
}

JsonValue *json_object_get(const JsonValue *objeto, const char *clave)
{
    if (!objeto || objeto->type != JSON_OBJECT)
        return NULL;
    for (int i = 0; i < objeto->count; i++) {
        if (strcmp(objeto->keys[i], clave) == 0)
            return objeto->items[i];
    }
    return NULL;
}

const char *json_get_string(const JsonValue *objeto, const char *clave,
                            const char *por_defecto)
{
    JsonValue *v = json_object_get(objeto, clave);
    if (v && v->type == JSON_STRING)
        return v->string;
    return por_defecto;
}

double json_get_number(const JsonValue *objeto, const char *clave,
                       double por_defecto)
{
    JsonValue *v = json_object_get(objeto, clave);
    if (v && v->type == JSON_NUMBER)
        return v->number;
    return por_defecto;
}

int json_get_int(const JsonValue *objeto, const char *clave, int por_defecto)
{
    double d = json_get_number(objeto, clave, (double)por_defecto);
    return (int)d;
}

int json_get_bool(const JsonValue *objeto, const char *clave, int por_defecto)
{
    JsonValue *v = json_object_get(objeto, clave);
    if (v && v->type == JSON_BOOL)
        return v->boolean;
    return por_defecto;
}

/* ------------------------------------------------------------------ */
/* Serializacion                                                       */
/* ------------------------------------------------------------------ */

static void escribir_cadena(GString *salida, const char *texto)
{
    g_string_append_c(salida, '"');
    for (const unsigned char *p = (const unsigned char *)texto; *p; p++) {
        switch (*p) {
        case '"':
            g_string_append(salida, "\\\"");
            break;
        case '\\':
            g_string_append(salida, "\\\\");
            break;
        case '\b':
            g_string_append(salida, "\\b");
            break;
        case '\f':
            g_string_append(salida, "\\f");
            break;
        case '\n':
            g_string_append(salida, "\\n");
            break;
        case '\r':
            g_string_append(salida, "\\r");
            break;
        case '\t':
            g_string_append(salida, "\\t");
            break;
        default:
            if (*p < 0x20)
                g_string_append_printf(salida, "\\u%04x", *p);
            else
                g_string_append_c(salida, (char)*p);
            break;
        }
    }
    g_string_append_c(salida, '"');
}

static void escribir_numero(GString *salida, double valor)
{
    char buffer[G_ASCII_DTOSTR_BUF_SIZE];
    if (!isfinite(valor)) {
        g_string_append(salida, "0");
        return;
    }
    if (valor == floor(valor) && fabs(valor) < 1e15)
        g_snprintf(buffer, sizeof buffer, "%.0f", valor);
    else
        g_ascii_formatd(buffer, sizeof buffer, "%.15g", valor);
    g_string_append(salida, buffer);
}

static void escribir_valor(GString *salida, const JsonValue *v, int pretty,
                           int nivel)
{
    if (!v) {
        g_string_append(salida, "null");
        return;
    }

    switch (v->type) {
    case JSON_NULL:
        g_string_append(salida, "null");
        break;
    case JSON_BOOL:
        g_string_append(salida, v->boolean ? "true" : "false");
        break;
    case JSON_NUMBER:
        escribir_numero(salida, v->number);
        break;
    case JSON_STRING:
        escribir_cadena(salida, v->string ? v->string : "");
        break;
    case JSON_ARRAY:
        if (v->count == 0) {
            g_string_append(salida, "[]");
            break;
        }
        g_string_append_c(salida, '[');
        for (int i = 0; i < v->count; i++) {
            if (i)
                g_string_append_c(salida, ',');
            if (pretty) {
                g_string_append_c(salida, '\n');
                g_string_append_printf(salida, "%*s", (nivel + 1) * 2, "");
            }
            escribir_valor(salida, v->items[i], pretty, nivel + 1);
        }
        if (pretty) {
            g_string_append_c(salida, '\n');
            g_string_append_printf(salida, "%*s", nivel * 2, "");
        }
        g_string_append_c(salida, ']');
        break;
    case JSON_OBJECT:
        if (v->count == 0) {
            g_string_append(salida, "{}");
            break;
        }
        g_string_append_c(salida, '{');
        for (int i = 0; i < v->count; i++) {
            if (i)
                g_string_append_c(salida, ',');
            if (pretty) {
                g_string_append_c(salida, '\n');
                g_string_append_printf(salida, "%*s", (nivel + 1) * 2, "");
            }
            escribir_cadena(salida, v->keys[i]);
            g_string_append_c(salida, ':');
            if (pretty)
                g_string_append_c(salida, ' ');
            escribir_valor(salida, v->items[i], pretty, nivel + 1);
        }
        if (pretty) {
            g_string_append_c(salida, '\n');
            g_string_append_printf(salida, "%*s", nivel * 2, "");
        }
        g_string_append_c(salida, '}');
        break;
    }
}

char *json_serialize(const JsonValue *v, int pretty)
{
    GString *salida = g_string_new(NULL);
    escribir_valor(salida, v, pretty, 0);
    return g_string_free(salida, FALSE);
}

/* ------------------------------------------------------------------ */
/* Analisis                                                            */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *inicio;
    const char *p;
    GString *error;
    int profundidad;
} Parser;

static void error_parser(Parser *ps, const char *formato, ...)
{
    if (ps->error->len > 0)
        return;

    va_list ap;
    va_start(ap, formato);
    g_string_append_printf(ps->error, "posicion %ld: ",
                           (long)(ps->p - ps->inicio));
    g_string_append_vprintf(ps->error, formato, ap);
    va_end(ap);
}

static void saltar_espacios(Parser *ps)
{
    while (*ps->p == ' ' || *ps->p == '\t' || *ps->p == '\n' || *ps->p == '\r')
        ps->p++;
}

static JsonValue *parsear_valor(Parser *ps);

static void agregar_utf8(GString *s, unsigned int cp)
{
    if (cp <= 0x7F) {
        g_string_append_c(s, (char)cp);
    } else if (cp <= 0x7FF) {
        g_string_append_c(s, (char)(0xC0 | (cp >> 6)));
        g_string_append_c(s, (char)(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        g_string_append_c(s, (char)(0xE0 | (cp >> 12)));
        g_string_append_c(s, (char)(0x80 | ((cp >> 6) & 0x3F)));
        g_string_append_c(s, (char)(0x80 | (cp & 0x3F)));
    } else {
        g_string_append_c(s, (char)(0xF0 | (cp >> 18)));
        g_string_append_c(s, (char)(0x80 | ((cp >> 12) & 0x3F)));
        g_string_append_c(s, (char)(0x80 | ((cp >> 6) & 0x3F)));
        g_string_append_c(s, (char)(0x80 | (cp & 0x3F)));
    }
}

static int leer_hex4(Parser *ps, unsigned int *salida)
{
    unsigned int valor = 0;
    for (int i = 0; i < 4; i++) {
        char c = ps->p[i];
        unsigned int d;
        if (c >= '0' && c <= '9')
            d = (unsigned int)(c - '0');
        else if (c >= 'a' && c <= 'f')
            d = (unsigned int)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            d = (unsigned int)(c - 'A' + 10);
        else
            return 0;
        valor = valor * 16 + d;
    }
    ps->p += 4;
    *salida = valor;
    return 1;
}

static JsonValue *parsear_cadena(Parser *ps)
{
    ps->p++; /* comilla inicial */
    GString *s = g_string_new(NULL);

    while (*ps->p && *ps->p != '"') {
        unsigned char c = (unsigned char)*ps->p;
        if (c == '\\') {
            ps->p++;
            switch (*ps->p) {
            case '"':
                g_string_append_c(s, '"');
                ps->p++;
                break;
            case '\\':
                g_string_append_c(s, '\\');
                ps->p++;
                break;
            case '/':
                g_string_append_c(s, '/');
                ps->p++;
                break;
            case 'b':
                g_string_append_c(s, '\b');
                ps->p++;
                break;
            case 'f':
                g_string_append_c(s, '\f');
                ps->p++;
                break;
            case 'n':
                g_string_append_c(s, '\n');
                ps->p++;
                break;
            case 'r':
                g_string_append_c(s, '\r');
                ps->p++;
                break;
            case 't':
                g_string_append_c(s, '\t');
                ps->p++;
                break;
            case 'u': {
                ps->p++;
                unsigned int cp;
                if (!leer_hex4(ps, &cp)) {
                    error_parser(ps, "secuencia \\u invalida");
                    g_string_free(s, TRUE);
                    return NULL;
                }
                if (cp >= 0xD800 && cp <= 0xDBFF && ps->p[0] == '\\' &&
                    ps->p[1] == 'u') {
                    const char *respaldo = ps->p;
                    ps->p += 2;
                    unsigned int bajo;
                    if (leer_hex4(ps, &bajo) && bajo >= 0xDC00 &&
                        bajo <= 0xDFFF) {
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (bajo - 0xDC00);
                    } else {
                        ps->p = respaldo;
                    }
                }
                agregar_utf8(s, cp);
                break;
            }
            default:
                error_parser(ps, "escape desconocido");
                g_string_free(s, TRUE);
                return NULL;
            }
        } else if (c < 0x20) {
            error_parser(ps, "caracter de control dentro de cadena");
            g_string_free(s, TRUE);
            return NULL;
        } else {
            g_string_append_c(s, (char)c);
            ps->p++;
        }
    }

    if (*ps->p != '"') {
        error_parser(ps, "cadena sin comilla de cierre");
        g_string_free(s, TRUE);
        return NULL;
    }
    ps->p++;

    JsonValue *v = json_new_string(s->str);
    g_string_free(s, TRUE);
    return v;
}

static JsonValue *parsear_numero(Parser *ps)
{
    char *fin = NULL;
    double valor = g_ascii_strtod(ps->p, &fin);
    if (fin == ps->p) {
        error_parser(ps, "numero invalido");
        return NULL;
    }
    if (!isfinite(valor)) {
        error_parser(ps, "numero fuera de rango");
        return NULL;
    }
    ps->p = fin;
    return json_new_number(valor);
}

static JsonValue *parsear_arreglo(Parser *ps)
{
    ps->p++; /* '[' */
    JsonValue *arreglo = json_new_array();
    saltar_espacios(ps);

    if (*ps->p == ']') {
        ps->p++;
        return arreglo;
    }

    for (;;) {
        saltar_espacios(ps);
        JsonValue *item = parsear_valor(ps);
        if (!item) {
            json_free(arreglo);
            return NULL;
        }
        json_array_add(arreglo, item);
        saltar_espacios(ps);
        if (*ps->p == ',') {
            ps->p++;
            continue;
        }
        if (*ps->p == ']') {
            ps->p++;
            return arreglo;
        }
        error_parser(ps, "se esperaba ',' o ']'");
        json_free(arreglo);
        return NULL;
    }
}

static JsonValue *parsear_objeto(Parser *ps)
{
    ps->p++; /* '{' */
    JsonValue *objeto = json_new_object();
    saltar_espacios(ps);

    if (*ps->p == '}') {
        ps->p++;
        return objeto;
    }

    for (;;) {
        saltar_espacios(ps);
        if (*ps->p != '"') {
            error_parser(ps, "se esperaba una clave de texto");
            json_free(objeto);
            return NULL;
        }
        JsonValue *clave = parsear_cadena(ps);
        if (!clave) {
            json_free(objeto);
            return NULL;
        }
        saltar_espacios(ps);
        if (*ps->p != ':') {
            error_parser(ps, "se esperaba ':'");
            json_free(clave);
            json_free(objeto);
            return NULL;
        }
        ps->p++;
        saltar_espacios(ps);
        JsonValue *valor = parsear_valor(ps);
        if (!valor) {
            json_free(clave);
            json_free(objeto);
            return NULL;
        }
        json_object_set(objeto, clave->string, valor);
        json_free(clave);

        saltar_espacios(ps);
        if (*ps->p == ',') {
            ps->p++;
            continue;
        }
        if (*ps->p == '}') {
            ps->p++;
            return objeto;
        }
        error_parser(ps, "se esperaba ',' o '}'");
        json_free(objeto);
        return NULL;
    }
}

static JsonValue *parsear_valor(Parser *ps)
{
    if (ps->profundidad > 200) {
        error_parser(ps, "documento demasiado anidado");
        return NULL;
    }

    saltar_espacios(ps);

    switch (*ps->p) {
    case '\0':
        error_parser(ps, "fin de documento inesperado");
        return NULL;
    case '{':
        ps->profundidad++;
        {
            JsonValue *v = parsear_objeto(ps);
            ps->profundidad--;
            return v;
        }
    case '[':
        ps->profundidad++;
        {
            JsonValue *v = parsear_arreglo(ps);
            ps->profundidad--;
            return v;
        }
    case '"':
        return parsear_cadena(ps);
    case 't':
        if (strncmp(ps->p, "true", 4) == 0) {
            ps->p += 4;
            return json_new_bool(1);
        }
        break;
    case 'f':
        if (strncmp(ps->p, "false", 5) == 0) {
            ps->p += 5;
            return json_new_bool(0);
        }
        break;
    case 'n':
        if (strncmp(ps->p, "null", 4) == 0) {
            ps->p += 4;
            return json_null();
        }
        break;
    default:
        if (*ps->p == '-' || (*ps->p >= '0' && *ps->p <= '9'))
            return parsear_numero(ps);
        break;
    }

    error_parser(ps, "valor inesperado");
    return NULL;
}

JsonValue *json_parse(const char *texto, char **error)
{
    if (error)
        *error = NULL;
    if (!texto) {
        if (error)
            *error = g_strdup("documento vacio");
        return NULL;
    }

    Parser ps;
    ps.inicio = texto;
    ps.p = texto;
    ps.error = g_string_new(NULL);
    ps.profundidad = 0;

    JsonValue *raiz = parsear_valor(&ps);
    if (raiz) {
        saltar_espacios(&ps);
        if (*ps.p != '\0') {
            error_parser(&ps, "contenido sobrante al final del documento");
            json_free(raiz);
            raiz = NULL;
        }
    }

    if (!raiz && error)
        *error = g_strdup(ps.error->len ? ps.error->str : "JSON invalido");

    g_string_free(ps.error, TRUE);
    return raiz;
}
