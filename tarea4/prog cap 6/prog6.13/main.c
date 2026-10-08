#include <stdio.h>
#include <stdlib.h>

const int mes = 12;
const int dep = 3;
const int anio = 8;

void lectura(float [mes][dep][anio], int, int, int);
void funcion1(float [mes][dep][anio], int, int, int);
void funcion2(float [mes][dep][anio], int, int, int);
void funcion3(float [mes][dep][anio], int, int, int);

int main()
{
    float pro[mes][dep][anio];
    lectura(pro, mes, dep, anio);
    funcion1(pro, mes, dep, 2);
    funcion2(pro, mes, dep, anio);
    funcion3(pro, mes, dep, anio);
    return 0;
}

void lectura(float a[][dep][anio], int f, int c, int p)
{
    int k, i, j;
    for (k=0; k<p; k++)
    {
        for (i=0; i<f; i++)
        {
            for (j=0; j<c; j++)
            {
                printf("\nAño: %d\tMes: %d\tDepartamento: %d", k+1, i+1, j+1);
                scanf("%f", &a[i][j][k]);
            }
        }
    }
}

void funcion1(float a[][dep][anio], int f, int c, int p)
{

    int i, j;
    float sum = 0.0;
    for (i=0; i<f; i++)
    {
        for (j=0; j<c; j++)
        {
            sum += a[i][j][p-1];
        }
    }
    printf("\n\nVentas totales de la empresa en el segundo año: %.2f", sum);

}


void funcion2(float a[][dep][anio], int f, int c, int p)
{
    int i, j;
    float sum1 = 0, sum2 = 0, sum3 = 0;
    for (i=0; i<f; i++)
    {
        for (j=0; j<c; j++)
        {
             switch (j+1)
             {
                 case 1: sum1 += a[i][j][p-1]; break;
                 case 2: sum2 += a[i][j][p-1]; break;
                 case 3: sum3 += a[i][j][p-1]; break;

             }
        }
        if (sum1 > sum2)
        {
            if (sum1 > sum3)
            {
                printf("\n\nDepartamento con mayores ventas en el ultimo año: Helios");
                printf("\nVentas: %.2f", sum1);
            }
            else
            {
               printf("\n\nDepartamento con mayores ventas en el ultimo año: Licra");
               printf("\nVentas: %.2f", sum3);
            }
        }
        else
        {
            if (sum2 > sum3)
            {
                printf("\n\nDepartamento con mayores ventas en el ultimo año: Lanas");
                printf("\nVentas: %.2f", sum2);
            }
            else
            {
               printf("\n\nDepartamento con mayores ventas en el ultimo año: Licra");
               printf("\nVentas: %.2f", sum3);
            }
        }

    }

}

void funcion3(float a[][dep][anio], int f, int c, int p)
{
    int k, i, j, de, me, an;
    float ven = -1.0;
    for (k=0; k<p; k++)
    {
        for (i=0; i<f; i++)
        {
            for (j=0; j<c; j++)
            {
                if (a[i][j][k] > ven)
                {
                    ven = a[i][j][k];
                    de = j;
                    me = i;
                    an = k;
                }
            }
        }

    }
    printf("\n\nDepartamento: %d\tMes: %d\tAño: %d", de+1, me+1, an+1);
    printf("\tVentas: %.2f", ven);

}

