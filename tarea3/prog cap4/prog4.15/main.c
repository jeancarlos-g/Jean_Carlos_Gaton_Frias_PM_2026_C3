#include <stdio.h>
#include <stdlib.h>

int productoria(int);
int main()
{
    int NUM;
    do
    {
        printf("\nIngresa el numero cual quieres calucular la productoria: ");
        scanf("%d", &NUM);
    }
    while (NUM > 100 || NUM < 1);
    printf("\nLa productoria de %d es: %d", NUM, productoria(NUM));
    return 0;
}
int productoria(int N)
{
    int I, PRO = 1;
    for (I = 1; I <= N; I++)
    {
        PRO *= I;

    }
    return (PRO);
}
