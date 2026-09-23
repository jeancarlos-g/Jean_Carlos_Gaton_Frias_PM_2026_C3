#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    int I, N, NUM, SPA = 0, SIM = 0, CIM = 0;
    printf("\nIngrese el numero de datos que se va a procesar:\t");
    scanf("%d", &N);
    if ( N > 0)
    {
        for (I=1; I<=1; I++)
        {
            printf("\Ingrese el numero %d: ",I );
            scanf("%d", &I);
            if (NUM)
            {
                if (pow(-1, NUM)>0)
                {
                    SPA = SPA + NUM;

                }else
                {
                    SIM = SIM + NUM;
                    CIM++;
                }
            }
        }
        printf("\nLa suma de los numeros pares es de: %d", SPA);
        printf("\nEl numero promedio de impares es: %5.2f", (float)(SIM / CIM));
    }else
    {
        printf("\nEl valor de N es incorrecto");
    }
}
