# AutoCrédito — Gestión de préstamos para compra de vehículos

Aplicación de escritorio escrita en **C** con interfaz gráfica **GTK 3**. Administra
marcas y modelos de vehículos, clientes, préstamos, cuotas mensuales, abonos y
genera un reporte de estado de pago. Toda la información se guarda en un archivo
**JSON**.

## Funcionalidades

| Requisito | Dónde se cumple |
|---|---|
| a. Gestión de marca y modelo de vehículo | Pestaña **Vehículos** |
| b. Clientes: agregar, buscar y borrar | Pestaña **Clientes** (búsqueda por nombre, cédula o correo) |
| c. Listas enlazadas | `src/model.h` / `src/model.c` |
| d. Memoria dinámica | `g_new0`, `g_strdup`, `g_free` en todo el modelo |
| e. Archivos JSON | `src/json.c` (motor propio) y `src/store.c` |
| f. Pagos de abono | Pestaña **Pagos**, panel *Abonos* |
| g. Gestión de pagos mensuales | Pestaña **Pagos** (marcar pagada / pendiente) |
| h. Reporte de estado de pago | Pestaña **Reporte** (meses, pagados/no pagados y saldo) |

El reporte muestra:

1. La lista completa de meses del préstamo.
2. Cada cuota marcada como **Pagada** (verde) o **No pagada** (rojo).
3. El saldo pendiente total y el saldo restante después de cada mes.

## Requisitos

- Compilador C (GCC o Clang)
- `make`
- `pkg-config`
- GTK 3 para desarrollo

### Windows (MSYS2, recomendado)

1. Instalar [MSYS2](https://www.msys2.org/).
2. Abrir la terminal **MSYS2 MINGW64** y ejecutar:

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-pkgconf \
  mingw-w64-x86_64-gtk3 make
```

### Linux (Debian/Ubuntu)

```bash
sudo apt install build-essential pkg-config libgtk-3-dev
```

### macOS (Homebrew)

```bash
brew install gtk+3 pkg-config
```

## Compilar y ejecutar

Desde la carpeta del proyecto:

```bash
make          # genera build/autocredito (o autocredito.exe en Windows)
make run      # compila y abre la aplicación
```

En Windows también se puede hacer doble clic en `ejecutar.bat`, que configura
las DLL de MSYS2 y compila si hace falta.

> El ejecutable de Windows necesita las DLL de GTK que están en
> `C:\msys64\mingw64\bin`. Si se ejecuta fuera de MSYS2, agregar esa carpeta al
> `PATH` (el lanzador `ejecutar.bat` ya lo hace).

### Datos de ejemplo

El proyecto incluye `data/ejemplo.json` con 3 marcas, 3 clientes y 3 préstamos
en distintos estados (cuotas pagadas, pendientes y un abono). Para abrirlo:

```bash
make ejemplo
```

o directamente:

```bash
./build/autocredito --data data/ejemplo.json
```

Así se puede probar el reporte sin cargar datos a mano. Los cambios que se
hagan sobre ese archivo también se guardan en él.

## Cómo usarla

1. **Vehículos**: agregue una marca (por ejemplo *Toyota*) y luego sus modelos
   con un precio de referencia (por ejemplo *Corolla*, 25000).
2. **Clientes**: registre nombre y cédula. Use el buscador para filtrar.
3. **Préstamos**: elija cliente, marca y modelo; complete precio, inicial, tasa
   anual, plazo y fecha de inicio. Al crear el préstamo se genera
   automáticamente la tabla de cuotas con amortización francesa.
4. **Pagos**:
   - Seleccione un préstamo en la lista superior.
   - Seleccione una cuota y use **Marcar como pagada** o **Marcar como pendiente**.
   - Registre abonos indicando monto, fecha y una nota opcional.
5. **Reporte**: seleccione el préstamo para ver el resumen y el estado mes a mes.
   La columna *Saldo tras cuota* es el saldo programado después de esa cuota; el
   saldo real (que descuenta los abonos) aparece en el resumen superior.

Los cambios se guardan automáticamente en `data/datos.json` después de cada
operación y también al cerrar la ventana.

### Opciones de línea de comandos

```text
--data <ruta>   Usa otro archivo JSON de datos
--selftest      Ejecuta las pruebas internas sin abrir la interfaz
--help          Muestra la ayuda
```

También se puede definir la variable de entorno `PRESTAMOS_DATA`.

## Formato de los datos

`data/datos.json` tiene esta estructura (resumida):

```json
{
  "version": 1,
  "next_ids": { "marca": 2, "modelo": 2, "cliente": 2, "prestamo": 2, "abono": 2 },
  "marcas": [
    {
      "id": 1,
      "nombre": "Toyota",
      "modelos": [{ "id": 1, "nombre": "Corolla", "precio": 25000 }]
    }
  ],
  "clientes": [
    {
      "id": 1,
      "nombre": "Ana Pérez",
      "cedula": "001-010101-0001A",
      "telefono": "8888-1111",
      "email": "ana@example.com"
    }
  ],
  "prestamos": [
    {
      "id": 1,
      "cliente_id": 1,
      "marca_id": 1,
      "modelo_id": 1,
      "vehiculo": "Toyota Corolla",
      "precio": 25000,
      "inicial": 5000,
      "financiado": 20000,
      "tasa_anual": 12,
      "plazo": 24,
      "fecha_inicio": "2026-01-15",
      "cuotas": [
        {
          "numero": 1,
          "mes": "2026-02",
          "monto": 941.47,
          "capital": 741.47,
          "interes": 200,
          "pagada": false,
          "fecha_pago": ""
        }
      ],
      "abonos": [
        { "id": 1, "fecha": "2026-02-01", "monto": 1000, "nota": "Abono extra" }
      ]
    }
  ]
}
```

## Estructura del proyecto

```text
prestamos-vehiculos/
├── Makefile
├── README.md
├── ejecutar.bat
├── data/                 # datos.json se crea al primer cambio
└── src/
    ├── main.c            # punto de entrada, CLI y pruebas internas
    ├── model.h / model.c # listas enlazadas, cuotas y cálculos
    ├── json.h / json.c   # lector y escritor JSON propio
    ├── store.h / store.c # carga y guardado del archivo
    ├── ui.h / ui.c       # ventana y pestañas GTK
    └── util.h / util.c   # formato de dinero y validaciones
```

## Detalles técnicos

### Listas enlazadas y memoria dinámica

```text
Marca ──► Modelo ──► ...
  │
  └── sig ──► Marca ...

Cliente ──► sig ──► ...

Prestamo ──► Cuota ──► Cuota ──► ...
    │
    └──────► Abono ──► Abono ──► ...
```

Cada nodo se reserva con `g_new0` y sus textos con `g_strdup`; se liberan con
`g_free` en `app_clear` / `app_free` y en las operaciones de borrado.

### Cálculo de cuotas

Se usa amortización francesa (cuota fija):

```text
i = tasa_anual / 100 / 12
cuota = financiado * i / (1 - (1 + i)^-plazo)
interés del mes = saldo * i
capital del mes = cuota - interés
```

La última cuota se ajusta para que la suma del capital sea exactamente el monto
financiado.

### Saldo y abonos

```text
total programado = suma de todas las cuotas
pagado           = cuotas marcadas como pagadas + abonos
saldo            = total programado - pagado   (nunca menor que cero)
```

Un abono se acredita de inmediato contra el saldo; el calendario de cuotas se
mantiene y cada cuota se sigue marcando individualmente.

### JSON

El motor JSON es propio (sin dependencias adicionales a GTK/GLib): soporta
objetos, arreglos, cadenas con escapes y `\uXXXX`, números, booleanos y `null`.
El guardado es atómico: escribe un archivo temporal y luego lo renombra.

## Pruebas

```bash
make test
```

Ejecuta 24 comprobaciones sobre el modelo y la persistencia: creación de
marcas/modelos/clientes, generación de 24 cuotas, suma de capital, abonos,
marcado de cuotas, interpretación de montos, guardado, carga y conservación de
saldos. Usa un archivo temporal y no toca `data/datos.json`.

## Limitaciones conocidas

- No hay edición de marcas, modelos ni clientes (solo agregar y eliminar).
- El abono no recalcula el interés futuro: se descuenta del saldo total.
- La validación de fechas comprueba el formato y rangos básicos, no días por mes.
- Una sola moneda (se muestra `$`).
