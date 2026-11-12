# CasCost

Calculadora de costos de uso de un vehículo por viaje, en C.
Dado un viaje (distancia en km y porcentaje en ciudad vs carretera),
calcula el costo total del viaje y el costo por km, con desglose por
rubro: combustible, mantenimiento, neumáticos, mantenimiento pesado
amortizado y costos fijos anuales (seguro, matrícula e impuestos,
limpieza, imprevistos y depreciación lineal por año).

## Compilación y ejecución

Con GCC (MinGW/MSYS2 en Windows, gcc en Linux/macOS):

```
gcc -O2 -o cascost main.c
```

Para ejecutar (Windows: `cascost.exe`; Linux/macOS: `./cascost`):

```
./cascost
```

No hay dependencias externas: solo la biblioteca estándar de C.

## Uso

El menú principal ofrece cuatro opciones:

```
=============== CAS COST ===============
1) Vehiculos
2) Combustibles
3) Calcular viaje
0) Salir
=========================================
```

- **Vehículos**: crear, listar, editar y eliminar vehículos. Cada
  vehículo se configura con 15 parámetros:
  1. Nombre.
  2. Combustible asociado (elegido de la lista de combustibles).
  3. Consumo en ciudad, en l/100 km.
  4. Consumo en carretera, en l/100 km.
  5. Precio de compra.
  6. Valor de reventa.
  7. Años de vida útil: define la depreciación lineal anual,
     calculada como (precio de compra − valor de reventa) / años.
  8. Costo de seguro por año.
  9. Matrícula e impuestos por año.
  10. Costo de limpieza por mes.
  11. Costo de imprevistos por año.
  12. Costo de mantenimiento por km.
  13. Costo de neumáticos por km.
  14. Monto del mantenimiento pesado, que se amortiza cada
      100.000 km.
  15. Km estimados por año: sirve para prorratear los costos fijos
      anuales al viaje (fijos anuales × km del viaje / km por año).
- **Combustibles**: crear, listar, editar y eliminar tipos de
  combustible (nombre y precio por litro).
  - No se puede eliminar un combustible que esté asignado a un
    vehículo.
  - Decimales: puede escribir 24,50 o 24.50.
- **Calcular viaje**: se pide el vehículo, la distancia en km y el
  porcentaje del viaje en ciudad (0 a 100). El reporte incluye el
  desglose por rubro, el costo total y el costo por km.
- **Salir**: guarda los datos y termina.

Los datos se guardan automáticamente tras cada alta, edición o baja,
y también al salir con la opción 0.

## Archivos de datos

- `combustibles.csv`: un combustible por línea, separador `;`:
  `id;nombre;precio_litro`
- `vehiculos.csv`: un vehículo por línea, separador `;`:
  `nombre;id_combustible;consumo_ciudad_l_100km;consumo_carretera_l_100km;precio_compra;valor_reventa;anios_vida;seguro_anual;matricula_impuestos_anual;limpieza_mensual;imprevistos_anual;mantenimiento_km;neumaticos_km;mantenimiento_pesado_100000km;km_anuales`

Ambos archivos se crean con encabezados en español la primera vez que
se ejecuta el programa; si `combustibles.csv` no existe, se siembran
dos combustibles de ejemplo (Gasolina y Diésel). Son texto plano y se
pueden editar a mano (usando `;` como separador y punto como decimal).

## Límites

- Máximo 100 vehículos y 50 combustibles.
- El porcentaje en ciudad se interpreta con decimales (por ejemplo,
  37.5); el resto del viaje se calcula como carretera.
