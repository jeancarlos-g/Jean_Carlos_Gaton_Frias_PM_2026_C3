# Spec Delta: vehicle-catalog

## Purpose

Administrar la colección de vehículos del usuario con su configuración completa de costos (fijos anuales, variables por kilómetro y consumo por entorno), para que la calculadora de viajes pueda prorratear los costos fijos y computar los variables por km de cada vehículo registrado.

## ADDED Requirements

### Requirement: Alta de vehículo con configuración completa de costos
El sistema SHALL permitir crear un nuevo vehículo pidiendo y guardando sus 15 parámetros: nombre, combustible asociado, consumo en ciudad (l/100km), consumo en carretera (l/100km), precio de compra, valor de reventa, años de vida útil, seguro anual, matrícula e impuestos anuales, limpieza mensual, imprevistos anuales, mantenimiento por km, neumáticos por km, mantenimiento pesado cada 100.000 km y km estimados por año.

#### Scenario: Creación exitosa
- **WHEN** el usuario elige "Crear vehículo" y completa los 15 parámetros con valores válidos
- **THEN** el sistema registra el vehículo y lo muestra en el listado de vehículos

#### Scenario: Selección de combustible de la lista
- **WHEN** el usuario crea o edita un vehículo
- **THEN** el sistema le ofrece elegir el combustible asociado entre los tipos de combustible existentes y no acepta el alta sin una selección válida

### Requirement: Listado de vehículos
El sistema SHALL listar todos los vehículos registrados con su nombre y un resumen de sus parámetros de costo, indicando claramente cuando no hay vehículos registrados.

#### Scenario: Listar con vehículos existentes
- **WHEN** el usuario entra al menú de vehículos habiendo registrado al menos uno
- **THEN** el sistema muestra cada vehículo registrado, numerado, con su nombre y resumen de parámetros

#### Scenario: Sin vehículos registrados
- **WHEN** el usuario entra al menú de vehículos sin haber registrado ninguno
- **THEN** el sistema informa que no hay vehículos registrados

### Requirement: Edición de vehículo
El sistema SHALL permitir editar cualquiera de los 15 parámetros de un vehículo existente, respetando las mismas validaciones que el alta.

#### Scenario: Edición exitosa
- **WHEN** el usuario elige un vehículo existente para editar y modifica sus parámetros con valores válidos
- **THEN** el sistema actualiza el vehículo y los valores editados se reflejan en el listado y en los cálculos posteriores

### Requirement: Baja de vehículo
El sistema SHALL permitir eliminar un vehículo registrado por su posición en el listado, y SHALL confirmar la baja antes de eliminarlo.

#### Scenario: Baja con confirmación previa
- **WHEN** el usuario elige eliminar un vehículo registrado
- **THEN** el sistema solicita confirmación y al confirmar lo quita del listado y deja de considerarlo en los cálculos

#### Scenario: Baja cancelada
- **WHEN** el usuario elige eliminar un vehículo y no confirma
- **THEN** el sistema conserva el vehículo sin cambios

### Requirement: Validación de parámetros del vehículo
El sistema SHALL rechazar valores inválidos en los parámetros del vehículo: km del alcance anual, años de vida útil y consumos por entorno deben ser mayores que cero; todos los costos deben ser mayores o iguales que cero; y el nombre del vehículo no puede ser vacío.

#### Scenario: Rechazo de valores fuera de rango
- **WHEN** el usuario ingresa un valor inválido (por ejemplo, km estimados por año igual a cero, o un costo negativo)
- **THEN** el sistema informa el error y vuelve a pedir el dato sin registrar el vehículo

### Requirement: Persistencia de vehículos en CSV
El sistema SHALL guardar los vehículos en un archivo CSV con encabezados en español de modo que los datos de vehículos sobrevivan al cierre y reaparezcan al reabrir el programa.

#### Scenario: Datos disponibles tras reiniciar
- **WHEN** el usuario registra un vehículo, cierra el programa y lo vuelve a abrir
- **THEN** el vehículo registrado sigue disponible en el menú de vehículos y en el cálculo de viajes
