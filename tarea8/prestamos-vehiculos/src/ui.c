#include "ui.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "store.h"
#include "util.h"

/* ------------------------------------------------------------------ */
/* Estado de la interfaz                                               */
/* ------------------------------------------------------------------ */

struct Ui {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *status;
    guint status_ctx;
    App *app;
    int cargando;

    /* Vehiculos */
    GtkListStore *ls_marcas;
    GtkListStore *ls_modelos;
    GtkWidget *tv_marcas;
    GtkWidget *tv_modelos;
    GtkWidget *lbl_modelos;

    /* Clientes */
    GtkListStore *ls_clientes;
    GtkWidget *tv_clientes;
    GtkWidget *entry_buscar;

    /* Prestamos */
    GtkListStore *ls_prestamos;
    GtkWidget *tv_prestamos;
    GtkWidget *cb_cliente;
    GtkWidget *cb_marca;
    GtkWidget *cb_modelo;
    GtkWidget *entry_precio;
    GtkWidget *entry_inicial;
    GtkWidget *entry_tasa;
    GtkWidget *entry_plazo;
    GtkWidget *entry_fecha;
    GtkWidget *lbl_financiado;

    /* Pagos */
    GtkWidget *cb_pago;
    GtkListStore *ls_cuotas;
    GtkListStore *ls_abonos;
    GtkWidget *tv_cuotas;
    GtkWidget *tv_abonos;
    GtkWidget *lbl_pago_resumen;
    GtkWidget *entry_abono_monto;
    GtkWidget *entry_abono_fecha;
    GtkWidget *entry_abono_nota;
    int prestamo_pago;

    /* Reporte */
    GtkWidget *cb_reporte;
    GtkListStore *ls_reporte;
    GtkWidget *tv_reporte;
    GtkWidget *lbl_rep_cliente;
    GtkWidget *lbl_rep_vehiculo;
    GtkWidget *lbl_rep_cond;
    GtkWidget *lbl_rep_cuotas;
    GtkWidget *lbl_rep_total;
    GtkWidget *lbl_rep_pagado;
    GtkWidget *lbl_rep_saldo;
};

static void ui_refresh_all(Ui *ui);
static void refresh_pagos(Ui *ui);
static void refresh_reporte(Ui *ui);
static void actualizar_financiado(Ui *ui);
static void refresh_combo_modelos(Ui *ui, int marca_id, int seleccion);

/* ------------------------------------------------------------------ */
/* Ayudantes generales                                                 */
/* ------------------------------------------------------------------ */

static void set_status(Ui *ui, const char *formato, ...)
{
    va_list ap;
    va_start(ap, formato);
    char *mensaje = g_strdup_vprintf(formato, ap);
    va_end(ap);

    gtk_statusbar_pop(GTK_STATUSBAR(ui->status), ui->status_ctx);
    gtk_statusbar_push(GTK_STATUSBAR(ui->status), ui->status_ctx, mensaje);
    g_free(mensaje);
}

void ui_show_error(Ui *ui, const char *mensaje)
{
    GtkWidget *dlg = gtk_message_dialog_new(
        GTK_WINDOW(ui->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s",
        mensaje ? mensaje : "Error desconocido");
    gtk_window_set_title(GTK_WINDOW(dlg), "Error");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

static void mostrar_info(Ui *ui, const char *mensaje)
{
    GtkWidget *dlg = gtk_message_dialog_new(
        GTK_WINDOW(ui->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", mensaje);
    gtk_window_set_title(GTK_WINDOW(dlg), "Información");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

static int confirmar(Ui *ui, const char *mensaje)
{
    GtkWidget *dlg = gtk_message_dialog_new(
        GTK_WINDOW(ui->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "%s", mensaje);
    gtk_window_set_title(GTK_WINDOW(dlg), "Confirmar");
    gtk_dialog_add_buttons(GTK_DIALOG(dlg), "Cancelar", GTK_RESPONSE_CANCEL,
                           "Eliminar", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_CANCEL);
    int respuesta = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    return respuesta == GTK_RESPONSE_ACCEPT;
}

static int guardar_cambios(Ui *ui)
{
    char *error = NULL;
    if (!store_save(ui->app, &error)) {
        ui_show_error(ui, error ? error : "No se pudieron guardar los datos.");
        g_free(error);
        return 0;
    }
    set_status(ui, "Datos guardados en %s", ui->app->data_file);
    return 1;
}

static GtkWidget *boton(const char *texto, const char *clase)
{
    GtkWidget *b = gtk_button_new_with_label(texto);
    if (clase)
        gtk_style_context_add_class(gtk_widget_get_style_context(b), clase);
    return b;
}

static GtkWidget *con_scroll(GtkWidget *hijo, int alto)
{
    GtkWidget *sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(sw), hijo);
    if (alto > 0)
        gtk_widget_set_size_request(sw, -1, alto);
    return sw;
}

typedef struct {
    const char *titulo;
    GType tipo;
    int derecha;
} ColDef;

static GtkWidget *crear_tabla(GtkListStore *modelo, const ColDef *columnas,
                              int cantidad)
{
    GtkWidget *tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelo));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tv),
                                 GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(tv)),
                                GTK_SELECTION_SINGLE);

    for (int i = 0; i < cantidad; i++) {
        GtkCellRenderer *r = gtk_cell_renderer_text_new();
        if (columnas[i].derecha)
            g_object_set(r, "xalign", 1.0f, NULL);

        GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes(
            columnas[i].titulo, r, "text", i, NULL);
        gtk_tree_view_column_set_resizable(c, TRUE);
        if (columnas[i].derecha)
            gtk_tree_view_column_set_alignment(c, 1.0f);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tv), c);
    }
    return tv;
}

static void celda_estado(GtkTreeViewColumn *columna, GtkCellRenderer *renderer,
                         GtkTreeModel *modelo, GtkTreeIter *iter, gpointer datos)
{
    (void)columna;
    int indice = GPOINTER_TO_INT(datos);
    char *texto = NULL;
    gtk_tree_model_get(modelo, iter, indice, &texto, -1);

    int pagada = texto && g_str_has_prefix(texto, "Pagada");
    g_object_set(renderer, "text", texto ? texto : "",
                 "foreground", pagada ? "#1a7f37" : "#cf222e", "weight",
                 pagada ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL, NULL);
    g_free(texto);
}

static void colorear_estado(GtkWidget *tv, int indice)
{
    GtkTreeViewColumn *columna =
        gtk_tree_view_get_column(GTK_TREE_VIEW(tv), indice);
    if (!columna)
        return;
    GList *celdas = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(columna));
    if (!celdas)
        return;
    GtkCellRenderer *renderer = celdas->data;
    gtk_tree_view_column_set_cell_data_func(
        columna, renderer, celda_estado, GINT_TO_POINTER(indice), NULL);
    g_list_free(celdas);
}

static int tabla_id(GtkWidget *tv)
{
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tv));
    GtkTreeModel *modelo = NULL;
    GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, &modelo, &iter))
        return 0;
    int id = 0;
    gtk_tree_model_get(modelo, &iter, 0, &id, -1);
    return id;
}

static void seleccionar_id(GtkWidget *tv, int id)
{
    GtkTreeModel *modelo = gtk_tree_view_get_model(GTK_TREE_VIEW(tv));
    if (!modelo)
        return;

    GtkTreeIter iter;
    gboolean valido = gtk_tree_model_get_iter_first(modelo, &iter);
    GtkTreeIter primero;
    int hay_primero = 0;

    while (valido) {
        int valor = 0;
        gtk_tree_model_get(modelo, &iter, 0, &valor, -1);
        if (!hay_primero) {
            primero = iter;
            hay_primero = 1;
        }
        if (valor == id) {
            gtk_tree_selection_select_iter(
                gtk_tree_view_get_selection(GTK_TREE_VIEW(tv)), &iter);
            return;
        }
        valido = gtk_tree_model_iter_next(modelo, &iter);
    }

    if (hay_primero)
        gtk_tree_selection_select_iter(
            gtk_tree_view_get_selection(GTK_TREE_VIEW(tv)), &primero);
}

static GtkWidget *crear_combo(void)
{
    GtkWidget *cb = gtk_combo_box_new();
    GtkCellRenderer *r = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb), r, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb), r, "text", 1, NULL);
    return cb;
}

static int combo_id(GtkWidget *cb)
{
    GtkTreeIter iter;
    if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(cb), &iter))
        return 0;
    GtkTreeModel *modelo = gtk_combo_box_get_model(GTK_COMBO_BOX(cb));
    int id = 0;
    gtk_tree_model_get(modelo, &iter, 0, &id, -1);
    return id;
}

static void combo_seleccionar(GtkWidget *cb, int id)
{
    GtkTreeModel *modelo = gtk_combo_box_get_model(GTK_COMBO_BOX(cb));
    if (!modelo)
        return;

    GtkTreeIter iter;
    gboolean valido = gtk_tree_model_get_iter_first(modelo, &iter);
    int primero = 0;
    int hay_primero = 0;

    while (valido) {
        int valor = 0;
        gtk_tree_model_get(modelo, &iter, 0, &valor, -1);
        if (!hay_primero) {
            primero = valor;
            hay_primero = 1;
        }
        if (valor == id) {
            gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cb), &iter);
            return;
        }
        valido = gtk_tree_model_iter_next(modelo, &iter);
    }

    if (hay_primero)
        gtk_combo_box_set_active(GTK_COMBO_BOX(cb), 0);
    else
        gtk_combo_box_set_active(GTK_COMBO_BOX(cb), -1);
    (void)primero;
}

static GtkWidget *panel(const char *titulo, GtkWidget **contenido)
{
    GtkWidget *marco = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(marco), GTK_SHADOW_NONE);

    GtkWidget *caja = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(caja), "card");
    gtk_container_add(GTK_CONTAINER(marco), caja);

    if (titulo) {
        GtkWidget *l = gtk_label_new(NULL);
        char *m = g_markup_printf_escaped(
            "<span size=\"large\" weight=\"bold\">%s</span>", titulo);
        gtk_label_set_markup(GTK_LABEL(l), m);
        gtk_label_set_xalign(GTK_LABEL(l), 0.0f);
        gtk_widget_set_halign(l, GTK_ALIGN_START);
        g_free(m);
        gtk_box_pack_start(GTK_BOX(caja), l, FALSE, FALSE, 0);
    }

    *contenido = caja;
    return marco;
}

static void campo(GtkWidget *grid, int fila, const char *texto,
                  GtkWidget *widget)
{
    GtkWidget *l = gtk_label_new(texto);
    gtk_label_set_xalign(GTK_LABEL(l), 0.0f);
    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), l, 0, fila, 1, 1);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), widget, 1, fila, 1, 1);
}

static GtkWidget *nueva_entrada(const char *texto, int ancho)
{
    GtkWidget *e = gtk_entry_new();
    if (texto)
        gtk_entry_set_text(GTK_ENTRY(e), texto);
    if (ancho > 0)
        gtk_entry_set_width_chars(GTK_ENTRY(e), ancho);
    return e;
}

static char *dialogo_texto(Ui *ui, const char *titulo, const char *etiqueta,
                           const char *inicial)
{
    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        titulo, GTK_WINDOW(ui->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancelar",
        GTK_RESPONSE_CANCEL, "Aceptar", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_ACCEPT);

    GtkWidget *caja = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 18);
    gtk_widget_set_margin_end(grid, 18);
    gtk_widget_set_margin_top(grid, 14);
    gtk_widget_set_margin_bottom(grid, 14);

    GtkWidget *entry = nueva_entrada(inicial, 30);
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
    campo(grid, 0, etiqueta, entry);
    gtk_container_add(GTK_CONTAINER(caja), grid);

    gtk_widget_show_all(dlg);
    char *resultado = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        const char *t = gtk_entry_get_text(GTK_ENTRY(entry));
        if (t && *t)
            resultado = g_strdup(t);
    }
    gtk_widget_destroy(dlg);
    return resultado;
}

/* ------------------------------------------------------------------ */
/* Refresco de datos                                                   */
/* ------------------------------------------------------------------ */

static void refresh_modelos(Ui *ui)
{
    int previo = tabla_id(ui->tv_modelos);
    int marca_id = tabla_id(ui->tv_marcas);
    Marca *m = marca_find(ui->app, marca_id);

    gtk_list_store_clear(ui->ls_modelos);

    if (!m) {
        gtk_label_set_markup(GTK_LABEL(ui->lbl_modelos), "Modelos");
        return;
    }

    char *titulo = g_markup_printf_escaped("Modelos de <b>%s</b>", m->nombre);
    gtk_label_set_markup(GTK_LABEL(ui->lbl_modelos), titulo);
    g_free(titulo);

    for (Modelo *mod = m->modelos; mod; mod = mod->sig) {
        GtkTreeIter iter;
        char *precio = fmt_money(mod->precio);
        gtk_list_store_append(ui->ls_modelos, &iter);
        gtk_list_store_set(ui->ls_modelos, &iter, 0, mod->id, 1, mod->nombre, 2,
                           precio, -1);
        g_free(precio);
    }

    seleccionar_id(ui->tv_modelos, previo);
}

static void refresh_marcas(Ui *ui)
{
    int previo = tabla_id(ui->tv_marcas);
    gtk_list_store_clear(ui->ls_marcas);

    for (Marca *m = ui->app->marcas; m; m = m->sig) {
        int cantidad = 0;
        for (Modelo *mod = m->modelos; mod; mod = mod->sig)
            cantidad++;
        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_marcas, &iter);
        gtk_list_store_set(ui->ls_marcas, &iter, 0, m->id, 1, m->nombre, 2,
                           cantidad, -1);
    }

    seleccionar_id(ui->tv_marcas, previo);
    refresh_modelos(ui);
}

static void refresh_clientes(Ui *ui)
{
    int previo = tabla_id(ui->tv_clientes);
    const char *filtro = gtk_entry_get_text(GTK_ENTRY(ui->entry_buscar));
    gtk_list_store_clear(ui->ls_clientes);

    for (Cliente *c = ui->app->clientes; c; c = c->sig) {
        if (*filtro && !texto_contiene_ci(c->nombre, filtro) &&
            !texto_contiene_ci(c->cedula, filtro) &&
            !texto_contiene_ci(c->email, filtro))
            continue;

        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_clientes, &iter);
        gtk_list_store_set(ui->ls_clientes, &iter, 0, c->id, 1, c->nombre, 2,
                           c->cedula, 3, c->telefono, 4, c->email, -1);
    }

    seleccionar_id(ui->tv_clientes, previo);
}

static void refresh_prestamos(Ui *ui)
{
    int previo = tabla_id(ui->tv_prestamos);
    gtk_list_store_clear(ui->ls_prestamos);

    for (Prestamo *p = ui->app->prestamos; p; p = p->sig) {
        Cliente *c = cliente_find(ui->app, p->cliente_id);
        char *financiado = fmt_money(p->financiado);
        char *cuota = fmt_money(prestamo_cuota_mensual(p));
        char *saldo = fmt_money(prestamo_saldo(p));
        char *pagadas = g_strdup_printf("%d/%d", prestamo_cuotas_pagadas(p),
                                        p->plazo);

        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_prestamos, &iter);
        gtk_list_store_set(ui->ls_prestamos, &iter, 0, p->id, 1,
                           c ? c->nombre : "(cliente eliminado)", 2, p->vehiculo,
                           3, financiado, 4, p->plazo, 5, cuota, 6, pagadas, 7,
                           saldo, -1);

        g_free(financiado);
        g_free(cuota);
        g_free(saldo);
        g_free(pagadas);
    }

    seleccionar_id(ui->tv_prestamos, previo);
}

static void refresh_combo_modelos(Ui *ui, int marca_id, int seleccion)
{
    GtkListStore *ls = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    Marca *m = marca_find(ui->app, marca_id);

    for (Modelo *mod = m ? m->modelos : NULL; mod; mod = mod->sig) {
        GtkTreeIter iter;
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, 0, mod->id, 1, mod->nombre, -1);
    }

    gtk_combo_box_set_model(GTK_COMBO_BOX(ui->cb_modelo), GTK_TREE_MODEL(ls));
    g_object_unref(ls);
    combo_seleccionar(ui->cb_modelo, seleccion);
}

static void refresh_combos(Ui *ui)
{
    App *a = ui->app;

    /* Clientes */
    int previo_cliente = combo_id(ui->cb_cliente);
    GtkListStore *ls = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (Cliente *c = a->clientes; c; c = c->sig) {
        GtkTreeIter iter;
        char *texto = g_strdup_printf("%s — %s", c->nombre, c->cedula);
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, 0, c->id, 1, texto, -1);
        g_free(texto);
    }
    gtk_combo_box_set_model(GTK_COMBO_BOX(ui->cb_cliente),
                            GTK_TREE_MODEL(ls));
    g_object_unref(ls);
    combo_seleccionar(ui->cb_cliente, previo_cliente);

    /* Marcas */
    int previo_marca = combo_id(ui->cb_marca);
    ls = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (Marca *m = a->marcas; m; m = m->sig) {
        GtkTreeIter iter;
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, 0, m->id, 1, m->nombre, -1);
    }
    gtk_combo_box_set_model(GTK_COMBO_BOX(ui->cb_marca), GTK_TREE_MODEL(ls));
    g_object_unref(ls);
    combo_seleccionar(ui->cb_marca, previo_marca);

    /* Modelos de la marca seleccionada */
    refresh_combo_modelos(ui, combo_id(ui->cb_marca), combo_id(ui->cb_modelo));

    /* Prestamos para pagos */
    int previo_pago = ui->prestamo_pago ? ui->prestamo_pago
                                        : combo_id(ui->cb_pago);
    ls = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (Prestamo *p = a->prestamos; p; p = p->sig) {
        Cliente *c = cliente_find(a, p->cliente_id);
        GtkTreeIter iter;
        char *texto = g_strdup_printf("%s · %s (#%d)",
                                      c ? c->nombre : "Cliente", p->vehiculo,
                                      p->id);
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, 0, p->id, 1, texto, -1);
        g_free(texto);
    }
    gtk_combo_box_set_model(GTK_COMBO_BOX(ui->cb_pago), GTK_TREE_MODEL(ls));
    g_object_unref(ls);
    combo_seleccionar(ui->cb_pago, previo_pago);
    ui->prestamo_pago = combo_id(ui->cb_pago);

    /* Prestamos para el reporte */
    int previo_reporte = combo_id(ui->cb_reporte);
    ls = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (Prestamo *p = a->prestamos; p; p = p->sig) {
        Cliente *c = cliente_find(a, p->cliente_id);
        GtkTreeIter iter;
        char *texto = g_strdup_printf("%s · %s (#%d)",
                                      c ? c->nombre : "Cliente", p->vehiculo,
                                      p->id);
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, 0, p->id, 1, texto, -1);
        g_free(texto);
    }
    gtk_combo_box_set_model(GTK_COMBO_BOX(ui->cb_reporte),
                            GTK_TREE_MODEL(ls));
    g_object_unref(ls);
    combo_seleccionar(ui->cb_reporte, previo_reporte);
}

static void refresh_pagos(Ui *ui)
{
    gtk_list_store_clear(ui->ls_cuotas);
    gtk_list_store_clear(ui->ls_abonos);

    Prestamo *p = prestamo_find(ui->app, combo_id(ui->cb_pago));
    if (!p) {
        gtk_label_set_text(GTK_LABEL(ui->lbl_pago_resumen),
                           "Seleccione un préstamo para ver sus pagos.");
        return;
    }

    for (Cuota *c = p->cuotas; c; c = c->sig) {
        char *monto = fmt_money(c->monto);
        char *capital = fmt_money(c->capital);
        char *interes = fmt_money(c->interes);
        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_cuotas, &iter);
        gtk_list_store_set(ui->ls_cuotas, &iter, 0, c->numero, 1, c->mes, 2,
                           monto, 3, capital, 4, interes, 5,
                           c->pagada ? "Pagada" : "No pagada", 6,
                           c->fecha_pago ? c->fecha_pago : "—", -1);
        g_free(monto);
        g_free(capital);
        g_free(interes);
    }

    for (Abono *a = p->abonos; a; a = a->sig) {
        char *monto = fmt_money(a->monto);
        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_abonos, &iter);
        gtk_list_store_set(ui->ls_abonos, &iter, 0, a->id, 1, a->fecha, 2,
                           monto, 3, a->nota, -1);
        g_free(monto);
    }

    char *total = fmt_money(prestamo_total(p));
    char *pagado = fmt_money(prestamo_pagado_total(p));
    char *saldo = fmt_money(prestamo_saldo(p));
    char *resumen = g_strdup_printf(
        "<b>Total:</b> %s     <b>Pagado:</b> %s     "
        "<b>Saldo:</b> <span foreground=\"#1f6feb\"><b>%s</b></span>     "
        "<b>Cuotas:</b> %d de %d",
        total, pagado, saldo, prestamo_cuotas_pagadas(p), p->plazo);
    gtk_label_set_markup(GTK_LABEL(ui->lbl_pago_resumen), resumen);
    g_free(total);
    g_free(pagado);
    g_free(saldo);
    g_free(resumen);
}

static void refresh_reporte(Ui *ui)
{
    gtk_list_store_clear(ui->ls_reporte);

    Prestamo *p = prestamo_find(ui->app, combo_id(ui->cb_reporte));
    if (!p) {
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cliente), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_vehiculo), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cond), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cuotas), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_total), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_pagado), "—");
        gtk_label_set_text(GTK_LABEL(ui->lbl_rep_saldo), "—");
        return;
    }

    Cliente *c = cliente_find(ui->app, p->cliente_id);
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cliente),
                       c ? c->nombre : "(cliente eliminado)");
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_vehiculo), p->vehiculo);

    char *financiado = fmt_money(p->financiado);
    char *cond = g_strdup_printf(
        "Financiado %s · Tasa %.2f%% anual · %d meses · Inicio %s", financiado,
        p->tasa_anual, p->plazo, p->fecha_inicio);
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cond), cond);
    g_free(financiado);
    g_free(cond);

    char *cuotas = g_strdup_printf("%d de %d pagadas",
                                   prestamo_cuotas_pagadas(p), p->plazo);
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_cuotas), cuotas);
    g_free(cuotas);

    char *total = fmt_money(prestamo_total(p));
    char *pagado = fmt_money(prestamo_pagado_total(p));
    char *saldo = fmt_money(prestamo_saldo(p));
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_total), total);
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_pagado), pagado);
    gtk_label_set_text(GTK_LABEL(ui->lbl_rep_saldo), saldo);
    g_free(total);
    g_free(pagado);
    g_free(saldo);

    double restante = prestamo_total(p);
    for (Cuota *q = p->cuotas; q; q = q->sig) {
        restante -= q->monto;
        if (restante < 0.005)
            restante = 0.0;

        char *monto = fmt_money(q->monto);
        char *pagado_cuota = fmt_money(q->pagada ? q->monto : 0.0);
        char *saldo_cuota = fmt_money(restante);

        GtkTreeIter iter;
        gtk_list_store_append(ui->ls_reporte, &iter);
        gtk_list_store_set(ui->ls_reporte, &iter, 0, q->numero, 1, q->mes, 2,
                           monto, 3, q->pagada ? "Pagada" : "No pagada", 4,
                           pagado_cuota, 5, saldo_cuota, -1);

        g_free(monto);
        g_free(pagado_cuota);
        g_free(saldo_cuota);
    }
}

static void actualizar_financiado(Ui *ui)
{
    double precio, inicial;
    if (!parse_double_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_precio)),
                          &precio) ||
        !parse_double_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_inicial)),
                          &inicial) ||
        precio <= 0) {
        gtk_label_set_markup(GTK_LABEL(ui->lbl_financiado),
                             "<b>Monto a financiar:</b> —");
        return;
    }

    double financiado = precio - inicial;
    if (financiado < 0)
        financiado = 0;
    char *monto = fmt_money(financiado);
    char *texto =
        g_strdup_printf("<b>Monto a financiar:</b> %s", monto);
    gtk_label_set_markup(GTK_LABEL(ui->lbl_financiado), texto);
    g_free(monto);
    g_free(texto);
}

static void ui_refresh_all(Ui *ui)
{
    ui->cargando = 1;

    refresh_marcas(ui);
    refresh_clientes(ui);
    refresh_prestamos(ui);
    refresh_combos(ui);

    ui->cargando = 0;

    refresh_pagos(ui);
    refresh_reporte(ui);
}

/* ------------------------------------------------------------------ */
/* Manejadores: marcas y modelos                                       */
/* ------------------------------------------------------------------ */

static void on_marca_nueva(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    char *nombre = dialogo_texto(ui, "Nueva marca", "Nombre de la marca:", "");
    if (!nombre)
        return;

    g_strstrip(nombre);
    if (!*nombre) {
        ui_show_error(ui, "El nombre de la marca no puede estar vacío.");
        g_free(nombre);
        return;
    }
    if (marca_existe(ui->app, nombre, 0)) {
        ui_show_error(ui, "Ya existe una marca con ese nombre.");
        g_free(nombre);
        return;
    }

    marca_add(ui->app, nombre);
    g_free(nombre);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Marca agregada.");
}

static void on_marca_eliminar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    int id = tabla_id(ui->tv_marcas);
    if (!id) {
        mostrar_info(ui, "Seleccione una marca de la lista.");
        return;
    }

    Marca *m = marca_find(ui->app, id);
    if (!m)
        return;
    if (m->modelos) {
        ui_show_error(
            ui, "No se puede eliminar la marca porque todavía tiene modelos. "
                "Elimine primero sus modelos.");
        return;
    }

    char *mensaje =
        g_strdup_printf("¿Eliminar la marca \"%s\"?", m->nombre);
    int ok = confirmar(ui, mensaje);
    g_free(mensaje);
    if (!ok)
        return;

    marca_delete(ui->app, id);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Marca eliminada.");
}

static void on_modelo_nuevo(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    int marca_id = tabla_id(ui->tv_marcas);
    Marca *m = marca_find(ui->app, marca_id);
    if (!m) {
        mostrar_info(ui, "Seleccione primero una marca.");
        return;
    }

    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        "Nuevo modelo", GTK_WINDOW(ui->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancelar",
        GTK_RESPONSE_CANCEL, "Guardar", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_ACCEPT);

    GtkWidget *caja = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 18);
    gtk_widget_set_margin_end(grid, 18);
    gtk_widget_set_margin_top(grid, 14);
    gtk_widget_set_margin_bottom(grid, 14);

    GtkWidget *e_nombre = nueva_entrada("", 30);
    GtkWidget *e_precio = nueva_entrada("0.00", 16);
    gtk_entry_set_activates_default(GTK_ENTRY(e_nombre), TRUE);
    gtk_entry_set_activates_default(GTK_ENTRY(e_precio), TRUE);
    campo(grid, 0, "Nombre del modelo:", e_nombre);
    campo(grid, 1, "Precio de referencia:", e_precio);
    gtk_container_add(GTK_CONTAINER(caja), grid);

    gtk_widget_show_all(dlg);
    char *nombre = NULL;
    double precio = 0;
    int valido = 0;

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        nombre = g_strdup(gtk_entry_get_text(GTK_ENTRY(e_nombre)));
        g_strstrip(nombre);
        valido = *nombre &&
                 parse_double_str(gtk_entry_get_text(GTK_ENTRY(e_precio)),
                                  &precio) &&
                 precio >= 0;
    }
    gtk_widget_destroy(dlg);

    if (!nombre)
        return;
    if (!valido) {
        ui_show_error(ui,
                      "Revise los datos: el nombre es obligatorio y el precio "
                      "debe ser un número mayor o igual a cero.");
        g_free(nombre);
        return;
    }

    modelo_add(ui->app, m, nombre, precio);
    g_free(nombre);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Modelo agregado a %s.", m->nombre);
}

static void on_modelo_eliminar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    int marca_id = tabla_id(ui->tv_marcas);
    int modelo_id = tabla_id(ui->tv_modelos);
    Marca *m = marca_find(ui->app, marca_id);
    if (!m || !modelo_id) {
        mostrar_info(ui, "Seleccione un modelo de la lista.");
        return;
    }

    Modelo *mod = modelo_find(m, modelo_id);
    if (!mod)
        return;
    if (modelo_en_uso(ui->app, modelo_id)) {
        ui_show_error(ui, "No se puede eliminar el modelo porque está asociado "
                          "a uno o más préstamos.");
        return;
    }

    char *mensaje =
        g_strdup_printf("¿Eliminar el modelo \"%s\"?", mod->nombre);
    int ok = confirmar(ui, mensaje);
    g_free(mensaje);
    if (!ok)
        return;

    modelo_delete(ui->app, m, modelo_id);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Modelo eliminado.");
}

static void on_marca_seleccionada(GtkTreeSelection *sel, gpointer datos)
{
    (void)sel;
    Ui *ui = datos;
    if (ui->cargando)
        return;
    refresh_modelos(ui);
}

/* ------------------------------------------------------------------ */
/* Manejadores: clientes                                               */
/* ------------------------------------------------------------------ */

static void on_cliente_nuevo(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;

    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        "Nuevo cliente", GTK_WINDOW(ui->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Cancelar",
        GTK_RESPONSE_CANCEL, "Guardar", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_ACCEPT);

    GtkWidget *caja = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_start(grid, 18);
    gtk_widget_set_margin_end(grid, 18);
    gtk_widget_set_margin_top(grid, 14);
    gtk_widget_set_margin_bottom(grid, 14);

    GtkWidget *e_nombre = nueva_entrada("", 30);
    GtkWidget *e_cedula = nueva_entrada("", 22);
    GtkWidget *e_telefono = nueva_entrada("", 22);
    GtkWidget *e_email = nueva_entrada("", 30);
    campo(grid, 0, "Nombre completo *:", e_nombre);
    campo(grid, 1, "Cédula / documento *:", e_cedula);
    campo(grid, 2, "Teléfono:", e_telefono);
    campo(grid, 3, "Correo electrónico:", e_email);
    gtk_container_add(GTK_CONTAINER(caja), grid);

    gtk_widget_show_all(dlg);
    char *nombre = NULL, *cedula = NULL, *telefono = NULL, *email = NULL;

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        nombre = g_strdup(gtk_entry_get_text(GTK_ENTRY(e_nombre)));
        cedula = g_strdup(gtk_entry_get_text(GTK_ENTRY(e_cedula)));
        telefono = g_strdup(gtk_entry_get_text(GTK_ENTRY(e_telefono)));
        email = g_strdup(gtk_entry_get_text(GTK_ENTRY(e_email)));
        g_strstrip(nombre);
        g_strstrip(cedula);
        g_strstrip(telefono);
        g_strstrip(email);
    }
    gtk_widget_destroy(dlg);

    if (!nombre)
        return;
    if (!*nombre || !*cedula) {
        ui_show_error(ui, "El nombre y la cédula son obligatorios.");
    } else if (cliente_buscar_cedula(ui->app, cedula)) {
        ui_show_error(ui, "Ya existe un cliente con esa cédula.");
    } else {
        cliente_add(ui->app, nombre, cedula, telefono, email);
        guardar_cambios(ui);
        ui_refresh_all(ui);
        set_status(ui, "Cliente agregado.");
    }

    g_free(nombre);
    g_free(cedula);
    g_free(telefono);
    g_free(email);
}

static void on_cliente_eliminar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    int id = tabla_id(ui->tv_clientes);
    if (!id) {
        mostrar_info(ui, "Seleccione un cliente de la lista.");
        return;
    }

    Cliente *c = cliente_find(ui->app, id);
    if (!c)
        return;
    if (cliente_tiene_prestamos(ui->app, id)) {
        ui_show_error(ui, "No se puede eliminar el cliente porque tiene "
                          "préstamos registrados.");
        return;
    }

    char *mensaje =
        g_strdup_printf("¿Eliminar al cliente \"%s\"?", c->nombre);
    int ok = confirmar(ui, mensaje);
    g_free(mensaje);
    if (!ok)
        return;

    cliente_delete(ui->app, id);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Cliente eliminado.");
}

static void on_buscar_changed(GtkEditable *e, gpointer datos)
{
    (void)e;
    Ui *ui = datos;
    refresh_clientes(ui);
}

static void on_buscar_limpiar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    gtk_entry_set_text(GTK_ENTRY(ui->entry_buscar), "");
    refresh_clientes(ui);
}

/* ------------------------------------------------------------------ */
/* Manejadores: prestamos                                              */
/* ------------------------------------------------------------------ */

static void on_marca_combo_changed(GtkComboBox *cb, gpointer datos)
{
    (void)cb;
    Ui *ui = datos;
    if (ui->cargando)
        return;
    refresh_combo_modelos(ui, combo_id(ui->cb_marca), 0);
}

static void on_modelo_combo_changed(GtkComboBox *cb, gpointer datos)
{
    (void)cb;
    Ui *ui = datos;
    if (ui->cargando)
        return;

    Marca *m = marca_find(ui->app, combo_id(ui->cb_marca));
    Modelo *mod = modelo_find(m, combo_id(ui->cb_modelo));
    if (mod) {
        char *precio = g_strdup_printf("%.2f", mod->precio);
        gtk_entry_set_text(GTK_ENTRY(ui->entry_precio), precio);
        g_free(precio);
    }
    actualizar_financiado(ui);
}

static void on_precio_changed(GtkEditable *e, gpointer datos)
{
    (void)e;
    actualizar_financiado(datos);
}

static void on_prestamo_crear(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    App *a = ui->app;

    int cliente_id = combo_id(ui->cb_cliente);
    int marca_id = combo_id(ui->cb_marca);
    int modelo_id = combo_id(ui->cb_modelo);

    if (!cliente_id) {
        ui_show_error(ui, "Seleccione un cliente. Si la lista está vacía, "
                          "agregue uno en la pestaña Clientes.");
        return;
    }
    Marca *m = marca_find(a, marca_id);
    Modelo *mod = modelo_find(m, modelo_id);
    if (!m || !mod) {
        ui_show_error(ui, "Seleccione una marca y un modelo.");
        return;
    }

    double precio, inicial, tasa;
    int plazo;
    if (!parse_double_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_precio)),
                          &precio) ||
        precio <= 0) {
        ui_show_error(ui, "El precio del vehículo debe ser mayor que cero.");
        return;
    }
    if (!parse_double_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_inicial)),
                          &inicial) ||
        inicial < 0) {
        ui_show_error(ui, "La inicial debe ser un número mayor o igual a cero.");
        return;
    }
    if (inicial >= precio) {
        ui_show_error(ui, "La inicial debe ser menor que el precio del "
                          "vehículo.");
        return;
    }
    if (!parse_double_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_tasa)),
                          &tasa) ||
        tasa < 0 || tasa > 200) {
        ui_show_error(ui, "La tasa anual debe estar entre 0 y 200.");
        return;
    }
    if (!parse_int_str(gtk_entry_get_text(GTK_ENTRY(ui->entry_plazo)),
                       &plazo) ||
        plazo < 1 || plazo > 120) {
        ui_show_error(ui, "El plazo debe ser un número entero entre 1 y 120 "
                          "meses.");
        return;
    }
    const char *fecha = gtk_entry_get_text(GTK_ENTRY(ui->entry_fecha));
    if (!fecha_valida(fecha)) {
        ui_show_error(ui, "La fecha de inicio debe tener el formato "
                          "AAAA-MM-DD.");
        return;
    }

    char *vehiculo = g_strdup_printf("%s %s", m->nombre, mod->nombre);
    Prestamo *p = prestamo_add(a, cliente_id, marca_id, modelo_id, vehiculo,
                               precio, inicial, tasa, plazo, fecha);
    g_free(vehiculo);

    if (!p) {
        ui_show_error(ui, "No se pudo crear el préstamo. Revise los datos.");
        return;
    }

    ui->prestamo_pago = p->id;
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Préstamo #%d creado con %d cuotas mensuales.", p->id,
               p->plazo);
}

static void on_prestamo_eliminar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    int id = tabla_id(ui->tv_prestamos);
    if (!id) {
        mostrar_info(ui, "Seleccione un préstamo de la lista.");
        return;
    }

    Prestamo *p = prestamo_find(ui->app, id);
    if (!p)
        return;

    char *mensaje = g_strdup_printf(
        "¿Eliminar el préstamo #%d de %s? También se eliminarán sus cuotas y "
        "abonos.",
        p->id, p->vehiculo);
    int ok = confirmar(ui, mensaje);
    g_free(mensaje);
    if (!ok)
        return;

    prestamo_delete(ui->app, id);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Préstamo eliminado.");
}

/* ------------------------------------------------------------------ */
/* Manejadores: pagos                                                  */
/* ------------------------------------------------------------------ */

static void on_pago_prestamo_changed(GtkComboBox *cb, gpointer datos)
{
    (void)cb;
    Ui *ui = datos;
    if (ui->cargando)
        return;
    ui->prestamo_pago = combo_id(ui->cb_pago);
    refresh_pagos(ui);
}

static void marcar_cuota(Ui *ui, int pagada)
{
    Prestamo *p = prestamo_find(ui->app, combo_id(ui->cb_pago));
    if (!p) {
        mostrar_info(ui, "Seleccione un préstamo.");
        return;
    }
    int numero = tabla_id(ui->tv_cuotas);
    if (!numero) {
        mostrar_info(ui, "Seleccione una cuota de la lista.");
        return;
    }
    Cuota *c = cuota_find(p, numero);
    if (!c)
        return;

    c->pagada = pagada;
    g_free(c->fecha_pago);
    c->fecha_pago = NULL;
    if (pagada) {
        char hoy[11];
        fecha_hoy(hoy, sizeof hoy);
        c->fecha_pago = g_strdup(hoy);
    }

    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Cuota %d marcada como %s.", numero,
               pagada ? "pagada" : "pendiente");
}

static void on_cuota_pagar(GtkButton *b, gpointer datos)
{
    (void)b;
    marcar_cuota(datos, 1);
}

static void on_cuota_pendiente(GtkButton *b, gpointer datos)
{
    (void)b;
    marcar_cuota(datos, 0);
}

static void on_abono_agregar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    Prestamo *p = prestamo_find(ui->app, combo_id(ui->cb_pago));
    if (!p) {
        mostrar_info(ui, "Seleccione un préstamo.");
        return;
    }

    double monto;
    if (!parse_double_str(
            gtk_entry_get_text(GTK_ENTRY(ui->entry_abono_monto)), &monto) ||
        monto <= 0) {
        ui_show_error(ui, "El monto del abono debe ser mayor que cero.");
        return;
    }

    const char *fecha = gtk_entry_get_text(GTK_ENTRY(ui->entry_abono_fecha));
    if (!fecha_valida(fecha)) {
        ui_show_error(ui, "La fecha del abono debe tener el formato "
                          "AAAA-MM-DD.");
        return;
    }

    const char *nota = gtk_entry_get_text(GTK_ENTRY(ui->entry_abono_nota));
    abono_add(ui->app, p, monto, fecha, nota);

    gtk_entry_set_text(GTK_ENTRY(ui->entry_abono_monto), "0.00");
    gtk_entry_set_text(GTK_ENTRY(ui->entry_abono_nota), "");

    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Abono registrado en el préstamo #%d.", p->id);
}

static void on_abono_eliminar(GtkButton *b, gpointer datos)
{
    (void)b;
    Ui *ui = datos;
    Prestamo *p = prestamo_find(ui->app, combo_id(ui->cb_pago));
    if (!p) {
        mostrar_info(ui, "Seleccione un préstamo.");
        return;
    }
    int id = tabla_id(ui->tv_abonos);
    if (!id) {
        mostrar_info(ui, "Seleccione un abono de la lista.");
        return;
    }
    if (!confirmar(ui, "¿Eliminar el abono seleccionado?"))
        return;

    abono_delete(p, id);
    guardar_cambios(ui);
    ui_refresh_all(ui);
    set_status(ui, "Abono eliminado.");
}

/* ------------------------------------------------------------------ */
/* Manejadores: reporte                                                */
/* ------------------------------------------------------------------ */

static void on_reporte_changed(GtkComboBox *cb, gpointer datos)
{
    (void)cb;
    Ui *ui = datos;
    if (ui->cargando)
        return;
    refresh_reporte(ui);
}

static void on_reporte_actualizar(GtkButton *b, gpointer datos)
{
    (void)b;
    refresh_reporte(datos);
}

/* ------------------------------------------------------------------ */
/* Construccion de pestanas                                            */
/* ------------------------------------------------------------------ */

static GtkWidget *tab_vehiculos(Ui *ui)
{
    GtkWidget *h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_margin_start(h, 16);
    gtk_widget_set_margin_end(h, 16);
    gtk_widget_set_margin_top(h, 16);
    gtk_widget_set_margin_bottom(h, 16);

    /* Marcas */
    GtkWidget *caja;
    GtkWidget *marco = panel("Marcas", &caja);
    ui->ls_marcas = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);
    const ColDef cols_marcas[] = {{"ID", G_TYPE_INT, 1},
                                  {"Marca", G_TYPE_STRING, 0},
                                  {"Modelos", G_TYPE_INT, 1}};
    ui->tv_marcas =
        crear_tabla(ui->ls_marcas, cols_marcas, G_N_ELEMENTS(cols_marcas));
    gtk_box_pack_start(GTK_BOX(caja), con_scroll(ui->tv_marcas, 0), TRUE, TRUE,
                       0);

    GtkWidget *fila = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *b_nueva = boton("Agregar marca", "suggested-action");
    GtkWidget *b_borrar = boton("Eliminar marca", "destructive-action");
    gtk_box_pack_start(GTK_BOX(fila), b_nueva, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila), b_borrar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja), fila, FALSE, FALSE, 0);

    g_signal_connect(b_nueva, "clicked", G_CALLBACK(on_marca_nueva), ui);
    g_signal_connect(b_borrar, "clicked", G_CALLBACK(on_marca_eliminar), ui);
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(ui->tv_marcas)),
                     "changed", G_CALLBACK(on_marca_seleccionada), ui);

    /* Modelos */
    GtkWidget *caja2;
    GtkWidget *marco2 = panel(NULL, &caja2);
    ui->lbl_modelos = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(ui->lbl_modelos), "Modelos");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_modelos), 0.0f);
    gtk_widget_set_halign(ui->lbl_modelos, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui->lbl_modelos),
                                "h2");
    gtk_box_pack_start(GTK_BOX(caja2), ui->lbl_modelos, FALSE, FALSE, 0);

    ui->ls_modelos = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING,
                                        G_TYPE_STRING);
    const ColDef cols_modelos[] = {{"ID", G_TYPE_INT, 1},
                                   {"Modelo", G_TYPE_STRING, 0},
                                   {"Precio ref.", G_TYPE_STRING, 1}};
    ui->tv_modelos =
        crear_tabla(ui->ls_modelos, cols_modelos, G_N_ELEMENTS(cols_modelos));
    gtk_box_pack_start(GTK_BOX(caja2), con_scroll(ui->tv_modelos, 0), TRUE,
                       TRUE, 0);

    fila = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *b_modelo = boton("Agregar modelo", "suggested-action");
    GtkWidget *b_modelo_borrar = boton("Eliminar modelo",
                                       "destructive-action");
    gtk_box_pack_start(GTK_BOX(fila), b_modelo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila), b_modelo_borrar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja2), fila, FALSE, FALSE, 0);

    g_signal_connect(b_modelo, "clicked", G_CALLBACK(on_modelo_nuevo), ui);
    g_signal_connect(b_modelo_borrar, "clicked",
                     G_CALLBACK(on_modelo_eliminar), ui);

    gtk_box_pack_start(GTK_BOX(h), marco, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(h), marco2, TRUE, TRUE, 0);
    return h;
}

static GtkWidget *tab_clientes(Ui *ui)
{
    GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(v, 16);
    gtk_widget_set_margin_end(v, 16);
    gtk_widget_set_margin_top(v, 16);
    gtk_widget_set_margin_bottom(v, 16);

    GtkWidget *caja;
    GtkWidget *marco = panel("Clientes", &caja);

    GtkWidget *busqueda = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *l = gtk_label_new("Buscar:");
    ui->entry_buscar = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->entry_buscar),
                                   "Nombre, cédula o correo");
    gtk_widget_set_hexpand(ui->entry_buscar, TRUE);
    GtkWidget *b_limpiar = boton("Limpiar", NULL);
    gtk_box_pack_start(GTK_BOX(busqueda), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(busqueda), ui->entry_buscar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(busqueda), b_limpiar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja), busqueda, FALSE, FALSE, 0);

    ui->ls_clientes = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING,
                                         G_TYPE_STRING, G_TYPE_STRING,
                                         G_TYPE_STRING);
    const ColDef cols[] = {{"ID", G_TYPE_INT, 1},
                           {"Nombre", G_TYPE_STRING, 0},
                           {"Cédula", G_TYPE_STRING, 0},
                           {"Teléfono", G_TYPE_STRING, 0},
                           {"Correo", G_TYPE_STRING, 0}};
    ui->tv_clientes =
        crear_tabla(ui->ls_clientes, cols, G_N_ELEMENTS(cols));
    gtk_box_pack_start(GTK_BOX(caja), con_scroll(ui->tv_clientes, 0), TRUE,
                       TRUE, 0);

    GtkWidget *fila = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *b_nuevo = boton("Nuevo cliente", "suggested-action");
    GtkWidget *b_borrar = boton("Eliminar cliente", "destructive-action");
    gtk_box_pack_start(GTK_BOX(fila), b_nuevo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila), b_borrar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja), fila, FALSE, FALSE, 0);

    g_signal_connect(b_nuevo, "clicked", G_CALLBACK(on_cliente_nuevo), ui);
    g_signal_connect(b_borrar, "clicked", G_CALLBACK(on_cliente_eliminar), ui);
    g_signal_connect(ui->entry_buscar, "changed",
                     G_CALLBACK(on_buscar_changed), ui);
    g_signal_connect(b_limpiar, "clicked", G_CALLBACK(on_buscar_limpiar), ui);

    gtk_box_pack_start(GTK_BOX(v), marco, TRUE, TRUE, 0);
    return v;
}

static GtkWidget *tab_prestamos(Ui *ui)
{
    GtkWidget *h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_widget_set_margin_start(h, 16);
    gtk_widget_set_margin_end(h, 16);
    gtk_widget_set_margin_top(h, 16);
    gtk_widget_set_margin_bottom(h, 16);

    GtkWidget *caja;
    GtkWidget *marco = panel("Préstamos registrados", &caja);
    ui->ls_prestamos = gtk_list_store_new(
        8, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    const ColDef cols[] = {{"ID", G_TYPE_INT, 1},
                           {"Cliente", G_TYPE_STRING, 0},
                           {"Vehículo", G_TYPE_STRING, 0},
                           {"Financiado", G_TYPE_STRING, 1},
                           {"Plazo", G_TYPE_INT, 1},
                           {"Cuota", G_TYPE_STRING, 1},
                           {"Pagadas", G_TYPE_STRING, 1},
                           {"Saldo", G_TYPE_STRING, 1}};
    ui->tv_prestamos =
        crear_tabla(ui->ls_prestamos, cols, G_N_ELEMENTS(cols));
    gtk_box_pack_start(GTK_BOX(caja), con_scroll(ui->tv_prestamos, 0), TRUE,
                       TRUE, 0);

    GtkWidget *b_borrar = boton("Eliminar préstamo", "destructive-action");
    gtk_widget_set_halign(b_borrar, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(caja), b_borrar, FALSE, FALSE, 0);
    g_signal_connect(b_borrar, "clicked", G_CALLBACK(on_prestamo_eliminar), ui);

    GtkWidget *caja_form;
    GtkWidget *marco_form = panel("Nuevo préstamo", &caja_form);
    gtk_widget_set_size_request(marco_form, 380, -1);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);

    ui->cb_cliente = crear_combo();
    ui->cb_marca = crear_combo();
    ui->cb_modelo = crear_combo();
    ui->entry_precio = nueva_entrada("0.00", 14);
    ui->entry_inicial = nueva_entrada("0.00", 14);
    ui->entry_tasa = nueva_entrada("12.00", 10);
    ui->entry_plazo = nueva_entrada("24", 10);
    ui->entry_fecha = nueva_entrada("", 14);

    char hoy[11];
    fecha_hoy(hoy, sizeof hoy);
    gtk_entry_set_text(GTK_ENTRY(ui->entry_fecha), hoy);

    campo(grid, 0, "Cliente:", ui->cb_cliente);
    campo(grid, 1, "Marca:", ui->cb_marca);
    campo(grid, 2, "Modelo:", ui->cb_modelo);
    campo(grid, 3, "Precio del vehículo:", ui->entry_precio);
    campo(grid, 4, "Inicial:", ui->entry_inicial);
    campo(grid, 5, "Tasa anual (%):", ui->entry_tasa);
    campo(grid, 6, "Plazo (meses):", ui->entry_plazo);
    campo(grid, 7, "Fecha de inicio:", ui->entry_fecha);

    ui->lbl_financiado = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(ui->lbl_financiado),
                         "<b>Monto a financiar:</b> —");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_financiado), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui->lbl_financiado),
                                "saldo");
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_financiado, 0, 8, 2, 1);

    GtkWidget *b_crear = boton("Crear préstamo", "suggested-action");
    gtk_grid_attach(GTK_GRID(grid), b_crear, 0, 9, 2, 1);

    gtk_box_pack_start(GTK_BOX(caja_form), grid, FALSE, FALSE, 0);

    g_signal_connect(ui->cb_marca, "changed",
                     G_CALLBACK(on_marca_combo_changed), ui);
    g_signal_connect(ui->cb_modelo, "changed",
                     G_CALLBACK(on_modelo_combo_changed), ui);
    g_signal_connect(ui->entry_precio, "changed",
                     G_CALLBACK(on_precio_changed), ui);
    g_signal_connect(ui->entry_inicial, "changed",
                     G_CALLBACK(on_precio_changed), ui);
    g_signal_connect(b_crear, "clicked", G_CALLBACK(on_prestamo_crear), ui);

    gtk_box_pack_start(GTK_BOX(h), marco, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(h), marco_form, FALSE, FALSE, 0);
    return h;
}

static GtkWidget *tab_pagos(Ui *ui)
{
    GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(v, 16);
    gtk_widget_set_margin_end(v, 16);
    gtk_widget_set_margin_top(v, 16);
    gtk_widget_set_margin_bottom(v, 16);

    GtkWidget *caja_top;
    GtkWidget *marco_top = panel(NULL, &caja_top);
    GtkWidget *fila = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *l = gtk_label_new("Préstamo:");
    ui->cb_pago = crear_combo();
    gtk_widget_set_hexpand(ui->cb_pago, TRUE);
    gtk_box_pack_start(GTK_BOX(fila), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila), ui->cb_pago, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caja_top), fila, FALSE, FALSE, 0);

    ui->lbl_pago_resumen = gtk_label_new("Seleccione un préstamo.");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_pago_resumen), 0.0f);
    gtk_label_set_ellipsize(GTK_LABEL(ui->lbl_pago_resumen),
                            PANGO_ELLIPSIZE_END);
    gtk_box_pack_start(GTK_BOX(caja_top), ui->lbl_pago_resumen, FALSE, FALSE,
                       0);
    gtk_box_pack_start(GTK_BOX(v), marco_top, FALSE, FALSE, 0);

    GtkWidget *h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_box_pack_start(GTK_BOX(v), h, TRUE, TRUE, 0);

    /* Cuotas */
    GtkWidget *caja;
    GtkWidget *marco = panel("Cuotas mensuales", &caja);
    ui->ls_cuotas = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING,
                                       G_TYPE_STRING, G_TYPE_STRING,
                                       G_TYPE_STRING, G_TYPE_STRING,
                                       G_TYPE_STRING);
    const ColDef cols[] = {{"#", G_TYPE_INT, 1},
                           {"Mes", G_TYPE_STRING, 0},
                           {"Cuota", G_TYPE_STRING, 1},
                           {"Capital", G_TYPE_STRING, 1},
                           {"Interés", G_TYPE_STRING, 1},
                           {"Estado", G_TYPE_STRING, 0},
                           {"Fecha pago", G_TYPE_STRING, 0}};
    ui->tv_cuotas = crear_tabla(ui->ls_cuotas, cols, G_N_ELEMENTS(cols));
    colorear_estado(ui->tv_cuotas, 5);
    gtk_box_pack_start(GTK_BOX(caja), con_scroll(ui->tv_cuotas, 0), TRUE, TRUE,
                       0);

    GtkWidget *fila_botones = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *b_pagar = boton("Marcar como pagada", "suggested-action");
    GtkWidget *b_pendiente = boton("Marcar como pendiente", NULL);
    gtk_box_pack_start(GTK_BOX(fila_botones), b_pagar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila_botones), b_pendiente, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja), fila_botones, FALSE, FALSE, 0);

    g_signal_connect(b_pagar, "clicked", G_CALLBACK(on_cuota_pagar), ui);
    g_signal_connect(b_pendiente, "clicked", G_CALLBACK(on_cuota_pendiente),
                     ui);

    /* Abonos */
    GtkWidget *caja2;
    GtkWidget *marco2 = panel("Abonos", &caja2);
    gtk_widget_set_size_request(marco2, 380, -1);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    ui->entry_abono_monto = nueva_entrada("0.00", 12);
    ui->entry_abono_fecha = nueva_entrada("", 12);
    ui->entry_abono_nota = nueva_entrada("", 22);
    char hoy[11];
    fecha_hoy(hoy, sizeof hoy);
    gtk_entry_set_text(GTK_ENTRY(ui->entry_abono_fecha), hoy);
    campo(grid, 0, "Monto:", ui->entry_abono_monto);
    campo(grid, 1, "Fecha:", ui->entry_abono_fecha);
    campo(grid, 2, "Nota:", ui->entry_abono_nota);
    gtk_box_pack_start(GTK_BOX(caja2), grid, FALSE, FALSE, 0);

    GtkWidget *b_abono = boton("Registrar abono", "suggested-action");
    gtk_box_pack_start(GTK_BOX(caja2), b_abono, FALSE, FALSE, 0);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(caja2), sep, FALSE, FALSE, 0);

    GtkWidget *l_abonos = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(l_abonos),
                         "<span weight=\"bold\">Abonos registrados</span>");
    gtk_label_set_xalign(GTK_LABEL(l_abonos), 0.0f);
    gtk_box_pack_start(GTK_BOX(caja2), l_abonos, FALSE, FALSE, 0);

    ui->ls_abonos =
        gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING);
    const ColDef cols_abonos[] = {{"ID", G_TYPE_INT, 1},
                                  {"Fecha", G_TYPE_STRING, 0},
                                  {"Monto", G_TYPE_STRING, 1},
                                  {"Nota", G_TYPE_STRING, 0}};
    ui->tv_abonos =
        crear_tabla(ui->ls_abonos, cols_abonos, G_N_ELEMENTS(cols_abonos));
    gtk_box_pack_start(GTK_BOX(caja2), con_scroll(ui->tv_abonos, 180), FALSE,
                       TRUE, 0);

    GtkWidget *b_abono_borrar = boton("Eliminar abono",
                                      "destructive-action");
    gtk_box_pack_start(GTK_BOX(caja2), b_abono_borrar, FALSE, FALSE, 0);

    g_signal_connect(b_abono, "clicked", G_CALLBACK(on_abono_agregar), ui);
    g_signal_connect(b_abono_borrar, "clicked",
                     G_CALLBACK(on_abono_eliminar), ui);
    g_signal_connect(ui->cb_pago, "changed",
                     G_CALLBACK(on_pago_prestamo_changed), ui);

    gtk_box_pack_start(GTK_BOX(h), marco, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(h), marco2, FALSE, FALSE, 0);
    return v;
}

static GtkWidget *tab_reporte(Ui *ui)
{
    GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(v, 16);
    gtk_widget_set_margin_end(v, 16);
    gtk_widget_set_margin_top(v, 16);
    gtk_widget_set_margin_bottom(v, 16);

    GtkWidget *caja_top;
    GtkWidget *marco_top = panel(NULL, &caja_top);
    GtkWidget *fila = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *l = gtk_label_new("Préstamo:");
    ui->cb_reporte = crear_combo();
    gtk_widget_set_hexpand(ui->cb_reporte, TRUE);
    GtkWidget *b_actualizar = boton("Actualizar", NULL);
    gtk_box_pack_start(GTK_BOX(fila), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fila), ui->cb_reporte, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(fila), b_actualizar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(caja_top), fila, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 18);

    GtkWidget *l1 = gtk_label_new("Cliente:");
    gtk_label_set_xalign(GTK_LABEL(l1), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l1), "dim");
    ui->lbl_rep_cliente = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_cliente), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), l1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_cliente, 1, 0, 5, 1);

    GtkWidget *l2 = gtk_label_new("Vehículo:");
    gtk_label_set_xalign(GTK_LABEL(l2), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l2), "dim");
    ui->lbl_rep_vehiculo = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_vehiculo), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), l2, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_vehiculo, 1, 1, 5, 1);

    GtkWidget *l3 = gtk_label_new("Condiciones:");
    gtk_label_set_xalign(GTK_LABEL(l3), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l3), "dim");
    ui->lbl_rep_cond = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_cond), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), l3, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_cond, 1, 2, 5, 1);

    GtkWidget *l4 = gtk_label_new("Cuotas:");
    gtk_label_set_xalign(GTK_LABEL(l4), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l4), "dim");
    ui->lbl_rep_cuotas = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_cuotas), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), l4, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_cuotas, 1, 3, 5, 1);

    GtkWidget *l5 = gtk_label_new("Total a pagar:");
    gtk_label_set_xalign(GTK_LABEL(l5), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l5), "dim");
    ui->lbl_rep_total = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_total), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui->lbl_rep_total),
                                "money");
    gtk_grid_attach(GTK_GRID(grid), l5, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_total, 1, 4, 1, 1);

    GtkWidget *l6 = gtk_label_new("Pagado:");
    gtk_label_set_xalign(GTK_LABEL(l6), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l6), "dim");
    ui->lbl_rep_pagado = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_pagado), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui->lbl_rep_pagado),
                                "money");
    gtk_grid_attach(GTK_GRID(grid), l6, 2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_pagado, 3, 4, 1, 1);

    GtkWidget *l7 = gtk_label_new("Saldo pendiente:");
    gtk_label_set_xalign(GTK_LABEL(l7), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(l7), "dim");
    ui->lbl_rep_saldo = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(ui->lbl_rep_saldo), 0.0f);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui->lbl_rep_saldo),
                                "saldo");
    gtk_grid_attach(GTK_GRID(grid), l7, 4, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ui->lbl_rep_saldo, 5, 4, 1, 1);

    gtk_box_pack_start(GTK_BOX(caja_top), grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(v), marco_top, FALSE, FALSE, 0);

    GtkWidget *caja;
    GtkWidget *marco = panel("Estado de pago por mes", &caja);
    ui->ls_reporte = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING);
    const ColDef cols[] = {{"#", G_TYPE_INT, 1},
                           {"Mes", G_TYPE_STRING, 0},
                           {"Cuota", G_TYPE_STRING, 1},
                           {"Estado", G_TYPE_STRING, 0},
                           {"Pagado", G_TYPE_STRING, 1},
                           {"Saldo tras cuota", G_TYPE_STRING, 1}};
    ui->tv_reporte = crear_tabla(ui->ls_reporte, cols, G_N_ELEMENTS(cols));
    colorear_estado(ui->tv_reporte, 3);
    gtk_box_pack_start(GTK_BOX(caja), con_scroll(ui->tv_reporte, 0), TRUE, TRUE,
                       0);
    gtk_box_pack_start(GTK_BOX(v), marco, TRUE, TRUE, 0);

    g_signal_connect(ui->cb_reporte, "changed",
                     G_CALLBACK(on_reporte_changed), ui);
    g_signal_connect(b_actualizar, "clicked",
                     G_CALLBACK(on_reporte_actualizar), ui);

    return v;
}

/* ------------------------------------------------------------------ */
/* Ventana principal                                                   */
/* ------------------------------------------------------------------ */

static void aplicar_css(void)
{
    static const char *css =
        "window { background-color: #eef1f5; }\n"
        ".card { background-color: #ffffff; border: 1px solid #d8dee6; "
        "border-radius: 10px; padding: 12px; }\n"
        ".h2 { font-size: 15px; font-weight: 700; color: #24303f; }\n"
        ".dim { color: #64707d; }\n"
        ".money { font-family: monospace; }\n"
        ".saldo { font-size: 18px; font-weight: 800; color: #1f6feb; }\n"
        "treeview { background-color: #ffffff; }\n"
        "treeview:selected { background-color: #cfe3ff; color: #10233f; }\n"
        "headerbar { background-color: #1f3a5f; }\n"
        "headerbar .title { color: #ffffff; font-weight: 700; }\n"
        "headerbar .subtitle { color: #c9d6e8; }\n";

    GtkCssProvider *proveedor = gtk_css_provider_new();
    gtk_css_provider_load_from_data(proveedor, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(), GTK_STYLE_PROVIDER(proveedor),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(proveedor);
}

static void on_destroy(GtkWidget *w, gpointer datos)
{
    (void)w;
    Ui *ui = datos;

    if (ui->app) {
        char *error = NULL;
        store_save(ui->app, &error);
        g_free(error);
        app_free(ui->app);
        g_free(ui->app);
    }
    g_free(ui);
}

Ui *ui_create(GtkApplication *app, App *modelo)
{
    Ui *ui = g_new0(Ui, 1);
    ui->app = modelo;
    ui->prestamo_pago = 0;

    aplicar_css();

    ui->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(ui->window), "AutoCrédito");
    gtk_window_set_default_size(GTK_WINDOW(ui->window), 1180, 760);

    GtkWidget *barra = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(barra), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(barra), "AutoCrédito");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(barra),
                                "Gestión de préstamos para compra de "
                                "vehículos");
    gtk_window_set_titlebar(GTK_WINDOW(ui->window), barra);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(ui->window), vbox);

    ui->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(ui->notebook), GTK_POS_TOP);
    gtk_box_pack_start(GTK_BOX(vbox), ui->notebook, TRUE, TRUE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(ui->notebook), tab_vehiculos(ui),
                             gtk_label_new("Vehículos"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ui->notebook), tab_clientes(ui),
                             gtk_label_new("Clientes"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ui->notebook), tab_prestamos(ui),
                             gtk_label_new("Préstamos"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ui->notebook), tab_pagos(ui),
                             gtk_label_new("Pagos"));
    gtk_notebook_append_page(GTK_NOTEBOOK(ui->notebook), tab_reporte(ui),
                             gtk_label_new("Reporte"));

    ui->status = gtk_statusbar_new();
    ui->status_ctx = gtk_statusbar_get_context_id(GTK_STATUSBAR(ui->status),
                                                  "principal");
    gtk_box_pack_end(GTK_BOX(vbox), ui->status, FALSE, FALSE, 0);

    g_signal_connect(ui->window, "destroy", G_CALLBACK(on_destroy), ui);

    ui_refresh_all(ui);
    set_status(ui, "Listo. Los datos se guardan automáticamente en %s",
               ui->app->data_file);

    gtk_widget_show_all(ui->window);
    return ui;
}
