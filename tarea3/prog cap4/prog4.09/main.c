#include <stdio.h>
#include <stdlib.h>

int suma(int x, int y)
{
    return (x + y);
}
int resta(int x, int y)
{
    return(x - y);
}
int control (int (*apf) (int, int), int x, int y)
{
    int RES;
    RES = (*apf) (x, y);
    return (RES);
}
int main()
{
    int R1, R2;
    R1 = control(suma, 15, 5);
    R2 = control(resta, 10, 4);
    printf("\nResultado1: %d", R1);
    printf("\nResultado2: %d", R2);
    return 0;
}
