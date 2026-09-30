#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int expresion(int, int, int);
int main()
{
    int EXP, T = 0, P = 0, Q = 0;
    EXP = expresion(T, P, Q);
    while (EXP < 5500)
    {
        while (5500)
        {
            while (5500)
            {
                printf("\nT: %d, P: %d, Resultado: %d", T, P, Q, EXP);
                Q++;
                EXP = expresion(T, P, Q);
            }
            P++;
            Q = 0;
            EXP = expresion(T, P, Q);
        }
        T++;
        P = 0;
        Q = 0;
        EXP = expresion(T, P, Q);

    }
    return 0;
}
int expresion(int T, int P, int Q)
{
    int RES;
    RES = 15 * pow(T,4) + 12 * pow(P,5) + 9 * pow(Q,6);
    return (RES);
}
