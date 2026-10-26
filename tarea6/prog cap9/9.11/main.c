#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int clave;
    int departamento;
    float salario;
    float ventas[20];
}empleado;

void incrementa(FILE *);

int main()
{
    FILE *ar;
    if((ar = fopen("ad1.dat", "r+")) != NULL)
        incrementa(ar);
    else
        printf("\nEl archivo no se puede abrir");
    rewind(ar);
    fclose(ar);
    return 0;
}

void incrementa(FILE *ap)
{
    int i, j, t;
    float sum;
    empleado emple;
    t = sizeof(empleado);
    fread(&emple, sizeof(empleado), 1, ap);
    while(!feof(ap))
    {
        i = ftell(ap) / t;
        sum = 0;
        for (j=0; j<12; j++)
            sum += emple.ventas[j];
        if (sum > 100000)
        {
            emple.salario = emple.salario * 1.10;

            fseek(ap, (i-1)*sizeof(emple), 0);
            fwrite(&emple, sizeof(empleado), 1, ap);
            fseek(ap, i*sizeof(empleado), 0);
        }
        fread(&emple, sizeof(empleado), 1, ap);
    }

}
