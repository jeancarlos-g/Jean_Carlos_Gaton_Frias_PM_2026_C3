#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void intercambia(char fra[][30], int);

int main()
{
    int i, n;
    char fra[20][30];
    printf("\nIngrese el numero de filas del arreglo: ");
    scanf("%d", &n);
    for (i=0; i<n; i++)
    {
        printf("\nIngrese la linea de texto numero %d: ", i+1);
        fflush(stdin);
        gets(fra[i]);
    }
    printf("\n\n");
    intercambia(fra, n);
    for (i=0; i<n; i++)
    {
        printf("\nImpresio de la linea de texto %d: ", i+1);
        puts(fra[i]);
    }
    return 0;
}

void intercambia(char fra[][30], int n)
{
    int i, j;
    j = n - 1;
    char cad[30];
    for (i=0; i < (n/2); i++)
    {
        strcpy(cad, fra[i]);
        strcpy(fra[i], fra[j]);
        strcpy(fra[j], cad);
        j--;
    }
}
