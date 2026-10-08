#include <stdio.h>
#include <stdlib.h>

const int max = 50;

void lectura (int[][max], int, int);
void suma (int [][max], int [][max], int [][max], int, int);
void imprime (int [][max], int, int);
int main()
{
    int ma[max][max], mv[max][max], mc[max][max];
    int fil, col;
    do
    {
        printf("\nIngrese el numero de filas de los arreglos: ");
        scanf("%d", &fil);
    }
    while ( fil > max || fil < 1);

    do
    {
        printf("\nIngrese numero de columnas de los arreglos: ");
        scanf("%d", &col);
    }
    while (col > max || col < 1);
    printf("\nLectura del arreglo ma\n");
    lectura(ma, fil, col);
    suma(ma, mb, mc, fil, col);
    printf("\nImpresion del arreglo mc\n");
    imprime(mc, fil, col);

    return 0;
}

void lectura(int a[][max], int f, int c)
{
    int i, j;
    for (i=0; i<f; i++)
    {
        for (j=0; j<f; j++)
        {
            printf("\nIngrese el elmento %d %d: ", i+1, j+1);
            scanf("%d", &a[i][j]);
        }
    }
}

void suma(int m1[][max], int m2[][max], int m3[][max], int f, int c)
{
    int i, j;
    for (i=0; i<f; i++)
    {
         for (j=0; j<f; j++)
         {
             m3[i][j] = m2[i][j] + m2[i][j];
         }
    }
}

void imprime(int a[][max], int f, int c)
{
    int i, j;
    for (i=0; i<f; i++)
    {
        for (j=0; j<c; j++)
        {
             printf("\nDiagonal %d %d: %d", i, j, a[i][j]);

        }
    }
}
