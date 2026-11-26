#include "model.h"

#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Utilidades internas                                                 */
/* ------------------------------------------------------------------ */

static void agregar_marca(App *app, Marca *m)
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

static void agregar_modelo(Marca *marca, Modelo *m)
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

static void agregar_cliente(App *app, Cliente *c)
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

static void agregar_prestamo(App *app, Prestamo *pr)
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

static void agregar_abono(Prestamo *p, Abono *a)
{
    if (!p->abonos) {
        p->abonos = a;
        return;
    }
    Abono *q = p->abonos;
    while (q->sig)
        q = q->sig;
    q->sig = a;
}

static void liberar_modelos(Modelo *m)
{
    while (m) {
        Modelo *sig = m->sig;
        g_free(m->nombre);
        g_free(m);
        m = sig;
    }
}

static void liberar_cuotas(Cuota *c)
{
    while (c) {
        Cuota *sig = c->sig;
        g_free(c->mes);
        g_free(c->fecha_pago);
        g_free(c);
        c = sig;
    }
}

static void liberar_abonos(Abono *a)
{
    while (a) {
        Abono *sig = a->sig;
        g_free(a->fecha);
        g_free(a->nota);
        g_free(a);
        a = sig;
    }
}

/* ------------------------------------------------------------------ */
/* Ciclo de vida                                                       */
/* ------------------------------------------------------------------ */

void app_init(App *app, const char *archivo_datos)
{
    memset(app, 0, sizeof *app);
    app->next_marca = 1;
    app->next_modelo = 1;
    app->next_cliente = 1;
    app->next_prestamo = 1;
    app->next_abono = 1;
    app->data_file = g_strdup(archivo_datos ? archivo_datos : "data/datos.json");
}

void app_clear(App *app)
{
    if (!app)
        return;

    Marca *m = app->marcas;
    while (m) {
        Marca *sig = m->sig;
        liberar_modelos(m->modelos);
        g_free(m->nombre);
        g_free(m);
        m = sig;
    }
    app->marcas = NULL;

    Cliente *c = app->clientes;
    while (c) {
        Cliente *sig = c->sig;
        g_free(c->nombre);
        g_free(c->cedula);
        g_free(c->telefono);
        g_free(c->email);
        g_free(c);
        c = sig;
    }
    app->clientes = NULL;

    Prestamo *p = app->prestamos;
    while (p) {
        Prestamo *sig = p->sig;
        liberar_cuotas(p->cuotas);
        liberar_abonos(p->abonos);
        g_free(p->vehiculo);
        g_free(p->fecha_inicio);
        g_free(p);
        p = sig;
    }
    app->prestamos = NULL;
}

void app_free(App *app)
{
    if (!app)
        return;
    app_clear(app);
    g_free(app->data_file);
    app->data_file = NULL;
}

/* ------------------------------------------------------------------ */
/* Marcas y modelos                                                    */
/* ------------------------------------------------------------------ */

Marca *marca_add(App *app, const char *nombre)
{
    if (!app || !nombre || !*nombre)
        return NULL;
    Marca *m = g_new0(Marca, 1);
    m->id = app->next_marca++;
    m->nombre = g_strdup(nombre);
    agregar_marca(app, m);
    return m;
}

Marca *marca_find(App *app, int id)
{
    for (Marca *m = app ? app->marcas : NULL; m; m = m->sig)
        if (m->id == id)
            return m;
    return NULL;
}

int marca_existe(App *app, const char *nombre, int ignorar_id)
{
    if (!app || !nombre)
        return 0;
    for (Marca *m = app->marcas; m; m = m->sig) {
        if (m->id != ignorar_id &&
            g_ascii_strcasecmp(m->nombre, nombre) == 0)
            return 1;
    }
    return 0;
}

int marca_delete(App *app, int id)
{
    Marca **pp = app ? &app->marcas : NULL;
    if (!pp)
        return 0;
    while (*pp) {
        Marca *m = *pp;
        if (m->id == id) {
            if (m->modelos)
                return 0;
            *pp = m->sig;
            g_free(m->nombre);
            g_free(m);
            return 1;
        }
        pp = &m->sig;
    }
    return 0;
}

Modelo *modelo_add(App *app, Marca *marca, const char *nombre, double precio)
{
    if (!app || !marca || !nombre || !*nombre || precio < 0)
        return NULL;
    Modelo *m = g_new0(Modelo, 1);
    m->id = app->next_modelo++;
    m->nombre = g_strdup(nombre);
    m->precio = precio;
    agregar_modelo(marca, m);
    return m;
}

Modelo *modelo_find(Marca *marca, int id)
{
    for (Modelo *m = marca ? marca->modelos : NULL; m; m = m->sig)
        if (m->id == id)
            return m;
    return NULL;
}

int modelo_delete(App *app, Marca *marca, int id)
{
    if (!app || !marca)
        return 0;
    Modelo **pp = &marca->modelos;
    while (*pp) {
        Modelo *m = *pp;
        if (m->id == id) {
            if (modelo_en_uso(app, id))
                return 0;
            *pp = m->sig;
            g_free(m->nombre);
            g_free(m);
            return 1;
        }
        pp = &m->sig;
    }
    return 0;
}

int modelo_en_uso(App *app, int modelo_id)
{
    for (Prestamo *p = app ? app->prestamos : NULL; p; p = p->sig)
        if (p->modelo_id == modelo_id)
            return 1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Clientes                                                            */
/* ------------------------------------------------------------------ */

Cliente *cliente_add(App *app, const char *nombre, const char *cedula,
                     const char *telefono, const char *email)
{
    if (!app || !nombre || !*nombre || !cedula || !*cedula)
        return NULL;
    Cliente *c = g_new0(Cliente, 1);
    c->id = app->next_cliente++;
    c->nombre = g_strdup(nombre);
    c->cedula = g_strdup(cedula);
    c->telefono = g_strdup(telefono ? telefono : "");
    c->email = g_strdup(email ? email : "");
    agregar_cliente(app, c);
    return c;
}

Cliente *cliente_find(App *app, int id)
{
    for (Cliente *c = app ? app->clientes : NULL; c; c = c->sig)
        if (c->id == id)
            return c;
    return NULL;
}

Cliente *cliente_buscar_cedula(App *app, const char *cedula)
{
    if (!app || !cedula)
        return NULL;
    for (Cliente *c = app->clientes; c; c = c->sig)
        if (g_ascii_strcasecmp(c->cedula, cedula) == 0)
            return c;
    return NULL;
}

int cliente_delete(App *app, int id)
{
    Cliente **pp = app ? &app->clientes : NULL;
    if (!pp)
        return 0;
    while (*pp) {
        Cliente *c = *pp;
        if (c->id == id) {
            if (cliente_tiene_prestamos(app, id))
                return 0;
            *pp = c->sig;
            g_free(c->nombre);
            g_free(c->cedula);
            g_free(c->telefono);
            g_free(c->email);
            g_free(c);
            return 1;
        }
        pp = &c->sig;
    }
    return 0;
}

int cliente_tiene_prestamos(App *app, int cliente_id)
{
    for (Prestamo *p = app ? app->prestamos : NULL; p; p = p->sig)
        if (p->cliente_id == cliente_id)
            return 1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Prestamos                                                           */
/* ------------------------------------------------------------------ */

static Cuota *generar_cuotas(const Prestamo *p)
{
    Cuota *primera = NULL, *ultima = NULL;
    double interes_mensual = p->tasa_anual / 100.0 / 12.0;
    double saldo = p->financiado;
    double cuota;

    if (interes_mensual > 1e-12)
        cuota = saldo * interes_mensual /
                (1.0 - pow(1.0 + interes_mensual, -(double)p->plazo));
    else
        cuota = saldo / (double)p->plazo;

    char mes_inicio[8];
    mes_de_fecha(p->fecha_inicio, mes_inicio);

    for (int k = 1; k <= p->plazo; k++) {
        double interes = saldo * interes_mensual;
        double capital = cuota - interes;
        double monto = cuota;

        if (k == p->plazo) {
            capital = saldo;
            monto = capital + interes;
        }
        if (capital > saldo)
            capital = saldo;
        saldo -= capital;
        if (saldo < 0.005)
            saldo = 0.0;

        Cuota *c = g_new0(Cuota, 1);
        c->numero = k;
        char mes[8];
        sumar_meses(mes_inicio, k, mes);
        c->mes = g_strdup(mes);
        c->monto = monto;
        c->capital = capital;
        c->interes = interes;
        c->pagada = 0;
        c->fecha_pago = NULL;

        if (!primera)
            primera = c;
        else
            ultima->sig = c;
        ultima = c;
    }
    return primera;
}

Prestamo *prestamo_add(App *app, int cliente_id, int marca_id, int modelo_id,
                       const char *vehiculo, double precio, double inicial,
                       double tasa_anual, int plazo, const char *fecha_inicio)
{
    if (!app || precio <= 0 || inicial < 0 || inicial >= precio || plazo < 1 ||
        plazo > 120 || tasa_anual < 0 || tasa_anual > 200)
        return NULL;
    if (!fecha_inicio || !*fecha_inicio)
        return NULL;

    Prestamo *p = g_new0(Prestamo, 1);
    p->id = app->next_prestamo++;
    p->cliente_id = cliente_id;
    p->marca_id = marca_id;
    p->modelo_id = modelo_id;
    p->vehiculo = g_strdup(vehiculo ? vehiculo : "");
    p->precio = precio;
    p->inicial = inicial;
    p->financiado = precio - inicial;
    p->tasa_anual = tasa_anual;
    p->plazo = plazo;
    p->fecha_inicio = g_strdup(fecha_inicio);
    p->cuotas = generar_cuotas(p);
    p->abonos = NULL;

    agregar_prestamo(app, p);
    return p;
}

Prestamo *prestamo_find(App *app, int id)
{
    for (Prestamo *p = app ? app->prestamos : NULL; p; p = p->sig)
        if (p->id == id)
            return p;
    return NULL;
}

int prestamo_delete(App *app, int id)
{
    Prestamo **pp = app ? &app->prestamos : NULL;
    if (!pp)
        return 0;
    while (*pp) {
        Prestamo *p = *pp;
        if (p->id == id) {
            *pp = p->sig;
            liberar_cuotas(p->cuotas);
            liberar_abonos(p->abonos);
            g_free(p->vehiculo);
            g_free(p->fecha_inicio);
            g_free(p);
            return 1;
        }
        pp = &p->sig;
    }
    return 0;
}

Cuota *cuota_find(Prestamo *prestamo, int numero)
{
    for (Cuota *c = prestamo ? prestamo->cuotas : NULL; c; c = c->sig)
        if (c->numero == numero)
            return c;
    return NULL;
}

Abono *abono_add(App *app, Prestamo *prestamo, double monto, const char *fecha,
                 const char *nota)
{
    if (!app || !prestamo || monto <= 0)
        return NULL;

    Abono *a = g_new0(Abono, 1);
    a->id = app->next_abono++;
    a->monto = monto;
    if (fecha && *fecha) {
        a->fecha = g_strdup(fecha);
    } else {
        char hoy[11];
        fecha_hoy(hoy, sizeof hoy);
        a->fecha = g_strdup(hoy);
    }
    a->nota = g_strdup(nota ? nota : "");
    agregar_abono(prestamo, a);
    return a;
}

Abono *abono_find(Prestamo *prestamo, int id)
{
    for (Abono *a = prestamo ? prestamo->abonos : NULL; a; a = a->sig)
        if (a->id == id)
            return a;
    return NULL;
}

int abono_delete(Prestamo *prestamo, int id)
{
    if (!prestamo)
        return 0;
    Abono **pp = &prestamo->abonos;
    while (*pp) {
        Abono *a = *pp;
        if (a->id == id) {
            *pp = a->sig;
            g_free(a->fecha);
            g_free(a->nota);
            g_free(a);
            return 1;
        }
        pp = &a->sig;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Calculos                                                            */
/* ------------------------------------------------------------------ */

double prestamo_total(const Prestamo *p)
{
    double total = 0.0;
    for (const Cuota *c = p ? p->cuotas : NULL; c; c = c->sig)
        total += c->monto;
    return total;
}

double prestamo_pagado_cuotas(const Prestamo *p)
{
    double total = 0.0;
    for (const Cuota *c = p ? p->cuotas : NULL; c; c = c->sig)
        if (c->pagada)
            total += c->monto;
    return total;
}

double prestamo_abonos(const Prestamo *p)
{
    double total = 0.0;
    for (const Abono *a = p ? p->abonos : NULL; a; a = a->sig)
        total += a->monto;
    return total;
}

double prestamo_pagado_total(const Prestamo *p)
{
    return prestamo_pagado_cuotas(p) + prestamo_abonos(p);
}

double prestamo_saldo(const Prestamo *p)
{
    double saldo = prestamo_total(p) - prestamo_pagado_total(p);
    if (saldo < 0.005)
        saldo = 0.0;
    return saldo;
}

double prestamo_cuota_mensual(const Prestamo *p)
{
    return (p && p->cuotas) ? p->cuotas->monto : 0.0;
}

int prestamo_cuotas_pagadas(const Prestamo *p)
{
    int n = 0;
    for (const Cuota *c = p ? p->cuotas : NULL; c; c = c->sig)
        if (c->pagada)
            n++;
    return n;
}

int prestamo_cuotas_vencidas(const Prestamo *p, const char *mes_actual)
{
    int n = 0;
    for (const Cuota *c = p ? p->cuotas : NULL; c; c = c->sig)
        if (!c->pagada && mes_actual && strcmp(c->mes, mes_actual) < 0)
            n++;
    return n;
}

/* ------------------------------------------------------------------ */
/* Fechas                                                              */
/* ------------------------------------------------------------------ */

void fecha_hoy(char *buffer, size_t n)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (!tm) {
        g_strlcpy(buffer, "1970-01-01", n);
        return;
    }
    strftime(buffer, n, "%Y-%m-%d", tm);
}

void mes_de_fecha(const char *fecha, char out[8])
{
    if (fecha && strlen(fecha) >= 7)
        g_strlcpy(out, fecha, 8);
    else
        g_strlcpy(out, "0000-00", 8);
}

void sumar_meses(const char *anio_mes, int meses, char out[8])
{
    int anio = 0, mes = 1;
    if (anio_mes)
        sscanf(anio_mes, "%d-%d", &anio, &mes);
    if (mes < 1 || mes > 12) {
        g_strlcpy(out, "0000-00", 8);
        return;
    }
    int total = anio * 12 + (mes - 1) + meses;
    int nuevo_anio = total / 12;
    int nuevo_mes = total % 12 + 1;
    g_snprintf(out, 8, "%04d-%02d", nuevo_anio, nuevo_mes);
}
