#include <stdio.h>
#include <stdlib.h>

const int TAM = 50;
void lectura(int *, int);
void frecuencia(int , int, int , int);
void impresion(int *, int);
void mayor(int *, int);

int main()
{
    int CAL[TAM], FRE[6] = {0};
    lectura(CAL, TAM);
    frecuencia(CAL, TAM, FRE, 6);
    printf("\nFrecuencia de calificaciones\n");
    impresion(FRE, 6);
    mayor(FRE, 6);
    return 0;
}
void lectura (int VEC[], int T)
{
    int I;
    for (I=0; I<T; I++)
    {
        printf("\nIngrese la calificación -0:5- del alumno %d: ", I+1);
        scanf("%d", &VEC[I]);
    }
}
void impresion (int VEC[], int T)
{
    int I;
    for (I=0; I<T; I++)
    {
        printf("\nVEC[%d]: %d", I+1, VEC[I]);
    }
}
void frecuencia (int A[], int P, int B[], int T)
{
    int I;
    for (I=0; I<P; I++)
    {
        if ((A[I] >=0) && (A[I] < 6))
        {
            B[A[I]]++;
        }
    }
}
void mayor(int *X, int T)
{
    int I, MFRE = 0, MVAL = X[0];
    for (I=1; I<T; I++)
    {
        if (MVAL < X[I])
        {
            MFRE = I;
            MVAL = X[I];
        }
    }
    printf("\n\nMayor frecuencia de calificaciones: %d \tValor: %d", MPRE, MVAL);
}
