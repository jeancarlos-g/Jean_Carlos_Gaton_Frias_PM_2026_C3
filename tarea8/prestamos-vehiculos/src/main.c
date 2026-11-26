#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "model.h"
#include "store.h"
#include "ui.h"
#include "util.h"

/* ------------------------------------------------------------------ */
/* Pruebas automaticas (sin interfaz)                                  */
/* ------------------------------------------------------------------ */

static int chequeo(const char *nombre, int correcto)
{
    g_print("[%s] %s\n", correcto ? " OK " : "FALLA", nombre);
    return correcto ? 0 : 1;
}

static int ejecutar_selftest(void)
{
    int fallos = 0;

    /* Siempre se usa un archivo temporal para no tocar los datos reales. */
    char *ruta = g_build_filename(g_get_tmp_dir(), "autocredito_selftest.json",
                                  NULL);

    g_print("== Autocrédito: pruebas internas ==\n");
    g_print("Archivo temporal: %s\n\n", ruta);

    double numero = 0;
    fallos += chequeo(
        "interpretar monto 1,234.56",
        parse_double_str("1,234.56", &numero) && fabs(numero - 1234.56) < 0.001);
    fallos += chequeo(
        "interpretar monto 25.000,00",
        parse_double_str("25.000,00", &numero) && fabs(numero - 25000.0) < 0.001);

    g_remove(ruta);

    App a;
    app_init(&a, ruta);

    Marca *m = marca_add(&a, "Toyota");
    Modelo *mo = modelo_add(&a, m, "Corolla", 25000);
    Cliente *c = cliente_add(&a, "Ana Pérez", "001-010101-0001A",
                             "8888-1111", "ana@example.com");

    fallos += chequeo("crear marca", m != NULL);
    fallos += chequeo("crear modelo", mo != NULL);
    fallos += chequeo("crear cliente", c != NULL);

    Prestamo *p = prestamo_add(&a, c->id, m->id, mo->id, "Toyota Corolla",
                               25000, 5000, 12, 24, "2026-01-15");
    fallos += chequeo("crear préstamo", p != NULL);
    if (!p) {
        app_free(&a);
        g_remove(ruta);
        g_free(ruta);
        return 1;
    }

    int cuotas = 0;
    double capital = 0;
    for (Cuota *q = p->cuotas; q; q = q->sig) {
        cuotas++;
        capital += q->capital;
    }

    fallos += chequeo("se generan 24 cuotas", cuotas == 24);
    fallos += chequeo("la suma de capital es el monto financiado",
                      fabs(capital - 20000.0) < 0.05);
    fallos += chequeo("el saldo inicial es el total programado",
                      fabs(prestamo_saldo(p) - prestamo_total(p)) < 0.01);
    fallos += chequeo("la primera cuota vence en 2026-02",
                      p->cuotas && strcmp(p->cuotas->mes, "2026-02") == 0);

    Abono *abono = abono_add(&a, p, 1000, "2026-02-01", "Abono extra");
    fallos += chequeo("registrar abono", abono != NULL);
    fallos += chequeo("el abono reduce el saldo",
                      fabs(prestamo_saldo(p) - (prestamo_total(p) - 1000)) <
                          0.01);

    p->cuotas->pagada = 1;
    p->cuotas->fecha_pago = g_strdup("2026-02-10");
    fallos += chequeo("marcar cuota como pagada",
                      prestamo_cuotas_pagadas(p) == 1);
    fallos += chequeo(
        "el total pagado suma cuota y abono",
        fabs(prestamo_pagado_total(p) - (p->cuotas->monto + 1000)) < 0.01);

    char *error = NULL;
    fallos += chequeo("guardar JSON", store_save(&a, &error));
    if (error) {
        g_print("        %s\n", error);
        g_free(error);
        error = NULL;
    }

    App b;
    app_init(&b, ruta);
    fallos += chequeo("cargar JSON", store_load(&b, &error));
    if (error) {
        g_print("        %s\n", error);
        g_free(error);
        error = NULL;
    }

    fallos += chequeo("marca cargada",
                      b.marcas && strcmp(b.marcas->nombre, "Toyota") == 0);
    fallos += chequeo("modelo cargado",
                      b.marcas && b.marcas->modelos &&
                          strcmp(b.marcas->modelos->nombre, "Corolla") == 0);
    fallos += chequeo("cliente cargado",
                      b.clientes &&
                          strcmp(b.clientes->cedula, "001-010101-0001A") == 0);

    Prestamo *q = b.prestamos;
    fallos += chequeo("préstamo cargado", q != NULL && q->plazo == 24);
    fallos += chequeo("saldo conservado",
                      q && fabs(prestamo_saldo(q) - prestamo_saldo(p)) < 0.01);
    fallos += chequeo("abono cargado",
                      q && q->abonos &&
                          fabs(q->abonos->monto - 1000.0) < 0.01);
    fallos += chequeo("cuota pagada conservada",
                      q && q->cuotas && q->cuotas->pagada == 1);

    char *texto = NULL;
    gsize largo = 0;
    if (g_file_get_contents(ruta, &texto, &largo, NULL)) {
        fallos += chequeo("el archivo generado es JSON",
                          largo > 0 && texto[0] == '{');
        g_free(texto);
    } else {
        fallos += chequeo("leer archivo generado", 0);
    }

    app_free(&a);
    app_free(&b);
    g_remove(ruta);
    g_free(ruta);

    g_print("\n%s\n", fallos == 0 ? "Todas las comprobaciones pasaron."
                                  : "Algunas comprobaciones fallaron.");
    return fallos == 0 ? 0 : 1;
}

/* ------------------------------------------------------------------ */
/* Aplicacion                                                          */
/* ------------------------------------------------------------------ */

static void on_activate(GtkApplication *app, gpointer datos)
{
    const char *ruta = datos;

    App *modelo = g_new0(App, 1);
    app_init(modelo, ruta);

    char *error = NULL;
    if (!store_load(modelo, &error)) {
        /* La ventana se crea igualmente, con datos vacios. */
        g_print("Aviso: %s\n", error ? error : "no se pudieron cargar datos");
    }

    Ui *ui = ui_create(app, modelo);

    if (error) {
        ui_show_error(ui, error);
        g_free(error);
    }
}

static void mostrar_ayuda(const char *programa)
{
    g_print("Uso: %s [opciones]\n\n", programa);
    g_print("  --data <ruta>   Archivo JSON de datos "
            "(por defecto data/datos.json)\n");
    g_print("  --selftest      Ejecuta pruebas internas sin abrir la "
            "interfaz\n");
    g_print("  --help          Muestra esta ayuda\n");
    g_print("\nTambién se puede definir la variable PRESTAMOS_DATA.\n");
}

int main(int argc, char **argv)
{
    const char *ruta = NULL;
    int selftest = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--data") == 0 && i + 1 < argc) {
            ruta = argv[++i];
        } else if (strcmp(argv[i], "--selftest") == 0) {
            selftest = 1;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            mostrar_ayuda(argv[0]);
            return 0;
        } else {
            g_printerr("Opción desconocida: %s\n", argv[i]);
            mostrar_ayuda(argv[0]);
            return 2;
        }
    }

    if (!ruta)
        ruta = g_getenv("PRESTAMOS_DATA");
    if (!ruta)
        ruta = "data/datos.json";

    if (selftest)
        return ejecutar_selftest();

    GtkApplication *app = gtk_application_new("org.ejemplo.autocredito",
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate),
                     (gpointer)ruta);

    /* Se pasa argc=1 para que GTK no intente interpretar nuestras opciones. */
    int estado = g_application_run(G_APPLICATION(app), 1, argv);
    g_object_unref(app);
    return estado;
}
