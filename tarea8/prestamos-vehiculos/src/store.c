#include "store.h"
#include "json.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Serializacion                                                       */
/* ------------------------------------------------------------------ */

static JsonValue *modelo_a_json(const Modelo *m)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "id", json_new_number(m->id));
    json_object_set(o, "nombre", json_new_string(m->nombre));
    json_object_set(o, "precio", json_new_number(m->precio));
    return o;
}

static JsonValue *marca_a_json(const Marca *m)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "id", json_new_number(m->id));
    json_object_set(o, "nombre", json_new_string(m->nombre));

    JsonValue *modelos = json_new_array();
    for (const Modelo *mod = m->modelos; mod; mod = mod->sig)
        json_array_add(modelos, modelo_a_json(mod));
    json_object_set(o, "modelos", modelos);
    return o;
}

static JsonValue *cliente_a_json(const Cliente *c)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "id", json_new_number(c->id));
    json_object_set(o, "nombre", json_new_string(c->nombre));
    json_object_set(o, "cedula", json_new_string(c->cedula));
    json_object_set(o, "telefono", json_new_string(c->telefono));
    json_object_set(o, "email", json_new_string(c->email));
    return o;
}

static JsonValue *cuota_a_json(const Cuota *c)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "numero", json_new_number(c->numero));
    json_object_set(o, "mes", json_new_string(c->mes));
    json_object_set(o, "monto", json_new_number(c->monto));
    json_object_set(o, "capital", json_new_number(c->capital));
    json_object_set(o, "interes", json_new_number(c->interes));
    json_object_set(o, "pagada", json_new_bool(c->pagada));
    json_object_set(o, "fecha_pago",
                    json_new_string(c->fecha_pago ? c->fecha_pago : ""));
    return o;
}

static JsonValue *abono_a_json(const Abono *a)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "id", json_new_number(a->id));
    json_object_set(o, "fecha", json_new_string(a->fecha));
    json_object_set(o, "monto", json_new_number(a->monto));
    json_object_set(o, "nota", json_new_string(a->nota));
    return o;
}

static JsonValue *prestamo_a_json(const Prestamo *p)
{
    JsonValue *o = json_new_object();
    json_object_set(o, "id", json_new_number(p->id));
    json_object_set(o, "cliente_id", json_new_number(p->cliente_id));
    json_object_set(o, "marca_id", json_new_number(p->marca_id));
    json_object_set(o, "modelo_id", json_new_number(p->modelo_id));
    json_object_set(o, "vehiculo", json_new_string(p->vehiculo));
    json_object_set(o, "precio", json_new_number(p->precio));
    json_object_set(o, "inicial", json_new_number(p->inicial));
    json_object_set(o, "financiado", json_new_number(p->financiado));
    json_object_set(o, "tasa_anual", json_new_number(p->tasa_anual));
    json_object_set(o, "plazo", json_new_number(p->plazo));
    json_object_set(o, "fecha_inicio", json_new_string(p->fecha_inicio));

    JsonValue *cuotas = json_new_array();
    for (const Cuota *c = p->cuotas; c; c = c->sig)
        json_array_add(cuotas, cuota_a_json(c));
    json_object_set(o, "cuotas", cuotas);

    JsonValue *abonos = json_new_array();
    for (const Abono *a = p->abonos; a; a = a->sig)
        json_array_add(abonos, abono_a_json(a));
    json_object_set(o, "abonos", abonos);
    return o;
}

int store_save(App *app, char **error)
{
    if (error)
        *error = NULL;
    if (!app || !app->data_file) {
        if (error)
            *error = g_strdup("Ruta de datos no configurada");
        return 0;
    }

    JsonValue *raiz = json_new_object();
    json_object_set(raiz, "version", json_new_number(1));

    JsonValue *ids = json_new_object();
    json_object_set(ids, "marca", json_new_number(app->next_marca));
    json_object_set(ids, "modelo", json_new_number(app->next_modelo));
    json_object_set(ids, "cliente", json_new_number(app->next_cliente));
    json_object_set(ids, "prestamo", json_new_number(app->next_prestamo));
    json_object_set(ids, "abono", json_new_number(app->next_abono));
    json_object_set(raiz, "next_ids", ids);

    JsonValue *marcas = json_new_array();
    for (const Marca *m = app->marcas; m; m = m->sig)
        json_array_add(marcas, marca_a_json(m));
    json_object_set(raiz, "marcas", marcas);

    JsonValue *clientes = json_new_array();
    for (const Cliente *c = app->clientes; c; c = c->sig)
        json_array_add(clientes, cliente_a_json(c));
    json_object_set(raiz, "clientes", clientes);

    JsonValue *prestamos = json_new_array();
    for (const Prestamo *p = app->prestamos; p; p = p->sig)
        json_array_add(prestamos, prestamo_a_json(p));
    json_object_set(raiz, "prestamos", prestamos);

    char *texto = json_serialize(raiz, 1);
    json_free(raiz);

    char *directorio = g_path_get_dirname(app->data_file);
    if (directorio && strcmp(directorio, ".") != 0)
        g_mkdir_with_parents(directorio, 0755);
    g_free(directorio);

    char *temporal = g_strconcat(app->data_file, ".tmp", NULL);
    GError *gerr = NULL;
    int ok = 0;

    if (g_file_set_contents(temporal, texto, -1, &gerr)) {
        if (g_rename(temporal, app->data_file) == 0) {
            ok = 1;
        } else {
            /* En algunos sistemas no se puede reemplazar directamente. */
            g_remove(app->data_file);
            if (g_rename(temporal, app->data_file) == 0)
                ok = 1;
        }
    }

    if (!ok && error) {
        *error = g_strdup_printf(
            "No se pudo escribir %s%s%s", app->data_file,
            gerr ? ": " : "", gerr ? gerr->message : "");
    }
    if (gerr)
        g_error_free(gerr);
    if (!ok)
        g_remove(temporal);

    g_free(temporal);
    g_free(texto);
    return ok;
}

/* ------------------------------------------------------------------ */
/* Carga                                                               */
/* ------------------------------------------------------------------ */

static void agregar_marca_cargada(App *app, Marca *m)
{
    if (!app->marcas) {
        app->marcas = m;
        return;
    }
    Marca *p = app->marcas;
    while (p->sig)
        p = p->sig;
    p->sig = m;
}

static void agregar_cliente_cargado(App *app, Cliente *c)
{
    if (!app->clientes) {
        app->clientes = c;
        return;
    }
    Cliente *p = app->clientes;
    while (p->sig)
        p = p->sig;
    p->sig = c;
}

static void agregar_prestamo_cargado(App *app, Prestamo *pr)
{
    if (!app->prestamos) {
        app->prestamos = pr;
        return;
    }
    Prestamo *p = app->prestamos;
    while (p->sig)
        p = p->sig;
    p->sig = pr;
}

static void agregar_modelo_cargado(Marca *marca, Modelo *m)
{
    if (!marca->modelos) {
        marca->modelos = m;
        return;
    }
    Modelo *p = marca->modelos;
    while (p->sig)
        p = p->sig;
    p->sig = m;
}

static void agregar_cuota_cargada(Prestamo *pr, Cuota *c)
{
    if (!pr->cuotas) {
        pr->cuotas = c;
        return;
    }
    Cuota *p = pr->cuotas;
    while (p->sig)
        p = p->sig;
    p->sig = c;
}

static void agregar_abono_cargado(Prestamo *pr, Abono *a)
{
    if (!pr->abonos) {
        pr->abonos = a;
        return;
    }
    Abono *p = pr->abonos;
    while (p->sig)
        p = p->sig;
    p->sig = a;
}

static void recalcular_ids(App *app)
{
    if (app->next_marca < 1)
        app->next_marca = 1;
    if (app->next_modelo < 1)
        app->next_modelo = 1;
    if (app->next_cliente < 1)
        app->next_cliente = 1;
    if (app->next_prestamo < 1)
        app->next_prestamo = 1;
    if (app->next_abono < 1)
        app->next_abono = 1;

    int max_marca = 0, max_modelo = 0, max_cliente = 0, max_prestamo = 0,
        max_abono = 0;

    for (Marca *m = app->marcas; m; m = m->sig) {
        if (m->id > max_marca)
            max_marca = m->id;
        for (Modelo *mod = m->modelos; mod; mod = mod->sig)
            if (mod->id > max_modelo)
                max_modelo = mod->id;
    }
    for (Cliente *c = app->clientes; c; c = c->sig)
        if (c->id > max_cliente)
            max_cliente = c->id;
    for (Prestamo *p = app->prestamos; p; p = p->sig) {
        if (p->id > max_prestamo)
            max_prestamo = p->id;
        for (Abono *a = p->abonos; a; a = a->sig)
            if (a->id > max_abono)
                max_abono = a->id;
    }

    if (app->next_marca <= max_marca)
        app->next_marca = max_marca + 1;
    if (app->next_modelo <= max_modelo)
        app->next_modelo = max_modelo + 1;
    if (app->next_cliente <= max_cliente)
        app->next_cliente = max_cliente + 1;
    if (app->next_prestamo <= max_prestamo)
        app->next_prestamo = max_prestamo + 1;
    if (app->next_abono <= max_abono)
        app->next_abono = max_abono + 1;
}

int store_load(App *app, char **error)
{
    if (error)
        *error = NULL;
    if (!app || !app->data_file) {
        if (error)
            *error = g_strdup("Ruta de datos no configurada");
        return 0;
    }

    app_clear(app);
    app->next_marca = 1;
    app->next_modelo = 1;
    app->next_cliente = 1;
    app->next_prestamo = 1;
    app->next_abono = 1;

    if (!g_file_test(app->data_file, G_FILE_TEST_EXISTS))
        return 1;

    char *texto = NULL;
    GError *gerr = NULL;
    if (!g_file_get_contents(app->data_file, &texto, NULL, &gerr)) {
        if (error)
            *error = g_strdup_printf("No se pudo leer %s: %s", app->data_file,
                                     gerr ? gerr->message : "error");
        if (gerr)
            g_error_free(gerr);
        return 0;
    }

    char *jerr = NULL;
    JsonValue *raiz = json_parse(texto, &jerr);
    g_free(texto);

    if (!raiz) {
        if (error)
            *error = g_strdup_printf("JSON invalido en %s: %s", app->data_file,
                                     jerr ? jerr : "error desconocido");
        g_free(jerr);
        return 0;
    }
    if (raiz->type != JSON_OBJECT) {
        if (error)
            *error = g_strdup_printf("%s no contiene un objeto JSON",
                                     app->data_file);
        json_free(raiz);
        return 0;
    }

    JsonValue *ids = json_object_get(raiz, "next_ids");
    if (ids) {
        app->next_marca = json_get_int(ids, "marca", 1);
        app->next_modelo = json_get_int(ids, "modelo", 1);
        app->next_cliente = json_get_int(ids, "cliente", 1);
        app->next_prestamo = json_get_int(ids, "prestamo", 1);
        app->next_abono = json_get_int(ids, "abono", 1);
    }

    JsonValue *marcas = json_object_get(raiz, "marcas");
    if (marcas && marcas->type == JSON_ARRAY) {
        for (int i = 0; i < marcas->count; i++) {
            JsonValue *o = marcas->items[i];
            if (!o || o->type != JSON_OBJECT)
                continue;

            Marca *m = g_new0(Marca, 1);
            m->id = json_get_int(o, "id", 0);
            if (m->id <= 0)
                m->id = app->next_marca++;
            m->nombre = g_strdup(json_get_string(o, "nombre", "Sin nombre"));

            JsonValue *modelos = json_object_get(o, "modelos");
            if (modelos && modelos->type == JSON_ARRAY) {
                for (int j = 0; j < modelos->count; j++) {
                    JsonValue *mo = modelos->items[j];
                    if (!mo || mo->type != JSON_OBJECT)
                        continue;
                    Modelo *mod = g_new0(Modelo, 1);
                    mod->id = json_get_int(mo, "id", 0);
                    if (mod->id <= 0)
                        mod->id = app->next_modelo++;
                    mod->nombre =
                        g_strdup(json_get_string(mo, "nombre", "Sin nombre"));
                    mod->precio = json_get_number(mo, "precio", 0.0);
                    agregar_modelo_cargado(m, mod);
                }
            }
            agregar_marca_cargada(app, m);
        }
    }

    JsonValue *clientes = json_object_get(raiz, "clientes");
    if (clientes && clientes->type == JSON_ARRAY) {
        for (int i = 0; i < clientes->count; i++) {
            JsonValue *o = clientes->items[i];
            if (!o || o->type != JSON_OBJECT)
                continue;
            Cliente *c = g_new0(Cliente, 1);
            c->id = json_get_int(o, "id", 0);
            if (c->id <= 0)
                c->id = app->next_cliente++;
            c->nombre = g_strdup(json_get_string(o, "nombre", "Sin nombre"));
            c->cedula = g_strdup(json_get_string(o, "cedula", ""));
            c->telefono = g_strdup(json_get_string(o, "telefono", ""));
            c->email = g_strdup(json_get_string(o, "email", ""));
            agregar_cliente_cargado(app, c);
        }
    }

    JsonValue *prestamos = json_object_get(raiz, "prestamos");
    if (prestamos && prestamos->type == JSON_ARRAY) {
        for (int i = 0; i < prestamos->count; i++) {
            JsonValue *o = prestamos->items[i];
            if (!o || o->type != JSON_OBJECT)
                continue;

            Prestamo *p = g_new0(Prestamo, 1);
            p->id = json_get_int(o, "id", 0);
            if (p->id <= 0)
                p->id = app->next_prestamo++;
            p->cliente_id = json_get_int(o, "cliente_id", 0);
            p->marca_id = json_get_int(o, "marca_id", 0);
            p->modelo_id = json_get_int(o, "modelo_id", 0);
            p->vehiculo = g_strdup(json_get_string(o, "vehiculo", ""));
            p->precio = json_get_number(o, "precio", 0.0);
            p->inicial = json_get_number(o, "inicial", 0.0);
            p->financiado = json_get_number(o, "financiado",
                                            p->precio - p->inicial);
            p->tasa_anual = json_get_number(o, "tasa_anual", 0.0);
            p->plazo = json_get_int(o, "plazo", 0);
            p->fecha_inicio =
                g_strdup(json_get_string(o, "fecha_inicio", "1970-01-01"));

            JsonValue *cuotas = json_object_get(o, "cuotas");
            if (cuotas && cuotas->type == JSON_ARRAY) {
                for (int j = 0; j < cuotas->count; j++) {
                    JsonValue *co = cuotas->items[j];
                    if (!co || co->type != JSON_OBJECT)
                        continue;
                    Cuota *c = g_new0(Cuota, 1);
                    c->numero = json_get_int(co, "numero", j + 1);
                    c->mes = g_strdup(json_get_string(co, "mes", "0000-00"));
                    c->monto = json_get_number(co, "monto", 0.0);
                    c->capital = json_get_number(co, "capital", 0.0);
                    c->interes = json_get_number(co, "interes", 0.0);
                    c->pagada = json_get_bool(co, "pagada", 0);
                    const char *fp =
                        json_get_string(co, "fecha_pago", "");
                    c->fecha_pago = *fp ? g_strdup(fp) : NULL;
                    agregar_cuota_cargada(p, c);
                }
            }

            JsonValue *abonos = json_object_get(o, "abonos");
            if (abonos && abonos->type == JSON_ARRAY) {
                for (int j = 0; j < abonos->count; j++) {
                    JsonValue *ao = abonos->items[j];
                    if (!ao || ao->type != JSON_OBJECT)
                        continue;
                    Abono *a = g_new0(Abono, 1);
                    a->id = json_get_int(ao, "id", 0);
                    if (a->id <= 0)
                        a->id = app->next_abono++;
                    a->fecha =
                        g_strdup(json_get_string(ao, "fecha", "1970-01-01"));
                    a->monto = json_get_number(ao, "monto", 0.0);
                    a->nota = g_strdup(json_get_string(ao, "nota", ""));
                    agregar_abono_cargado(p, a);
                }
            }

            agregar_prestamo_cargado(app, p);
        }
    }

    json_free(raiz);
    recalcular_ids(app);
    return 1;
}
