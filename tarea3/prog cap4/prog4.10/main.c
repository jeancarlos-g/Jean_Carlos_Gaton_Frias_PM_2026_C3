#include <stdio.h>
#include <stdlib.h>

int multiplo(int, int);
int main()
{
    int NU1, NU2, RES;
    printf("\nIngresa los 2 numeros: ");
    scanf("%d %d", &NU1, &NU2);
    RES = multiplo(NU1, NU2);
    if (RES)
    {
        printf("\El segundo numero es el multiplo del primero");

    }else
    {
        printf("\El segundo numero no es el multiplo del primero");
    }
    return 0;
}
int multiplo(int N1, int N2)
{
    int RES;
    if ((N2 % N1) == 0)
    {
        RES = 1;

    }else
    {
        RES = 0;

    }
    return (RES);
}
