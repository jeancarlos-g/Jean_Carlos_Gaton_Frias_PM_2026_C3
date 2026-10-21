#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char calle[20];
    int numero;
    int cp;
    char localidad[20];

}domicilio;

struct empleado
{
    char nombre[20];
    char departamento[20];
    float sueldo;
    domicilio direccionl
};

void lectura (struct empleado *a)
{
    printf("\nIngrese le nombre del empleado: ");
    gets(a->nombre);
    fflush(stdin);
    printf("\nIngrese el departamento de la empresa: ");
    gets(a->departamento);
    printf("\Ingrese el sueldo del empleado: ");
    scanf("%f", &a->sueldo);
    fflush(stdin);
    printf("\nIngrese la direccion del empleado -- - ");
    printf("\n\tCalle: ");
    gets(a->direccion.calle);
    printf("\tNumero: ");
    scanf("%d", &a->direccion.numero);
    printf("\tCodigo postal: ");
    scanf("%d", &a->direccion.cp);
    fflush(stdin);
    printf("\tLocalidad: ");
    gets(a->direccion.localidad);

}

int main()
{
    struct empleado e0 = {"arturo", "compras", 15500.75, "san jeronimo", 120, 3490, "toluca"};
    struct empleado *e1, *e2, e3, e4;

    e1 = new(struct empleado);

    printf("\nIngrese le nombre del empleado 1: ");
    scanf("%s", &(*e1).nombre);
    fflush(stdin);
    printf("\nIngrese el departamento de la empresa: ");
    gets(e1->departamento);
    printf("\Ingrese el sueldo del empleado 1: ");
    scanf("%f", %e1->sueldo);
    fflush(stdin);
    printf("\nIngrese la direccion del empleado 1 -- - ");
    printf("\n\tCalle: ");
    gets(e1->direccion.calle);
    printf("\tNumero: ");
    scanf("%d", &e1->direccion.numero);
    printf("\tCodigo postal: ");
    scanf("%d", &e1->direccion.cp);
    fflush(stdin);
    printf("\tLocalidad: ");
    fflush(stdin);
    gets(e1->direccion.localidad);

    printf("\nIngrese le nombre del empleado 3: ");
    scanf("%s", &(*e3).nombre);
    fflush(stdin);
    printf("\nIngrese el departamento de la empresa: ");
    gets(e3->departamento);
    printf("\Ingrese el sueldo del empleado 3: ");
    scanf("%f", %e3->sueldo);
    fflush(stdin);
    printf("\nIngrese la direccion del empleado 3 -- - ");
    printf("\n\tCalle: ");
    gets(e3->direccion.calle);
    printf("\tNumero: ");
    scanf("%d", &e3->direccion.numero);
    printf("\tCodigo postal: ");
    scanf("%d", &e3->direccion.cp);
    fflush(stdin);
    printf("\tLocalidad: ");
    fflush(stdin);
    gets(e3->direccion.localidad);

    e2 = new(struct empleado);
    lectura(e2);
    lectura(&e4);

    printf("\nDatos del empleado 1\n");
    printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e1->nombre, e1->departamento, e1->sueldo, e1->direccion.calle, e1->direccion.numero, e1->direccion.cp, e1->localidad);

    printf("\nDatos del empleado 4\n");
     printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e4->nombre, e4->departamento, e4->sueldo, e4->direccion.calle, e4->direccion.numero, e4->direccion.cp, e4->localidad);
    return 0;
}
