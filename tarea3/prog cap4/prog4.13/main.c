#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void parimp(int, int *, int *);
int main()
{
    int I, N, NUM, PAR = 0, IMP = 0;
    printf("\nIngresa el numero de datos: ");
    scanf("%d", &N);
    for (I = 1; I <= N; I++)
    {
        printf("\nIngresa el numero %d:", I);
        scanf("%d", &I);
        parimp(NUM, &PAR, &IMP);
    }
    printf("\nIngresa el numero de pares: %d", PAR);
    printf("\nIngresa el numero de impares: %d", IMP);
    return 0;
}
void parimp(int NUM, int *P, int * I)
{
    int RES;
    RES = pow(-1, NUM);
    if (RES < 0)
    {
        *I += 1;
    }
}
