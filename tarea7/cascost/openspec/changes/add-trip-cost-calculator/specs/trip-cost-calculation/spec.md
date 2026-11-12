# Spec Delta: trip-cost-calculation

## Purpose

Calcular el costo de un viaje en un vehículo dado: el usuario ingresa la distancia en km y el porcentaje del viaje en ciudad, y el sistema entrega el costo total del viaje y el costo por km, con desglose por rubro, combinando costos fijos anuales prorrateados y costos variables por km.

## ADDED Requirements

### Requirement: Entrada y validación del viaje
El sistema SHALL pedir para cada viaje el vehículo a usar, la distancia del viaje en km y el porcentaje del viaje en ciudad, y SHALL rechazar distancias menores o iguales que cero y porcentajes fuera del rango 0 a 100.

#### Scenario: Viaje válido
- **WHEN** el usuario selecciona un vehículo registrado y ingresa una distancia mayor que cero con un porcentaje entre 0 y 100
- **THEN** el sistema calcula y muestra el resultado del viaje

#### Scenario: Rechazo de datos inválidos
- **WHEN** el usuario ingresa una distancia igual a cero o un porcentaje negativo o mayor que 100
- **THEN** el sistema informa el error y vuelve a pedir el dato sin calcular el viaje

#### Scenario: Sin vehículos disponibles
- **WHEN** el usuario entra al cálculo de viajes sin ningún vehículo registrado
- **THEN** el sistema informa que primero debe registrar un vehículo y no pide los datos del viaje

### Requirement: Costo de combustible por entorno
El sistema SHALL calcular el consumo de combustible aplicando el consumo en ciudad del vehículo a la parte del viaje en ciudad y el consumo en carretera a la parte restante, y SHALL valorar los litros al precio por litro del combustible asociado al vehículo.

#### Scenario: Viaje mixto valorizado al precio vigente
- **WHEN** un vehículo con consumo de 10 l/100km en ciudad y 6 l/100km en carretera, asociado a un combustible de 1,50 por litro, recorre 100 km con 50% en ciudad
- **THEN** el sistema calcula 8 litros y reporta 12,00 de combustible

### Requirement: Costos variables por kilómetro
El sistema SHALL sumar al viaje los costos variables proporcionales a la distancia: mantenimiento y neumáticos multiplicados por los km del viaje, y el mantenimiento pesado amortizado en sus 100.000 km de vida.

#### Scenario: Variables de un viaje de 100 km
- **WHEN** un vehículo tiene mantenimiento de 0,05 por km, neumáticos de 0,03 por km y mantenimiento pesado de 300 cada 100.000 km, y el viaje es de 100 km
- **THEN** el sistema reporta 5,00 de mantenimiento, 3,00 de neumáticos y 0,30 de mantenimiento pesado

### Requirement: Prorrateo de costos fijos anuales
El sistema SHALL sumar los costos fijos anuales del vehículo (seguro, matrícula e impuestos, limpieza multiplicada por 12 e imprevistos, más la depreciación lineal anual definida como (precio de compra − valor de reventa) dividido por los años de vida útil) y SHALL asignar al viaje la fracción de esa suma correspondiente a los km del viaje sobre los km estimados por año del vehículo.

#### Scenario: Fijos amortizados al viaje
- **WHEN** un vehículo con costos fijos anuales de 1.440,00 y 12.000 km estimados por año recorre un viaje de 100 km
- **THEN** el sistema asigna 12,00 de costos fijos al viaje

### Requirement: Reporte de resultado con desglose
El sistema SHALL mostrar para cada viaje calculado el costo de cada rubro (combustible, mantenimiento, neumáticos, mantenimiento pesado y costos fijos amortizados), el costo total del viaje y el costo por km, definido como el costo total dividido por la distancia del viaje.

#### Scenario: Desglose completo del viaje
- **WHEN** el sistema calcula un viaje de 100 km con 50% en ciudad cuyos rubros suman combustible 12,00, mantenimiento 5,00, neumáticos 3,00, mantenimiento pesado 0,30 y costos fijos 12,00
- **THEN** el sistema muestra cada rubro, un costo total de 32,30 y un costo por km de 0,3230
