#include <stdio.h>
#include <stdlib.h>

void hanoi(int n, char a, char b, char c)
{

    if (n == 1)
    {
        printf("\nMover disco %d de %c a %c", n, a, c);
        return;
    }
    hanoi(n-1, a,  b, c);
    printf("\nMover disco %d de %c a %c", n, a, b);
    hanoi(n-1, b, c, a);


}


int main()
{
    int disc;
    printf("Ingrese numero de discos: \n");
    scanf("%d", &disc);

    hanoi(disc, 'A', 'B', 'C');

    return;
}


