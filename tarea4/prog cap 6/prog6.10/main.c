#include <stdio.h>
#include <stdlib.h>

const int max = 50;

void cuadrado(int[][max], int);
void imprime(int[][max], int);

int main()
{
    int cma[max][max], tam;
    do
    {
        printf("\nIngrese el tamano impar de la matriz: ");
        scanf("%d", &tam);
    }
    while ((tam > max || tam < 1) && (tam % 2));
    cuadrado(cma, tam);
    imprime(cma, tam);

    return 0;
}
void cuadrado(int a[][max], int n)
{
    int i = 1, fil = 0, col = n / 2, num = n * n;
    while(i <= num)
    {
        a[fil][col] = i;
        if (i%n != 0)
        {
            fil = (fil - 1 + n) % n;
            col = (col + 1) % n;
        }
        else
        {
            fil++;
        }
        i++;
    }
}

void imprime(int a[][max], int n)
{
    int i, j;
    for (i=0; i<n; i++)
    {
        for (j=0; j<n; j++)
        {
            printf("\nElemento %d %d: %d", i+1, j+1, a[i][j]);
        }

    }
}
