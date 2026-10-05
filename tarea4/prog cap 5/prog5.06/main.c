#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int MAX = 100;
void lectura (float *, int);
double suma (float *, int);

int main()
{
    float VEC[MAX];
    double RES;
    lectura(VEC, MAX);
    printf("\n\nSuma del arreglo: %.2lf", RES);
    return 0;
}
void lectura(float A[], int T)
{
    int I;
    for (I=0; I<T; I++)
    {
        printf("\nIngrese el elmento %d: ", I+1);
        scanf("%f", &A[I]);
    }
}
double suma(float A[], int T)
{
    int I;
    double AUX = 0.0;
    for (I=0; I<T; I++)
    {
        AUX += pow(A[I], 2);
    }
    return(AUX);
}
