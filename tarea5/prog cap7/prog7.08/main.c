#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void minymay(char cad);

int main()
{
    int i, n;
    char fra[20][50];

    printf("\nIngrese el numero de filas del arreglo: ");
    scanf("%d", &n);
    for (i=0; i<n; i++)
    {
        printf("\nIngrese la linea %d del texto: ", i+1);
        fflush(stdin);
        gets(fra[i]);
    }
    printf("\n\n");
    for (i=0; i<n; i++)
    {
        minymay(fra[i]);
    }

    return 0;
}

void minymay(char *cadena)
{
    int i = 0, mi = 0, ma = 0;
    while (cadena[i] != '\0')
    {
        if (islower(cadena[i]))
        {
            mi++;
        }
        else
        {
            if (isupper(cadena[i]))
            {
                ma++;
            }
        }
        i++;
    }
    printf("\nNumero de letras minusculas: %d", mi);
    printf("\nNumero de letras mayusculas: %d", ma);
}
