#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char zona[20];
    char calle[20];
    char colo[20];
}ubicacion;

typedef struct
{
    char clave[5];
    float scu;
    float ste;
    char car[50];
    ubicacion ubi;

    float precio;
    char dispo;
}propiedades;

void lectura(propiedades *, int);
void f1(propiedades *, int);
void f2(propiedades *, int);

int main()
{
    propiedades propie[100];
    int tam;
    do
    {
        printf("\nIngrese el numero de propiedades: ");
        scanf("%d", &tam);
    }
    while (tam > 100 || tam < 1);
    lectura(propie, tam);
    f1(propie, tam);
    f2(propie, tam);
    return 0;
}

void lectura(propiedades a[], int t)
{
    int i;
    for (i=0; i<t; i++)
    {
        printf("\n\tIngrese datos de la propiedad %d", i + 1);

        printf("\nClave: ");
        fflush(stdin);
        gets(a[i].clave);

        printf("Superficie cubierta: ");
        scanf("%f", &a[i].scu);

        printf("Superficie terreno: ");
        scanf("%f", &a[i].ste);

        printf("Características: ");
        fflush(stdin);
        gets(a[i].car);

        printf("\tZona: ");
        fflush(stdin);
        gets(a[i].ubi.zona);

        printf("\tCalle: ");
        fflush(stdin);
        gets(a[i].ubi.calle);

        printf("\tColonia: ");
        fflush(stdin);
        gets(a[i].ubi.colo);

        printf("Precio: ");
        scanf("%f", &a[i].precio);

        printf("Disponibilidad (Venta-V Renta-R): ");
        fflush(stdin);
        scanf("%c", &a[i].dispo);
    }
}

void f1(propiedades a[], int t)
{
    int i;
    printf("\n\t\tListado de propiedades para ventas en miraflores");
    for (i=0; i<t; i++)
    {
        if ((a[i].dispo == 'v') && (strcmp(a[i],ubi.zona, "miraflores") == 0))
        {
            if ((a[i].precio >= 450000) && (a[i].precio <= 650000))
            {
                printf("\nClave de la propiedad: ");
                puts(a[i].clave);
                printf("\nSuperficie cubierta: %.2f", a[i].scu);
                printf("\nSuperficie terreno: %.2f", a[i].ste);
                printf("\nCaracterísticas: ");
                puts(a[i].car);
                printf("Calle: ");
                puts(a[i].ubi.calle);
                printf("Colonia: ");
                puts(a[i].ubi.colo);
                printf("Precio: %.2f\n", a[i].precio);
            }
        }

        void f2(propiedades)
        {
            int i;
            float li, ls;
            char zon[20];
            printf("\nListado de personas para renta");
            printf("\nIngrese zona geografica: ");
            fflush(stdin);
            gets(zon);
            printf("\nIngrese el limete inferior del precio: ");
            scanf("%f", &li);
            printf("\nIngrese el limete superior del precio: ");
            scanf("%f", &ls);
            for (i=0; i<t; i++)
            {
                if ((a[i].dispo == 'r') && (strcmp(a[i],ubi.zona, zon) == 0))
                {
                   if ((a[i].precio >= li) && (a[i].precio <= ls))
                   {
                       printf("\nClave de la propiedad: ");
                       puts(a[i].clave);
                       printf("\nSuperficie cubierta: %.2f", a[i].scu);
                       printf("\nSuperficie terreno: %.2f", a[i].ste);
                       printf("\nCaracterísticas: ");
                       puts(a[i].car);
                       printf("Calle: ");
                       puts(a[i].ubi.calle);
                       printf("Colonia: ");
                       puts(a[i].ubi.colo);
                       printf("Precio: %.2f", a[i].precio);
                   }
                }
            }
        }
    }
}
