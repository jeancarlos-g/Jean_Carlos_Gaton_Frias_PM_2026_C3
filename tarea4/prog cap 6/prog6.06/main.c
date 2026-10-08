#include <stdio.h>
#include <stdlib.h>

const int max = 50;

void lectura(float[][max], int, int);
void intercambia(float[][max], int, int);
void imprime(float[][max], int, int);

int main()
{
    int f, c;
    float mat[max][max];
    do
    {
        printf("\nIngrese el numero de filas: ");
        scanf("%d", &f);
    }
    while ( f > max || f < 1);
    do
    {
        printf("\nIngrese el numero de columnas: ");
        scanf("%d", &c);
    }
    while ( c > max || c < 1);
    lectura(mat, f, c);
    intercambia(mat, f, c);
    imprime(mat, f, c);
    return 0;
}

void lectura(float a[][max], int f, int c)
{
    int i, j;
    for (i=0; i<f; i++)
    {
        for (j=0; j<c; j++)
        {
            printf("\nIngrese el elemento %d %d: ", i+1, j+1);
            scanf("%f", &a[i][j]);
        }
    }
}

void intercambia(float a[][max], int f, int c)
{
    int i, j;
    float aux;
    for (i=0; i<(c / 2); i++)
    {
        for (j=0; j<c; j++)
        {
            aux = a[j][i];
            a[j][i] = a[j][c - i -1];
            a[j][c - i - 1] = aux;
        }
    }
}

void imprime(float a[][max], int f, int c)
{
    int i, j;
    for (i=0; i<f; i++)
    {
        for (j=0; j<c; j++)
        {
            printf("\nEl elemento %d %d: %5.2f", i+1, j+1, a[i][j]);
        }
    }
}
