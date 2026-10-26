#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void minmay(FILE *ar);

int main()
{
    FILE *ap;
    if ((ap = fopen("arc.txt","r")) != NULL)
    {
        minmay(ap);
        fclose(ap);
    }
    else
        printf("\nNo se pudo abrir el archivo");
    return 0;
}

void minmay(FILE *ap1)
{
    char cad[30];
    int i, mi = 0, ma = 0;
    while (!feof(ap1))
    {
        if( islower(cad[i]))
            mi++;
        else if (isupper(cad[i]))
            ma++;
       i++;
    }
    printf("\nNumero de minusculas: %d", mi);
    printf("\nNumero de mayusculas: %d", ma);
}
