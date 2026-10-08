#include <stdio.h>
#include <stdlib.h>

const int tam = 10;

void lectura (int [][tam], int);
void imprime (int [][tam], int);
int main()
{
    int mat[tam][tam];
    lectura(mat, tam);
    imprime(mat, tam);
    return 0;
}

void lectura(int a[][tam], int f)
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

void imprime(int a[][tam], int f)
{
    int i, j;
    for (i=0; i<f; i++)
    {
        for (j=0; j<tam; j++)
        {
            if (i == j)
            {
                printf("\nDiagonal %d %d: %d", i, j, a[i][j]);
            }
        }
    }
}
