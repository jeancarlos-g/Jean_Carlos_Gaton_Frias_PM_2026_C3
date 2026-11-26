#ifndef STORE_H
#define STORE_H

#include "model.h"

/* Carga el archivo JSON en la aplicacion. Devuelve 1 si todo fue bien.
 * Si el archivo no existe, deja la aplicacion vacia y devuelve 1. */
int store_load(App *app, char **error);

/* Guarda el estado completo en el archivo JSON de la aplicacion. */
int store_save(App *app, char **error);

#endif
