#include <stdio.h>
#include <stdlib.h>

const int TAM = 100;
void imprime (int, int);
void primo (int, int *);

int main()
{
    int p[TAM] = {1,2};
    int fla, j = 2, pri = 3;
    while (j <= TAM)
    {
        fla = 1;
        primo(pri, &fla);
        if (fla)
        {
            p[j] = pri;
            j++;
        }
        pri += 2;
    }
    imprime(p, TAM);
    return 0;
}

void primo (int a, int *b)
{
    int di = 3;
    while (*b && (di < (a / 2)))
    {
        if ((a % di) == 0)
        {
            *b = 0;
        }
        di++;
    }
}

void imprime(int primos[], int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        printf("\nPrimos[%d]: %d", i, primos [i]);
    }
}
