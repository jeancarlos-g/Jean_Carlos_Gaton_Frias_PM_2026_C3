# Tasks

## 1. Estructura del proyecto y persistencia CSV

- [x] 1.1 Crear el proyecto en C: `main.c` con esqueleto de menú principal (Vehículos, Combustibles, Calcular viaje, Salir) que compile con un compilador de C y muestre el menú en español; verificar compilando y recorriendo las opciones sin datos
- [x] 1.2 Definir las estructuras de datos de Vehículo (15 parámetros) y Combustible (nombre, precio por litro), con sus listas dinámicas o arreglos y contadores; verificar con una función de prueba que crea e imprime un registro en memoria
- [x] 1.3 Implementar la carga y el guardado de `vehiculos.csv` y `combustibles.csv` con encabezados en español (creación del archivo si no existe, reescritura completa al guardar); verificar con un ciclo: guardar, cerrar, reabrir y comprobar los mismos datos en los menús

## 2. Menú de combustibles

- [x] 2.1 Implementar alta de combustible con validación de nombre no vacío y precio mayor o igual que cero; verificar intentando dar de alta con nombre vacío y con precio negativo (ambos rechazados y re-pedido) y con datos válidos (queda en el listado)
- [x] 2.2 Implementar listado numerado de combustibles con mensaje claro cuando no hay ninguno; verificar con cero, uno y varios combustibles
- [x] 2.3 Implementar edición de nombre y precio de un combustible existente; verificar que el nuevo precio se refleja en el listado
- [x] 2.4 Implementar baja de combustible con regla de protección: rechazar la baja de un combustible asignado a algún vehículo informando el motivo, y permitir la baja cuando no hay vehículos asignados; verificar ambos casos
- [x] 2.5 Cargar combustibles de ejemplo (Gasolina, Diésel) la primera vez que el archivo no exista, para que el alta de vehículos siempre tenga opciones; verificar en la primera ejecución

## 3. Menú de vehículos

- [x] 3.1 Implementar alta de vehículo pidiendo los 15 parámetros con selección de combustible de la lista existente; verificar el alta completa y la aparición en el listado
- [x] 3.2 Implementar validación de los parámetros del vehículo (km estimados por año, años de vida útil y consumos mayores que cero; costos mayores o iguales que cero; nombre no vacío); verificar con valores inválidos (cero, negativos, nombre vacío) y con un caso límite de porcentaje dentro de rango
- [x] 3.3 Implementar listado numerado de vehículos con nombre y resumen de parámetros, con mensaje claro cuando no hay ninguno; verificar con cero y varios vehículos
- [x] 3.4 Implementar edición de cualquiera de los 15 parámetros con las mismas validaciones del alta; verificar que los valores editados se reflejan en el listado y en la persistencia
- [x] 3.5 Implementar baja de vehículo con confirmación previa y cancelación; verificar que la baja confirmada quita el vehículo y que la cancelada lo conserva

## 4. Menú de cálculo de viaje

- [x] 4.1 Implementar la entrada del viaje (selección de vehículo, km del viaje, porcentaje en ciudad) con rechazo de distancia menor o igual que cero y de porcentaje fuera de 0 a 100, y aviso si no hay vehículos registrados; verificar los rechazos y el aviso
- [x] 4.2 Implementar el cálculo de combustible por entorno (consumo en ciudad aplicado a la parte en ciudad y consumo en carretera al resto, valorizado al precio por litro del combustible del vehículo); verificar con el ejemplo del spec: 100 km, 50% ciudad, 10/6 l/100km, 1,50 por litro → 12,00
- [x] 4.3 Implementar los costos variables por km (mantenimiento y neumáticos por los km del viaje, más mantenimiento pesado amortizado cada 100.000 km); verificar con el ejemplo del spec: 0,05 / 0,03 / 300 cada 100.000 km en 100 km → 5,00 / 3,00 / 0,30
- [x] 4.4 Implementar el prorrateo de costos fijos anuales (seguro + matrícula + limpieza x 12 + imprevistos + depreciación lineal anual por (compra − reventa) / años de vida) al viaje según km del viaje / km anuales; verificar con el ejemplo del spec: fijos 1.440,00 y 12.000 km/año en un viaje de 100 km → 12,00
- [x] 4.5 Implementar el reporte del viaje con desglose por rubro, costo total y costo por km; verificar con el ejemplo integral del spec: 100 km, 50% ciudad → total 32,30 y 0,3230 por km

## 5. Integración y cierre

- [x] 5.1 Verificar el flujo completo de extremo a extremo: alta de combustible, alta de vehículo, cálculo de viaje, cierre y persistencia de ambos CSV, y reapertura mostrando los mismos datos con cálculos idénticos
- [x] 5.2 Recorrer todos los menús buscando fugas de memoria y de manejadores de archivo y confirmar que guardar ocurre al salir y tras cada alta/baja/edición; verificar que no queda ningún archivo abierto al terminar el programa
- [x] 5.3 Redactar el `README.md` en español con instrucciones de compilación, uso de los menús y descripción de los archivos CSV; verificar que las instrucciones de compilación funcionan tal como están escritas

## Workflow follow-up

- Archive the change once implementation and review are complete.
