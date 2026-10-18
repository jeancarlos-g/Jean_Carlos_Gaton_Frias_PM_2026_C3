#include <stdio.h>
#include <stdlib.h>

void inverso (char *);

int main()
{
    char fra[50];
    printf("\nIngrese la linea de texto: ");
    gets(fra);
    printf("\nEscribe la linea de texto de forma inversa: ");
    inverso(fra);
    return 0;
}

void inverso(char *cadena)
{
    if (cadena[0] != '\0')
    {
        inverso(&cadena[1]);
        putchar(cadena[0]);
    }
}
