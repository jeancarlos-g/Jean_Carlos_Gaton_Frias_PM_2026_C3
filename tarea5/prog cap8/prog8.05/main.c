#include <stdio.h>
#include <stdlib.h>
#include <string.h>

union datos
{
    char celular[15];
    char correo[20];
};

typedef struct
{
    int matricula;
    char nombre[20];
    char carrera[20];
    float promedio;
    union datos personales;

}alumno;

void lectura(alumno a);

int main()
{
    alumno a1 = {120, "maria", "contabilidad", 8.9, "5-158-40-50"}, a2, a3;
    printf("alumno 2\n");
    printf("\nIngrese la matricula: ");
    scanf("&d", &a2.matricula);
    fflush(stdin);
    printf("\nIngrese el nombre: ");
    gets(a2.nombre);
    fflush(stdin);
    printf("\nIngrese carrera: ");
    gets(a2.carrera);
    printf("\nIngrese promedio: ");
    scanf("%f", &a2.promedio);
    fflush(stdin);
    printf("\nIngrese el correo electronico: ");
    gets(a2.personales.correo);

    printf("alumno 3\n");

    printf("\nDatos del alumno 1\n");
    printf("%d\n", a1.matricula);
    puts(a1.nombre);
    puts(a1.carrera);
    printf("%.2f\n", a1.promedio);
    puts(a1.personales.celular);
    puts(a1.personales.correo);

    strcpy(a0.personales.correo, "hgimenez@hotmail.com");
    puts(a0.personales.celular);
    puts(a0.personales.correo);

    printf("\nDatos del alumno 2\n");
    printf("%d\n", a2.matricula);
    puts(a2.nombre);
    puts(a2.carrera);
    printf("%.2f\n", a2.promedio);
    puts(a2.personales.celular);
    puts(a2.personales.correo);

    printf("\nIngrese el telefono celular del alumno 2: ");
    fflush(stdin);
    gets(a2.personales.celular);

    puts(a2.personales.celular);
    puts(a2.personales.correo);

    printf("\nDatos del alumno 3\n");
    printf("%d\n", a3.matricula);
    puts(a3.nombre);
    puts(a3.carrera);
    printf("%.2f\n", a3.promedio);
    puts(a3.personales.celular);
    puts(a3.personales.correo);

    return 0;
}

void lectura(alumno *a)
{
    printf("\nIngrese la matricula: ");
    scanf("&d", &(*a).matricula);
    fflush(stdin);
    printf("\nIngrese el nombre: ");
    gets(a->nombre);
    fflush(stdin);
    printf("\nIngrese carrera: ");
    gets(a->carrera);
    printf("\nIngrese promedio: ");
    scanf("%f", &(*a).promedio);
    fflush(stdin);
    printf("\nIngrese el telefono celular: ");
    gets(a->personales.celular);
}
