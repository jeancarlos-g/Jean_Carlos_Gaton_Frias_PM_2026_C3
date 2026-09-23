#include <stdio.h>
#include <stdlib.h>

int main()
{
    float PAG, SPA;
    SPA = 0;
    printf("\nIngrese le primer pago: \t");
    scanf("%f", &PAG);
    while(PAG)
    {
        SPA = SPA + PAG;
        printf("\nIngrese el siguiente pago:\t ");
        scanf("%f", &PAG);

    }
    printf("\nEl total de pagos del mes es: %.2f", SPA);


}
