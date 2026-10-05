#include <stdio.h>
#include <stdlib.h>

const int max = 100;

void lectura (int *, int);
int binaria (int *, int, int);

int main()
{
    int res, ele, tam, vec[max];
    do
    {
        printf("\nIngrese el tamano del arreglo: ");
        scanf("%d", &tam);
    }
    while (tam > max || tam < 1);
    lectura(vec, tam);
    printf("\nIngrese el elmento a buscar: ");
    scanf("%d", &ele);
    res = binaria(vec, tam, ele);
    if (res)
    {
        printf("\nEl elmento se encuentra en la posicion: %d", res);
    }
    else
    {
        printf("\nEl elmento no se encuentra en el arreglo");
    }
    return 0;
}

void lectura(int a[], int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        printf("\nIngrese el elmento %d: ", i + 1);
        scanf("%d", &a[i]);
    }
}

int binaria(int a[], int t, int e)
{
    int ele, izq = 0, cen, der = t-1, ban = 0;
    while ((izq <= der) && (!ban))
    {
        cen = (izq + der) / 2;
        if(e == a[cen])
        {
            ban = cen;

        }else
        {
            if (e > a[cen])
            {
                izq = cen;

            }else
            {
                der = cen;
            }
        }
    }
    return (ban);
}
