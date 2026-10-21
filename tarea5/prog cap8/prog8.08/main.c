#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char cal[20];
    int num;
    char col[20];
    char cp[5];
    char ciu[20];
}domicilio;

typedef struct
{
    char nom[20];
    int edad;
    char sexo;
    int con;
    domicilio dom;

    char tel[10];
}paciente;

void lectura(paciente *, int);
void f1 (paciente *, int);
void f2 (paciente *, int);
void f3 (paciente *, int);

int main()
{
    paciente hospital[100];
    int tam;
    do
    {
        printf("\nIngrese el numero de pacientes: ");
        scanf("%d", tam);
    }
    while (tam > 50 || tam < 1);

    lectura(hospital, tam);
    f1(hospital, tam);
    f2(hospital, tam);
    f3(hospital, tam);

    return 0;
}

void lectura(paciente a[], int t)
{
    int i;
    for (i=0; i<t; i++)
    {
        printf("\n\t\tPaciente %d", i+1);
        fflush(stdin);
        printf("\nNombre: ");
        gets(a[i].nom);

        printf("Edad: ");
        scanf("%d", &a[i].edad);
        fflush(stdin);

        printf("Sexo (F-M): ");
        scanf("%c", &a[i].sexo);

        printf("Condición (1..5): ");
        scanf("%d", &a[i].con);
        fflush(stdin);

        printf("\tCalle: ");
        gets(a[i].dom.cal);

        printf("\tNúmero: ");
        scanf("%d", &a[i].dom.num);
        fflush(stdin);

        printf("\tColonia: ");
        gets(a[i].dom.col);
        fflush(stdin);

        printf("\tCódigo Postal: ");
        gets(a[i].dom.cp);
        fflush(stdin);

        printf("\tCiudad: ");
        gets(a[i].dom.ciu);
        fflush(stdin);

        printf("Teléfono: ");
        gets(a[i].tel);
    }
}

void f1(paciente a[], int t)
{
    int i, fem, mas, tot;
    for (i=0; i<t; i++)
    {
        switch (a[i].sexo)
        {
            case 'f': fem++; break;
            case 'm': mas++; break;
        }
    }
    tot = fem + mas;
    printf("\nPorcentaje de Hombres: %.2f%%", (float)mas / tot * 100);
    printf("\nPorcentaje de Mujeres: %.2f%%", (float)fem / tot * 100);
}

void f2(paciente a[], int t)
{
    int i, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0;
    for (i=0; i<t; i++)
    {
        switch (a[i].con)
        {
            case 1: c1++; break;
            case 2: c2++; break;
            case 3: c3++; break;
            case 4: c4++; break;
            case 5: c5++; break;
        }
    }
    printf("\nNumero de pacientes en codicion 1: %d", c1);
    printf("\nNumero de pacientes en codicion 2: %d", c2);
    printf("\nNumero de pacientes en codicion 3: %d", c3);
    printf("\nNumero de pacientes en codicion 4: %d", c4);
    printf("\nNumero de pacientes en codicion 5: %d", c5);
}

void f3(paciente a[], int t)
{
    int i;
    printf("\nPacientes ingresados en estado de gravedad");
    for (i=0; i<t; i++)
    {
        if (a[i].con == 5)
        {
            printf("\nNombre: %s\tTelefono: %s", a[i].nom, a[i].tel);
        }
    }
}
