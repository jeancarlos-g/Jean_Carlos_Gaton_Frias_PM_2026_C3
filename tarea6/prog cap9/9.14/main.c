#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void minmay(FILE *ar);

int main()
{
    char p;
    FILE *ar;
    if ((ar = fopen("arc5.txt","r")) != NULL)
    {
        minmay(ar);
        fclose(ar);
    }
    else
        printf("\nNo se pudo abrir el archivo");
    return 0;
}

void minmay(FILE *arc)
{
    int min = 0, may = 0;
    char p;
    while (!feof(arc))
    {
        p = fgetc(arc);
        if( islower(p))
            min++;
        else if (isupper(p))
            may++;
    }
    printf("\nNumero de minusculas: %d", min);
    printf("\nNumero de mayusculas: %d", may);
}
