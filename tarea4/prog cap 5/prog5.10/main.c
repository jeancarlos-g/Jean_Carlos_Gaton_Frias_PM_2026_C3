#include <stdio.h>
#include <stdlib.h>

const int max = 100;

void lectura (int *, int);
int busca (int *, int, int);

int main()
{
    int res, ele, tam, vec[max];
    do
    {
        printf("\nIngrese el tamano del arreglo");
        scanf("%d", &tam);
    }
    while (tam > max || tam < 1);
    lectura(vec, tam);
    printf("\nIngrese el elmento a buscar: ");
    scanf("%d", &ele);
    res = busca(vec, tam, ele);
    if (res)
    {
        printf("\nEl elmento se encuentra en el arreglo");
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

int busca(int a[], int t, int e)
{
    int res, i = 0, ban = 0;
    while ((i < t) && (e >= a[i]) && !ban)
           {
               if (a[i] == e)
            {
                ban++;
            }else
            {
                i++;
            }
            if (ban)
            {
                res = i + 1;
            }else
            {
                res = ban;
            }

           }
            return (res);
}
