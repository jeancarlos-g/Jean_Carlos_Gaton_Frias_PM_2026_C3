#include <stdio.h>
#include <stdlib.h>

const int max = 100;

void lectura (int *, int);
void ordena(int *, int);
void imprime(int *, int);

int main()
{
    int tam, vec[max];
    do
    {
        printf("\nIngrese el tamano del arreglo: ");
        scanf("%d", &tam);
    }
    while (tam > max || tam < 1);
    lectura(vec, tam);
    ordena(vec, tam);
    imprime(vec, tam);

    return 0;
}

void lectura(int a[], int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        printf("\nIngrese el elmento %d: ", i + 1);
        scanf("%d", &a[i]);
    }
}


void imprime(int a[], int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        printf("\nA[%d]: %d", i, a[i]);
    }
}

void ordena(int a[], int t)
{
    int i, j, men, l;
    for (i = 0; i < (t - 1); i++)
    {
        men = a[i];
       l = i;
       for (j = (i+1); j < t; j++)
       {
           if (a[j] < men)
           {
               men = a[j];
               l = j;
           }
       }
       a[l] = a[i];
       a[i] = men;
    }
}


