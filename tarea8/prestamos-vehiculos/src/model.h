#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>

/* ------------------------------------------------------------------ */
/* Estructuras enlazadas                                               */
/* ------------------------------------------------------------------ */

typedef struct Modelo {
    int id;
    char *nombre;
    double precio;
    struct Modelo *sig;
} Modelo;

typedef struct Marca {
    int id;
    char *nombre;
    Modelo *modelos;
    struct Marca *sig;
} Marca;

typedef struct Cliente {
    int id;
    char *nombre;
    char *cedula;
    char *telefono;
    char *email;
    struct Cliente *sig;
} Cliente;

typedef struct Cuota {
    int numero;
    char *mes;        /* YYYY-MM */
    double monto;
    double capital;
    double interes;
    int pagada;
    char *fecha_pago; /* YYYY-MM-DD o NULL */
    struct Cuota *sig;
} Cuota;

typedef struct Abono {
    int id;
    char *fecha;
    double monto;
    char *nota;
    struct Abono *sig;
} Abono;

typedef struct Prestamo {
    int id;
    int cliente_id;
    int marca_id;
    int modelo_id;
    char *vehiculo;
    double precio;
    double inicial;
    double financiado;
    double tasa_anual;
    int plazo;
    char *fecha_inicio;
    Cuota *cuotas;
    Abono *abonos;
    struct Prestamo *sig;
} Prestamo;

typedef struct App {
    Marca *marcas;
    Cliente *clientes;
    Prestamo *prestamos;
    int next_marca;
    int next_modelo;
    int next_cliente;
    int next_prestamo;
    int next_abono;
    char *data_file;
} App;

/* ------------------------------------------------------------------ */
/* Ciclo de vida                                                       */
/* ------------------------------------------------------------------ */

void app_init(App *app, const char *archivo_datos);
void app_clear(App *app);
void app_free(App *app);

/* ------------------------------------------------------------------ */
/* Marcas y modelos                                                    */
/* ------------------------------------------------------------------ */

Marca *marca_add(App *app, const char *nombre);
Marca *marca_find(App *app, int id);
int marca_delete(App *app, int id); /* 0 si no existe o tiene modelos */
int marca_existe(App *app, const char *nombre, int ignorar_id);

Modelo *modelo_add(App *app, Marca *marca, const char *nombre, double precio);
Modelo *modelo_find(Marca *marca, int id);
int modelo_delete(App *app, Marca *marca, int id);
int modelo_en_uso(App *app, int modelo_id);

/* ------------------------------------------------------------------ */
/* Clientes                                                            */
/* ------------------------------------------------------------------ */

Cliente *cliente_add(App *app, const char *nombre, const char *cedula,
                     const char *telefono, const char *email);
Cliente *cliente_find(App *app, int id);
Cliente *cliente_buscar_cedula(App *app, const char *cedula);
int cliente_delete(App *app, int id);
int cliente_tiene_prestamos(App *app, int cliente_id);

/* ------------------------------------------------------------------ */
/* Prestamos                                                           */
/* ------------------------------------------------------------------ */

Prestamo *prestamo_add(App *app, int cliente_id, int marca_id, int modelo_id,
                       const char *vehiculo, double precio, double inicial,
                       double tasa_anual, int plazo, const char *fecha_inicio);
Prestamo *prestamo_find(App *app, int id);
int prestamo_delete(App *app, int id);

Cuota *cuota_find(Prestamo *prestamo, int numero);

Abono *abono_add(App *app, Prestamo *prestamo, double monto, const char *fecha,
                 const char *nota);
Abono *abono_find(Prestamo *prestamo, int id);
int abono_delete(Prestamo *prestamo, int id);

/* ------------------------------------------------------------------ */
/* Calculos                                                            */
/* ------------------------------------------------------------------ */

double prestamo_total(const Prestamo *p);
double prestamo_pagado_cuotas(const Prestamo *p);
double prestamo_abonos(const Prestamo *p);
double prestamo_pagado_total(const Prestamo *p);
double prestamo_saldo(const Prestamo *p);
double prestamo_cuota_mensual(const Prestamo *p);
int prestamo_cuotas_pagadas(const Prestamo *p);
int prestamo_cuotas_vencidas(const Prestamo *p, const char *mes_actual);

/* ------------------------------------------------------------------ */
/* Fechas                                                              */
/* ------------------------------------------------------------------ */

void fecha_hoy(char *buffer, size_t n);
void mes_de_fecha(const char *fecha, char out[8]);
void sumar_meses(const char *anio_mes, int meses, char out[8]);

#endif
