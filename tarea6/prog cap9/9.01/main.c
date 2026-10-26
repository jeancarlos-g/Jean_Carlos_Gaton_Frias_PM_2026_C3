#include <stdio.h>
#include <stdlib.h>

int main()
{
    char p1;

    FILE *ar;

    ar = fopen("arch1022.txt", "w");

    if (ar != NULL)
    {
        printf("\nEscribe tu linea[]");
        while ((p1 = getchar()) != '\n')
        {
            fputc(p1, ar);
        }
        fclose(ar);
    }
    else
        printf("\nNo se puede abrir el archivo");
    return 0;
}
