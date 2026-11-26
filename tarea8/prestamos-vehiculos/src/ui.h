#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

#include "model.h"

typedef struct Ui Ui;

/* Construye la ventana principal. Toma posesion de "modelo". */
Ui *ui_create(GtkApplication *app, App *modelo);

void ui_show_error(Ui *ui, const char *mensaje);

#endif
