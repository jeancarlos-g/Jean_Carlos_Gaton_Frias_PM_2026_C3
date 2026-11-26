#ifndef JSON_H
#define JSON_H

/*
 * Mini motor JSON escrito en C.
 * Toda la memoria se administra con las funciones de GLib (g_free).
 */

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

typedef struct JsonValue JsonValue;

struct JsonValue {
    JsonType type;
    int boolean;
    double number;
    char *string;
    JsonValue **items;  /* elementos de arreglo u objeto */
    char **keys;        /* claves, solo para objetos */
    int count;
    int capacity;
};

JsonValue *json_null(void);
JsonValue *json_new_bool(int valor);
JsonValue *json_new_number(double valor);
JsonValue *json_new_string(const char *valor);
JsonValue *json_new_array(void);
JsonValue *json_new_object(void);

void json_free(JsonValue *v);

void json_array_add(JsonValue *arreglo, JsonValue *valor);
void json_object_set(JsonValue *objeto, const char *clave, JsonValue *valor);

JsonValue *json_object_get(const JsonValue *objeto, const char *clave);

const char *json_get_string(const JsonValue *objeto, const char *clave,
                            const char *por_defecto);
double json_get_number(const JsonValue *objeto, const char *clave,
                       double por_defecto);
int json_get_int(const JsonValue *objeto, const char *clave, int por_defecto);
int json_get_bool(const JsonValue *objeto, const char *clave, int por_defecto);

/* Serializa el arbol. Si pretty != 0 usa sangria de 2 espacios.
 * La cadena devuelta se libera con g_free. */
char *json_serialize(const JsonValue *v, int pretty);

/* Analiza un documento JSON. En caso de error devuelve NULL y asigna a
 * *error un mensaje (liberar con g_free). */
JsonValue *json_parse(const char *texto, char **error);

#endif
