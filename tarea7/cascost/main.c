/*
 * CasCost - Calculadora de costos de uso de un vehiculo por viaje.
 *
 * Compilar:  gcc -O2 -o cascost main.c
 * Ejecutar:  ./cascost
 *
 * Datos persistentes: combustibles.csv y vehiculos.csv (separador ';',
 * encabezados en espanol, decimales con punto).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_COMBUSTIBLES 50
#define MAX_VEHICULOS    100
#define NOMBRE_LEN       64
#define ARCH_COMBUSTIBLES "combustibles.csv"
#define ARCH_VEHICULOS    "vehiculos.csv"

/* ---------------------------------------------------------------- datos */

typedef struct {
    int   id;                       /* identificador estable del combustible */
    char  nombre[NOMBRE_LEN];
    float precio_litro;
} Combustible;

typedef struct {
    char  nombre[NOMBRE_LEN];       /* 1  */
    int   id_combustible;           /* 2  */
    float consumo_ciudad;           /* 3  l/100km */
    float consumo_carretera;        /* 4  l/100km */
    float precio_compra;            /* 5  */
    float valor_reventa;            /* 6  */
    float anios_vida;               /* 7  depreciacion lineal anual */
    float seguro_anual;             /* 8  */
    float matricula_anual;          /* 9  */
    float limpieza_mensual;         /* 10 */
    float imprevistos_anual;        /* 11 */
    float mantenimiento_km;         /* 12 por km     */
    float neumaticos_km;            /* 13 por km     */
    float mant_pesado_100k;         /* 14 cada 100000 km */
    float km_anuales;               /* 15 km estimados por ano */
} Vehiculo;

static Combustible combustibles[MAX_COMBUSTIBLES];
static int n_combustibles = 0;
static int proximo_id_combustible = 1;

static Vehiculo vehiculos[MAX_VEHICULOS];
static int n_vehiculos = 0;

/* -------------------------------------------------------------- entrada */

/* Lee una linea de stdin. Si la entrada se agota (EOF), termina el
 * programa para evitar un bucle infinito. */
static void leer_linea(char *buf, size_t n)
{
    if (!fgets(buf, (int)n, stdin)) {
        printf("\nEntrada terminada (EOF). Saliendo de CasCost.\n");
        exit(0);
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

/* Quita espacios al inicio y al final, devolviendo un apuntador dentro
 * de la misma cadena. */
static char *recortar(char *s)
{
    char *fin;
    while (*s && isspace((unsigned char)*s)) s++;
    fin = s + strlen(s);
    while (fin > s && isspace((unsigned char)fin[-1])) fin--;
    *fin = '\0';
    return s;
}

/* Permite escribir precios con coma decimal (24,50 = 24.50). */
static void coma_a_punto(char *s)
{
    for (; *s; s++)
        if (*s == ',')
            *s = '.';
}

/* Lee un flotante mayor (incluir = 0) o mayor o igual (incluir = 1)
 * que minimo. */
static float leer_flotante(const char *msg, float minimo, int incluir)
{
    char linea[128];
    char *fin;
    float v;
    for (;;) {
        printf("%s", msg);
        leer_linea(linea, sizeof linea);
        coma_a_punto(linea);
        v = strtof(linea, &fin);
        if (fin == linea || *recortar(fin) != '\0') {
            printf("  Entrada no valida: ingrese un numero.\n");
            continue;
        }
        if ((incluir && v >= minimo) || (!incluir && v > minimo))
            return v;
        printf("  Valor fuera de rango: el minimo permitido es %g%s\n",
               minimo, incluir ? " (inclusive)" : " (exclusivo)");
    }
}

/* Lee un entero dentro del rango [minimo, maximo]. */
static int leer_entero(const char *msg, int minimo, int maximo)
{
    char linea[128];
    char *fin;
    long v;
    for (;;) {
        printf("%s", msg);
        leer_linea(linea, sizeof linea);
        v = strtol(linea, &fin, 10);
        if (fin == linea || *recortar(fin) != '\0') {
            printf("  Entrada no valida: ingrese un numero.\n");
            continue;
        }
        if (v >= minimo && v <= maximo)
            return (int)v;
        printf("  Valor fuera de rango: escriba un numero entre %d y %d.\n",
               minimo, maximo);
    }
}

/* Lee texto no vacio y lo recorta. */
static void leer_texto(const char *msg, char *buf, size_t n)
{
    for (;;) {
        char *r;
        printf("%s", msg);
        leer_linea(buf, n);
        r = recortar(buf);
        if (r[0] != '\0') {
            if (r != buf)
                memmove(buf, r, strlen(r) + 1);
            return;
        }
        printf("  El texto no puede estar vacio.\n");
    }
}

/* Texto para edicion: ENTER conserva el valor actual. */
static void leer_texto_editado(const char *msg, const char *actual,
                               char *buf, size_t n)
{
    char *r;
    printf("%s [ENTER conserva \"%s\"]: ", msg, actual);
    leer_linea(buf, n);
    r = recortar(buf);
    if (r[0] == '\0') {
        strncpy(buf, actual, n - 1);
        buf[n - 1] = '\0';
        return;
    }
    if (r != buf)
        memmove(buf, r, strlen(r) + 1);
}

/* Flotante para edicion: ENTER conserva el valor actual. */
static void leer_flotante_editado(const char *msg, float actual,
                                  float minimo, int incluir, float *salida)
{
    char linea[128];
    char *fin;
    float v;
    for (;;) {
        printf("%s [ENTER conserva %g]: ", msg, actual);
        leer_linea(linea, sizeof linea);
        recortar(linea);
        if (linea[0] == '\0') {
            *salida = actual;
            return;
        }
        coma_a_punto(linea);
        v = strtof(linea, &fin);
        if (fin == linea || *recortar(fin) != '\0') {
            printf("  Entrada no valida: escriba un numero o ENTER para conservar.\n");
            continue;
        }
        if ((incluir && v >= minimo) || (!incluir && v > minimo)) {
            *salida = v;
            return;
        }
        printf("  Valor fuera de rango: el minimo permitido es %g%s\n",
               minimo, incluir ? " (inclusive)" : " (exclusivo)");
    }
}

/* Devuelve 1 (si) o 0 (no) ante una respuesta s/n. */
static int leer_confirmacion(const char *msg)
{
    char linea[16];
    for (;;) {
        printf("%s", msg);
        leer_linea(linea, sizeof linea);
        recortar(linea);
        linea[0] = (char)tolower((unsigned char)linea[0]);
        if (linea[0] == 's')
            return 1;
        if (linea[0] == 'n')
            return 0;
        printf("  Responda con s o n.\n");
    }
}

static void pausa(void)
{
    char linea[16];
    printf("\nPresione ENTER para continuar...");
    leer_linea(linea, sizeof linea);
}

/* ------------------------------------------------- persistencia (CSV) */

static void guardar_combustibles(void)
{
    FILE *f = fopen(ARCH_COMBUSTIBLES, "w");
    int i;
    if (!f) {
        printf("  ERROR: no se pudo abrir %s para escritura.\n",
               ARCH_COMBUSTIBLES);
        return;
    }
    fprintf(f, "id;nombre;precio_litro\n");
    for (i = 0; i < n_combustibles; i++)
        fprintf(f, "%d;%s;%.6g\n", combustibles[i].id,
                combustibles[i].nombre, combustibles[i].precio_litro);
    fclose(f);
}

static void guardar_vehiculos(void)
{
    FILE *f = fopen(ARCH_VEHICULOS, "w");
    int i;
    if (!f) {
        printf("  ERROR: no se pudo abrir %s para escritura.\n",
               ARCH_VEHICULOS);
        return;
    }
    fprintf(f, "nombre;id_combustible;consumo_ciudad_l_100km;"
               "consumo_carretera_l_100km;precio_compra;valor_reventa;"
               "anios_vida;seguro_anual;matricula_impuestos_anual;"
               "limpieza_mensual;imprevistos_anual;mantenimiento_km;"
               "neumaticos_km;mantenimiento_pesado_100000km;km_anuales\n");
    for (i = 0; i < n_vehiculos; i++) {
        const Vehiculo *v = &vehiculos[i];
        fprintf(f, "%s;%d;%.6g;%.6g;%.6g;%.6g;%.6g;%.6g;%.6g;%.6g;"
                   "%.6g;%.6g;%.6g;%.6g;%.6g\n",
                v->nombre, v->id_combustible, v->consumo_ciudad,
                v->consumo_carretera, v->precio_compra, v->valor_reventa,
                v->anios_vida, v->seguro_anual, v->matricula_anual,
                v->limpieza_mensual, v->imprevistos_anual,
                v->mantenimiento_km, v->neumaticos_km,
                v->mant_pesado_100k, v->km_anuales);
    }
    fclose(f);
}

/* Carga los datos; crea los archivos si no existen y, la primera vez,
 * siembra combustibles de ejemplo para que el alta de vehiculos tenga
 * opciones disponibles. */
static void cargar_datos(void)
{
    FILE *f;
    char linea[1024];

    f = fopen(ARCH_COMBUSTIBLES, "r");
    if (!f) {
        if (n_combustibles < MAX_COMBUSTIBLES) {
            combustibles[n_combustibles].id = proximo_id_combustible++;
            strcpy(combustibles[n_combustibles].nombre, "Gasolina");
            combustibles[n_combustibles].precio_litro = 1.55f;
            n_combustibles++;
        }
        if (n_combustibles < MAX_COMBUSTIBLES) {
            combustibles[n_combustibles].id = proximo_id_combustible++;
            strcpy(combustibles[n_combustibles].nombre, "Diesel");
            combustibles[n_combustibles].precio_litro = 1.35f;
            n_combustibles++;
        }
        guardar_combustibles();
        printf("Se crearon combustibles de ejemplo: Gasolina y Diesel.\n");
    } else {
        while (fgets(linea, sizeof linea, f)) {
            int id;
            char nom[NOMBRE_LEN];
            float precio;
            linea[strcspn(linea, "\r\n")] = '\0';
            recortar(linea);
            if (linea[0] == '\0' || n_combustibles >= MAX_COMBUSTIBLES)
                continue;
            /* "id;nombre;..." descarta el encabezado porque %d no casa
             * con la palabra "id". */
            if (sscanf(linea, "%d;%63[^;];%f", &id, nom, &precio) != 3)
                continue;
            if (id <= 0)
                continue;
            combustibles[n_combustibles].id = id;
            recortar(nom);
            strncpy(combustibles[n_combustibles].nombre, nom, NOMBRE_LEN - 1);
            combustibles[n_combustibles].nombre[NOMBRE_LEN - 1] = '\0';
            combustibles[n_combustibles].precio_litro = precio;
            if (id >= proximo_id_combustible)
                proximo_id_combustible = id + 1;
            n_combustibles++;
        }
        fclose(f);
    }

    f = fopen(ARCH_VEHICULOS, "r");
    if (!f) {
        guardar_vehiculos(); /* crea el archivo con su encabezado */
        return;
    }
    while (fgets(linea, sizeof linea, f)) {
        Vehiculo v = {0};
        int ok;
        linea[strcspn(linea, "\r\n")] = '\0';
        recortar(linea);
        if (linea[0] == '\0' || n_vehiculos >= MAX_VEHICULOS)
            continue;
        ok = sscanf(linea,
                    "%63[^;];%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",
                    v.nombre, &v.id_combustible, &v.consumo_ciudad,
                    &v.consumo_carretera, &v.precio_compra,
                    &v.valor_reventa, &v.anios_vida, &v.seguro_anual,
                    &v.matricula_anual, &v.limpieza_mensual,
                    &v.imprevistos_anual, &v.mantenimiento_km,
                    &v.neumaticos_km, &v.mant_pesado_100k, &v.km_anuales);
        if (ok != 15)
            continue; /* encabezado o linea danada: se ignora */
        recortar(v.nombre);
        if (v.nombre[0] == '\0' || v.id_combustible <= 0 ||
            v.consumo_ciudad <= 0 || v.consumo_carretera <= 0 ||
            v.anios_vida <= 0 || v.km_anuales <= 0)
            continue;
        vehiculos[n_vehiculos++] = v;
    }
    fclose(f);
}

static Combustible *buscar_combustible(int id)
{
    int i;
    for (i = 0; i < n_combustibles; i++)
        if (combustibles[i].id == id)
            return &combustibles[i];
    return NULL;
}

/* ---------------------------------------------- menu de combustibles */

static void listar_combustibles(void)
{
    int i;
    if (n_combustibles == 0) {
        printf("No hay combustibles registrados.\n");
        return;
    }
    printf("\n   #  %-28s %s\n", "Nombre", "Precio por litro");
    for (i = 0; i < n_combustibles; i++)
        printf("   %-2d %-28s %.2f\n", i + 1, combustibles[i].nombre,
               combustibles[i].precio_litro);
}

static void alta_combustible(void)
{
    Combustible *c;
    char nombre[NOMBRE_LEN];
    float precio;

    if (n_combustibles >= MAX_COMBUSTIBLES) {
        printf("Limite de %d combustibles alcanzado.\n", MAX_COMBUSTIBLES);
        return;
    }
    leer_texto("Nombre del combustible: ", nombre, sizeof nombre);
    precio = leer_flotante("Precio por litro (>= 0): ", 0.0f, 1);

    c = &combustibles[n_combustibles++];
    c->id = proximo_id_combustible++;
    strncpy(c->nombre, nombre, NOMBRE_LEN - 1);
    c->nombre[NOMBRE_LEN - 1] = '\0';
    c->precio_litro = precio;
    guardar_combustibles();
    printf("Combustible \"%s\" registrado con precio %.2f por litro.\n",
           nombre, precio);
}

static void editar_combustible(void)
{
    int seleccion;
    Combustible *c;
    char nombre[NOMBRE_LEN];
    float precio;

    if (n_combustibles == 0) {
        printf("No hay combustibles registrados para editar.\n");
        return;
    }
    listar_combustibles();
    seleccion = leer_entero("Numero del combustible a editar: ", 1,
                            n_combustibles);
    c = &combustibles[seleccion - 1];
    printf("Editando \"%s\" (precio actual %.2f).\n", c->nombre,
           c->precio_litro);
    leer_texto_editado("Nombre del combustible", c->nombre, nombre,
                       sizeof nombre);
    precio = c->precio_litro;
    leer_flotante_editado("Precio por litro (>= 0)", c->precio_litro,
                          0.0f, 1, &precio);
    strncpy(c->nombre, nombre, NOMBRE_LEN - 1);
    c->nombre[NOMBRE_LEN - 1] = '\0';
    c->precio_litro = precio;
    guardar_combustibles();
    printf("Combustible actualizado: \"%s\" a %.2f por litro.\n",
           c->nombre, c->precio_litro);
}

/* Devuelve 1 si el combustible esta asignado a al menos un vehiculo. */
static int combustible_en_uso(int id)
{
    int i;
    for (i = 0; i < n_vehiculos; i++)
        if (vehiculos[i].id_combustible == id)
            return 1;
    return 0;
}

static void baja_combustible(void)
{
    int seleccion;
    Combustible *c;
    int i;

    if (n_combustibles == 0) {
        printf("No hay combustibles registrados para eliminar.\n");
        return;
    }
    listar_combustibles();
    seleccion = leer_entero("Numero del combustible a eliminar: ", 1,
                            n_combustibles);
    c = &combustibles[seleccion - 1];
    if (combustible_en_uso(c->id)) {
        printf("No se puede eliminar \"%s\": hay vehiculos que lo usan.\n"
               "Primero edite o elimine esos vehiculos.\n", c->nombre);
        return;
    }
    for (i = seleccion - 1; i < n_combustibles - 1; i++)
        combustibles[i] = combustibles[i + 1];
    n_combustibles--;
    guardar_combustibles();
    printf("Combustible eliminado.\n");
}

static void menu_combustibles(void)
{
    for (;;) {
        int op;
        printf("\n--- COMBUSTIBLES ---\n"
               "1) Listar\n"
               "2) Crear\n"
               "3) Editar\n"
               "4) Eliminar\n"
               "0) Volver\n");
        op = leer_entero("Opcion: ", 0, 4);
        switch (op) {
        case 1: listar_combustibles();                break;
        case 2: alta_combustible();   pausa();        break;
        case 3: editar_combustible(); pausa();        break;
        case 4: baja_combustible();   pausa();        break;
        case 0: return;
        }
    }
}

/* ---------------------------------------------- menu de vehiculos */

/* Costos fijos anuales: seguro + matricula + limpieza x 12 + imprevistos
 * + depreciacion lineal anual (compra - reventa) / anos de vida. */
static float costo_fijo_anual(const Vehiculo *v)
{
    return v->seguro_anual + v->matricula_anual +
           v->limpieza_mensual * 12.0f + v->imprevistos_anual +
           (v->precio_compra - v->valor_reventa) / v->anios_vida;
}

static void listar_vehiculos(void)
{
    int i;
    if (n_vehiculos == 0) {
        printf("No hay vehiculos registrados.\n");
        return;
    }
    printf("\n   #  %-28s %-16s %-22s %-14s %s\n", "Nombre", "Combustible",
           "Consumo ciu/carr (l/100km)", "Fijos anuales", "Km/ano");
    for (i = 0; i < n_vehiculos; i++) {
        const Vehiculo *v = &vehiculos[i];
        const Combustible *c = buscar_combustible(v->id_combustible);
        printf("   %-2d %-28s %-16s %6.2f / %-15.2f %9.2f %12.0f\n",
               i + 1, v->nombre, c ? c->nombre : "(desconocido)",
               v->consumo_ciudad, v->consumo_carretera,
               costo_fijo_anual(v), v->km_anuales);
    }
}

/* Elegir combustible de la lista; devuelve su id o -1 si no hay ninguno. */
static int elegir_combustible(const char *msg)
{
    if (n_combustibles == 0) {
        printf("No hay combustibles registrados. Primero registre uno en"
               " el menu Combustibles.\n");
        return -1;
    }
    listar_combustibles();
    return combustibles[leer_entero(msg, 1, n_combustibles) - 1].id;
}

static void alta_vehiculo(void)
{
    Vehiculo *v;
    int id_comb;

    if (n_vehiculos >= MAX_VEHICULOS) {
        printf("Limite de %d vehiculos alcanzado.\n", MAX_VEHICULOS);
        return;
    }
    if (n_combustibles == 0) {
        printf("No hay combustibles registrados. Primero registre uno en"
               " el menu Combustibles.\n");
        return;
    }

    v = &vehiculos[n_vehiculos];
    memset(v, 0, sizeof *v);
    leer_texto("Nombre del vehiculo: ", v->nombre, sizeof v->nombre);
    printf("Elija el combustible del vehiculo.\n");
    id_comb = elegir_combustible("Numero del combustible: ");
    if (id_comb < 0)
        return;
    v->id_combustible = id_comb;

    printf("Complete los parametros (ENTER acepta solo numeros).\n");
    v->consumo_ciudad =     leer_flotante("Consumo en ciudad (l/100km, > 0): ",              0.0f, 0);
    v->consumo_carretera =  leer_flotante("Consumo en carretera (l/100km, > 0): ",           0.0f, 0);
    v->precio_compra =      leer_flotante("Precio de compra (>= 0): ",                      0.0f, 1);
    v->valor_reventa =      leer_flotante("Valor de reventa (>= 0): ",                      0.0f, 1);
    v->anios_vida =         leer_flotante("Anios de vida util (depreciacion, > 0): ",       0.0f, 0);
    v->seguro_anual =       leer_flotante("Costo de seguro por ano (>= 0): ",               0.0f, 1);
    v->matricula_anual =    leer_flotante("Matricula e impuestos por ano (>= 0): ",         0.0f, 1);
    v->limpieza_mensual =   leer_flotante("Costo de limpieza por mes (>= 0): ",             0.0f, 1);
    v->imprevistos_anual =  leer_flotante("Costo de imprevistos por ano (>= 0): ",           0.0f, 1);
    v->mantenimiento_km =   leer_flotante("Costo de mantenimiento por km (>= 0): ",          0.0f, 1);
    v->neumaticos_km =      leer_flotante("Costo de neumaticos por km (>= 0): ",            0.0f, 1);
    v->mant_pesado_100k =   leer_flotante("Mantenimiento pesado cada 100000 km (>= 0): ",   0.0f, 1);
    v->km_anuales =         leer_flotante("Km estimados por ano (> 0): ",                   0.0f, 0);

    n_vehiculos++;
    guardar_vehiculos();
    printf("Vehiculo \"%s\" registrado.\n", v->nombre);
}

static void editar_vehiculo(void)
{
    int seleccion, id_comb;
    Vehiculo *v;
    char nombre[NOMBRE_LEN];

    if (n_vehiculos == 0) {
        printf("No hay vehiculos registrados para editar.\n");
        return;
    }
    listar_vehiculos();
    seleccion = leer_entero("Numero del vehiculo a editar: ", 1,
                            n_vehiculos);
    v = &vehiculos[seleccion - 1];
    printf("Editando \"%s\". ENTER conserva el valor actual de cada campo.\n",
           v->nombre);

    leer_texto_editado("Nombre del vehiculo", v->nombre, nombre,
                       sizeof nombre);
    strncpy(v->nombre, nombre, NOMBRE_LEN - 1);
    v->nombre[NOMBRE_LEN - 1] = '\0';

    printf("Elija el combustible del vehiculo (ENTER no disponible aqui).\n");
    id_comb = elegir_combustible("Numero del combustible: ");
    if (id_comb >= 0)
        v->id_combustible = id_comb;

    leer_flotante_editado("Consumo en ciudad (l/100km, > 0)",              v->consumo_ciudad,     0.0f, 0, &v->consumo_ciudad);
    leer_flotante_editado("Consumo en carretera (l/100km, > 0)",           v->consumo_carretera,  0.0f, 0, &v->consumo_carretera);
    leer_flotante_editado("Precio de compra (>= 0)",                      v->precio_compra,      0.0f, 1, &v->precio_compra);
    leer_flotante_editado("Valor de reventa (>= 0)",                      v->valor_reventa,      0.0f, 1, &v->valor_reventa);
    leer_flotante_editado("Anios de vida util (depreciacion, > 0)",       v->anios_vida,         0.0f, 0, &v->anios_vida);
    leer_flotante_editado("Costo de seguro por ano (>= 0)",               v->seguro_anual,       0.0f, 1, &v->seguro_anual);
    leer_flotante_editado("Matricula e impuestos por ano (>= 0)",         v->matricula_anual,    0.0f, 1, &v->matricula_anual);
    leer_flotante_editado("Costo de limpieza por mes (>= 0)",             v->limpieza_mensual,   0.0f, 1, &v->limpieza_mensual);
    leer_flotante_editado("Costo de imprevistos por ano (>= 0)",          v->imprevistos_anual,  0.0f, 1, &v->imprevistos_anual);
    leer_flotante_editado("Costo de mantenimiento por km (>= 0)",         v->mantenimiento_km,   0.0f, 1, &v->mantenimiento_km);
    leer_flotante_editado("Costo de neumaticos por km (>= 0)",            v->neumaticos_km,      0.0f, 1, &v->neumaticos_km);
    leer_flotante_editado("Mantenimiento pesado cada 100000 km (>= 0)",   v->mant_pesado_100k,   0.0f, 1, &v->mant_pesado_100k);
    leer_flotante_editado("Km estimados por ano (> 0)",                   v->km_anuales,         0.0f, 0, &v->km_anuales);

    guardar_vehiculos();
    printf("Vehiculo \"%s\" actualizado.\n", v->nombre);
}

static void baja_vehiculo(void)
{
    int seleccion, i;
    Vehiculo *v;

    if (n_vehiculos == 0) {
        printf("No hay vehiculos registrados para eliminar.\n");
        return;
    }
    listar_vehiculos();
    seleccion = leer_entero("Numero del vehiculo a eliminar: ", 1,
                            n_vehiculos);
    v = &vehiculos[seleccion - 1];
    {
        char msg[NOMBRE_LEN + 48];
        sprintf(msg, "Eliminar el vehiculo \"%s\"? (s/n): ", v->nombre);
        if (!leer_confirmacion(msg)) {
            printf("Operacion cancelada: el vehiculo se conserva.\n");
            return;
        }
    }
    for (i = seleccion - 1; i < n_vehiculos - 1; i++)
        vehiculos[i] = vehiculos[i + 1];
    n_vehiculos--;
    guardar_vehiculos();
    printf("Vehiculo eliminado.\n");
}

static void menu_vehiculos(void)
{
    for (;;) {
        int op;
        printf("\n--- VEHICULOS ---\n"
               "1) Listar\n"
               "2) Crear\n"
               "3) Editar\n"
               "4) Eliminar\n"
               "0) Volver\n");
        op = leer_entero("Opcion: ", 0, 4);
        switch (op) {
        case 1: listar_vehiculos();                  break;
        case 2: alta_vehiculo();   pausa();          break;
        case 3: editar_vehiculo(); pausa();          break;
        case 4: baja_vehiculo();   pausa();          break;
        case 0: return;
        }
    }
}

/* ------------------------------------------------- calculo de viajes */

static void calcular_viaje(void)
{
    int seleccion;
    Vehiculo *v;
    Combustible *c;
    float km, pct_ciudad, km_ciudad, km_carretera, litros;
    float cost_comb, cost_mant, cost_neu, cost_pesado, fijos_anuales;
    float cost_fijos, total, por_km;
    char linea_rubro[128];

    if (n_vehiculos == 0) {
        printf("No hay vehiculos registrados. Primero registre uno en"
               " el menu Vehiculos.\n");
        return;
    }
    listar_vehiculos();
    seleccion = leer_entero("Numero del vehiculo para el viaje: ", 1,
                            n_vehiculos);
    v = &vehiculos[seleccion - 1];
    c = buscar_combustible(v->id_combustible);
    if (!c) {
        printf("El vehiculo apunta a un combustible que ya no existe."
               " Edite el vehiculo en el menu Vehiculos.\n");
        return;
    }

    km = leer_flotante("Distancia del viaje en km (> 0): ", 0.0f, 0);
    for (;;) {
        pct_ciudad = leer_flotante("Porcentaje del viaje en ciudad (0 a 100): ",
                                   0.0f, 1);
        if (pct_ciudad <= 100.0f)
            break;
        printf("  Valor fuera de rango: el porcentaje no puede superar 100.\n");
    }

    km_ciudad    = km * pct_ciudad / 100.0f;
    km_carretera = km - km_ciudad;
    litros       = v->consumo_ciudad * km_ciudad / 100.0f +
                   v->consumo_carretera * km_carretera / 100.0f;
    cost_comb    = litros * c->precio_litro;
    cost_mant    = v->mantenimiento_km * km;
    cost_neu     = v->neumaticos_km * km;
    cost_pesado  = v->mant_pesado_100k * km / 100000.0f;
    fijos_anuales = costo_fijo_anual(v);
    cost_fijos   = fijos_anuales * km / v->km_anuales;
    total        = cost_comb + cost_mant + cost_neu + cost_pesado +
                   cost_fijos;
    por_km       = total / km;

    printf("\n====================== COSTO DEL VIAJE ======================\n");
    printf("Vehiculo: %s\n", v->nombre);
    printf("Combustible: %s (precio %.2f por litro)\n", c->nombre,
           c->precio_litro);
    printf("Distancia: %.2f km | Ciudad: %.2f km | Carretera: %.2f km\n",
           km, km_ciudad, km_carretera);
    printf("-------------------------------------------------------------\n");
    sprintf(linea_rubro, "Combustible (%.2f L)", litros);
    printf("%-43s %16.2f\n", linea_rubro, cost_comb);
    printf("%-43s %16.2f\n", "Mantenimiento (por km)", cost_mant);
    printf("%-43s %16.2f\n", "Neumaticos (por km)", cost_neu);
    printf("%-43s %16.2f\n",
           "Manten. pesado (amortizado en 100000 km)", cost_pesado);
    sprintf(linea_rubro, "Costos fijos (%.2f x %.0f / %.0f)",
            fijos_anuales, km, v->km_anuales);
    printf("%-43s %16.2f\n", linea_rubro, cost_fijos);
    printf("  fijos anuales = seguro + matricula + limpieza x 12\n");
    printf("  + imprevistos + depreciacion (compra - reventa) / anos\n");
    printf("-------------------------------------------------------------\n");
    printf("%-43s %16.2f\n", "COSTO TOTAL DEL VIAJE", total);
    printf("%-43s %16.4f\n", "COSTO POR KM", por_km);
    printf("=============================================================\n");
}

/* ----------------------------------------------------------------- main */

int main(void)
{
    int op;
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    cargar_datos();
    printf("Bienvenido a CasCost: calculadora de costos de viajes.\n");
    for (;;) {
        printf("\n=============== CAS COST ===============\n"
               "1) Vehiculos\n"
               "2) Combustibles\n"
               "3) Calcular viaje\n"
               "0) Salir\n");
        op = leer_entero("Opcion: ", 0, 3);
        switch (op) {
        case 1: menu_vehiculos();                          break;
        case 2: menu_combustibles();                       break;
        case 3: calcular_viaje();            pausa();      break;
        case 0:
            guardar_combustibles();
            guardar_vehiculos();
            printf("Datos guardados en %s y %s. Hasta pronto.\n",
                   ARCH_VEHICULOS, ARCH_COMBUSTIBLES);
            return 0;
        }
    }
}
