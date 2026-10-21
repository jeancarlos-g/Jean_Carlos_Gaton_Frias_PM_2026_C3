#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct alumno
{
    int matricula;
    char nombre[20];
    char carrera[20];
    float promedio;
    char direccion[20];
};

void lectura (struct alumno *);

int main()
{
    struct alumno a0 = {120, "maria", "contabilidad", 8.9, "queretaro"};
    struct alumno *a3, *a4, *a5, *a6;

    a3 = &a0;
    a4 = new (struct alumno);


    printf("\nIngrese la matricula del alumno 4: ");
    scanf("%d", &(*a4).matricula);
    fflush(stdin);
    printf("\nIngrese el nombre del alumno 4: ");
    gets(a4->nombre);
    printf("\nIngrese la carrera del alumno 4: ");
    gets((*a4).carrera);
    printf("\nIngrese el promedio del alumno 4: ");
    scanf("%f", &a4->promedio);
    fflush(stdin);
    printf("\nIngrese la direccion del alumno 4: ");
    gets(a4->direccion);

    a5 = new (struct alumno);
    printf("\nDatos del alumno 3\n");
    printf("%d\t%s\t%.2f\t%s", a3->matricula, a3->nombre, a3->carrera, a3->promedio, a3->direccion);

    printf("\nDatos del alumno 4\n");
    printf("%d\t%s\t%.2f\t%s", a4->matricula, a4->nombre, a4->carrera, a4->promedio, a4->direccion);

    printf("\nDatos del alumno 5\n");
    printf("%d\t%s\t%.2f\t%s", a5->matricula, a5->nombre, a5->carrera, a5->promedio, a5->direccion);

    printf("\nDatos del alumno 3\n");
    printf("%d\t%s\t%.2f\t%s", a6.matricula, a6.nombre, a6.carrera, a6.promedio, a6.direccion);


    return 0;
}

void lectura(struct alumno)
{
    printf("\nIngrese la matricula del alumno: ");
    scanf("%d" &(*a).matricula);
    fflush(stdin);
    printf("\nIngrese el nombre del alumno: ");
    gets(a->nombre);
    printf("\nIngrese la carrera del alumno: ");
    gets((*a).carrera);
    printf("\nIngrese el promedio del alumno: ");
    scanf("%f" &a->promedio);
    fflush(stdin);
    printf("\nIngrese la direccion del alumno: ");
    gets(a->direccion);
}
