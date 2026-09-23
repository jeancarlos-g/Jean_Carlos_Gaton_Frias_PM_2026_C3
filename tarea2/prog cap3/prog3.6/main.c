#include <stdio.h>
#include <stdlib.h>

int main()
{
    int I = 0;
    float SAL, PRO, NOM = 0;
    printf("\nIngrese el salario del profesor:\t");
    scanf("%f", &SAL);
    do
    {
        NOM = NOM + SAL;
        I = I + 1;
        printf("\nIngrese el salario del profesor -0 para terminar:\t");
        scanf("%f", &SAL);
    }
    while (SAL);
        PRO = NOM / I;
        printf("\nNomina: %.2f \t promedio de salarios: %.2f", NOM, PRO);
}
