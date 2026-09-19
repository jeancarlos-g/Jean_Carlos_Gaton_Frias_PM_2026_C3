#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    int NUM;
    printf("\nIngrese un numero: ");
    scanf("%d", &NUM);
    if (NUM == 0)
    {
        printf("\nNulo");

    }else if (pow (-1, NUM) > 0)
    {
        printf("\nPar");

    }else
    {
        printf("\nImpar");
    }




}
