#include <stdio.h>
#include <stdlib.h>

const int max = 100;

void lectura (int[][max], int);
int simetrico (int[][max], int);
int main()
{
    int mat[max][max], n, res;
    do
    {
        printf("\nIngrese el tamano del arreglo: ");
        scanf("%d", &n);
    }
    while (n > max || n < 1);
    lectura(mat, n);
    res = simetrico(mat, n);
    if (res)
    {
        printf("\nEl arreglo bidimensional es simetrico");

    }else
    {
        printf("\nEl arreglo bidimensional no es simetrico");
    }
    return 0;
}

void lectura(int a[][max], int t)
{
    int i, j;
    for (i=0; i<t; i++)
    {
        for (j=0; j<t; j++)
        {
            printf("Fila: %d\tColumna: %d", i+1, j+1);
            scanf("%d", &a[i][j]);
        }
    }
}

int simetrico(int a[][max], int t)
{
    int i = 0, j, f = 1;
    while ((i < t) && f)
    {
        j = 0;
        while ((j < i) && f)
        {
            if (a[i][j] == a[j][i])
            {
                j++;

            }else
            {
                f = 0;
            }
        }
        i++;
    }
    return(f);
}
