#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#define SALIR 0

#define SUMAR 1

#define RESTAR 2

#define MULTIPLICAR 3

#define DIVIDIR 4

#define RAIZ 5

#define ELEVAR 6

#define ERR_OK 0

#define ERR_SYNTAX 1

#define ERR_DivByZero 555

#define ERR_RaizNegativa 556

// Funciones()

// Sirve para dividir el problema

// Valor y Referencia (punteros)

// devolver el error

// Ambito de las variable o ciclo de vida

// *operador de indireccion y para declarar punteros

// &operador de direccion

int suma(double s1, double s2, double *r);//declaracion de funcion

int divicion(double dividendo, double divisor, double *r);

int multiplicacion(double multiplicando, double multiplicador, double *r);

int resta(double minuendo, double sustraendo, double *r);

int raiz(double radicando, double *r);

int elevar (double base, double exponente, double *r);

int main()

{

    int menu = -1;

    double n1 = 0.0;

    double n2 = 0.0;

    double result = 0.0;//variable global

    int err = ERR_OK;

    printf("\nCALCULADORA V1.0");

    do

    {

        printf("\n0-SALIR\n1-SUMAR\n2-RESTAR\n3-MULTIPLICAR\n4-DIVIDIR\n5-RAIZ\n6-ELEVAR\n");

        scanf("%i",&menu);

        if(menu == SUMAR)

        {

            printf("\nSUMA");

            printf("\nEscriba el primer numero:");

            scanf("%lf",&n1);

            printf("\nEscriba el segundo numero:");

            scanf("%lf",&n2);

            err = suma(n1,n2,&result);

            if(err == ERR_OK)

            {

                printf("\nResultado de la SUMA de %lf + %lf = %lf",n1,n2,result);

            }

            else

            {

            }

        }

        if(menu == RESTAR)

        {

            printf("\nRESTAR");

            printf("\nEscriba el minuendo:");

            scanf("%lf",&n1);

            printf("\nEscriba el susrtraendo:");

            scanf("%lf",&n2);

            err = resta(n1,n2,&result);

            if(err == ERR_OK)

            {

                printf("\nResultado de la RESTA de %lf - %lf = %lf",n1,n2,result);

            }

            else

            {

            }
        }

        if(menu == MULTIPLICAR)

        {
            printf("\nMULTIPLICACION");

            printf("\nEscriba el multiplicando:");

            scanf("%lf",&n1);

            printf("\nEscriba el multiplicador:");

            scanf("%lf",&n2);

            err = multiplicacion(n1,n2,&result);

            if(err == ERR_OK)

            {

                printf("\nResultado de la MULTPLICACION de %lf * %lf = %lf",n1,n2,result);

            }

            else

            {

            }
        }

        if(menu == DIVIDIR)

        {

            printf("\nDIVIDIR");

            printf("\nEscriba el Dividendo:");

            scanf("%lf",&n1);

            printf("\nEscriba el Divisor:");

            scanf("%lf",&n2);

            err = divicion(n1,n2,&result);

            if(err == ERR_OK)

            {

               printf("\nDivision %lf/%lf=%lf",n1,n2,result);

            }else

            {

                if(err == ERR_DivByZero)

                {

                   printf("\nNo se puede dividir entre cero");

                }

            }

        }
        if (menu == RAIZ)
        {
            printf("\nRADICAR");

            printf("\nEscriba el radicando:");

            scanf("%lf",&n1);

            err = raiz(n1,&result);

            if(err == ERR_OK)

            {

                printf("\nResultado de la RAIZ de %lf = %lf",n1,result);

            }

            else if (ERR_RaizNegativa)
            {
                printf("\nError");
            }
        }
        if (menu == ELEVAR)
        {
            printf("\nELEVACION");

            printf("\nEscriba la base:");

            scanf("%lf",&n1);

            printf("\nEscriba el exponente:");

            scanf("%lf",&n2);

            err = elevar(n1,n2,&result);

            if(err == ERR_OK)

            {

                printf("\nResultado de la ELEVACION de %lf ^ %lf = %lf",n1,n2,result);

            }

            else

            {

            }
        }

    }

    while(menu != SALIR);

    return 0;

}

int suma(double s1, double s2, double *r)

{

    *r = s1 + s2;

    return ERR_OK;

}

int divicion(double dividendo, double divisor, double *r)

{

    if(divisor != 0)

    {

        *r = dividendo / divisor;

        return ERR_OK;

    }

    else

    {

        return ERR_DivByZero;

    }

}

int multiplicacion(double multiplicando, double multiplicador, double *r)

{
   *r = multiplicando * multiplicador;
   return ERR_OK;
}

int resta(double minuendo, double sustraendo, double *r)

{
   *r = minuendo - sustraendo;
   return ERR_OK;
}
int raiz(double radicando, double *r)
{
    if (radicando >= 0)
    {
        *r = sqrt(radicando);
    }
    else
    {
        return ERR_RaizNegativa;
    }
}
int elevar (double base, double exponente, double *r)
{
    *r = pow(base, exponente);
    return ERR_OK;
}
