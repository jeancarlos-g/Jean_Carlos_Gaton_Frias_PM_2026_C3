# Spec Delta: fuel-pricing

## Purpose

Administrar los tipos de combustible disponibles y su precio por litro, para que los vehículos consuman precios actualizados al momento de calcular un viaje. El modelo acepta cualquier tipo de combustible; los vehículos actuales son a gasolina.

## ADDED Requirements

### Requirement: Alta de tipo de combustible
El sistema SHALL permitir crear un nuevo tipo de combustible con su nombre y su precio por litro.

#### Scenario: Creación exitosa
- **WHEN** el usuario elige "Crear combustible" e ingresa un nombre no vacío y un precio por litro válido
- **THEN** el sistema registra el tipo de combustible y lo muestra en el listado

#### Scenario: Rechazo de nombre vacío o precio inválido
- **WHEN** el usuario ingresa un nombre vacío o un precio por litro negativo
- **THEN** el sistema informa el error y vuelve a pedir el dato sin registrar el combustible

### Requirement: Listado de combustibles
El sistema SHALL listar todos los tipos de combustible registrados con su nombre y precio por litro, indicando claramente cuando no hay ninguno.

#### Scenario: Listar combustibles registrados
- **WHEN** el usuario entra al menú de combustibles con al menos uno registrado
- **THEN** el sistema muestra cada combustible, numerado, con su nombre y precio por litro

### Requirement: Edición de combustible
El sistema SHALL permitir editar el nombre y el precio por litro de un combustible existente.

#### Scenario: Actualización de precio
- **WHEN** el usuario edita el precio por litro de un combustible existente con un valor válido
- **THEN** el sistema actualiza el precio y los cálculos de viajes posteriores usan el nuevo valor

### Requirement: Baja protegida de combustible asignado
El sistema SHALL permitir eliminar un combustible sin vehículos asignados, y SHALL rechazar la eliminación de un combustible que está asignado a al menos un vehículo, informando la regla que impide la baja.

#### Scenario: Baja de combustible sin vehículos asignados
- **WHEN** el usuario elimina un combustible que ningún vehículo está usando
- **THEN** el sistema lo quita del listado

#### Scenario: Intento de baja de combustible asignado
- **WHEN** el usuario intenta eliminar un combustible que tiene uno o más vehículos asignados
- **THEN** el sistema rechaza la operación informando que el combustible está en uso y conserva tanto el combustible como los vehículos que lo referencian

### Requirement: Persistencia de combustibles en CSV
El sistema SHALL guardar los combustibles en un archivo CSV con encabezados en español de modo que los datos de combustibles sobrevivan al cierre y reaparezcan al reabrir el programa.

#### Scenario: Precios disponibles tras reiniciar
- **WHEN** el usuario registra o edita un combustible, cierra el programa y lo vuelve a abrir
- **THEN** los combustibles con sus precios y su enlace a los vehículos siguen disponibles en los menús y en el cálculo de viajes
