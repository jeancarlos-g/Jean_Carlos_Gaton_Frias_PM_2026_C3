# Proposal: add-trip-cost-calculator

## Why

No existe una herramienta para saber lo que realmente cuesta usar un vehículo en un viaje concreto. El costo total de mover un vehículo combina rubros fijos anuales (seguro, matrícula, depreciación, etc.) y rubros por kilómetro, y repartir los fijos a mano en cada viaje es laborioso y propenso a error. CasCost resuelve esto: dado un viaje (km y porcentaje en ciudad vs carretera), entrega el costo total del viaje y el costo por km, con desglose por rubro.

## What Changes

- Nuevo programa de consola en lenguaje C con menú principal: Vehículos, Combustibles, Calcular viaje, Salir.
- Nueva capacidad de gestión (CRUD) de vehículos con sus 15 parámetros de costo: nombre, combustible asociado, consumo en ciudad y en carretera, precio de compra, valor de reventa, años de vida útil, seguro anual, matrícula e impuestos anuales, limpieza mensual, imprevistos anuales, mantenimiento por km, neumáticos por km, mantenimiento pesado cada 100.000 km y km estimados por año (parámetro nuevo acordado durante la exploración, necesario para prorratear los costos fijos).
- Nueva capacidad de gestión (CRUD) de tipos de combustible (nombre + precio por litro) y del enlace vehículo-combustible. Los vehículos actuales son a gasolina, pero el modelo acepta cualquier tipo.
- Nueva capacidad de cálculo de viaje: entrada km del viaje y % en ciudad; salida costo total y costo por km con desglose por rubro.
- Nueva persistencia en archivos CSV (texto editable, encabezados en español): `vehiculos.csv` y `combustibles.csv`.
- Depreciación lineal por año: `(precio_compra - valor_reventa) / años_vida`.
- Validaciones de entrada (rangos de km, porcentajes, precios positivos) y protección contra borrar un combustible asignado a un vehículo.

## Capabilities

### New Capabilities

- `vehicle-catalog`: CRUD de vehículos con su configuración completa de costos (fijos anuales, variables por km y consumo por entorno), incluyendo validaciones y persistencia en CSV.
- `fuel-pricing`: CRUD de tipos de combustible con precio por litro, incluyendo validaciones, persistencia en CSV y la regla de integridad con los vehículos que los usan.
- `trip-cost-calculation`: cálculo del costo total y el costo por km de un viaje dado su distancia y porcentaje en ciudad, con desglose por rubro y depreciación lineal anual.

### Modified Capabilities

<!-- Ninguna: proyecto nuevo, no hay specs existentes. -->

## Impact

- Código nuevo en C en este repositorio (no hay código previo): `main.c` (menú), módulos de vehículos, combustibles, cálculo y persistencia CSV.
- Archivos de datos creados en tiempo de ejecución: `vehiculos.csv`, `combustibles.csv`.
- Sin dependencias externas: solo biblioteca estándar de C (stdio, string, stdlib).
- Sin APIs ni sistemas externos.
