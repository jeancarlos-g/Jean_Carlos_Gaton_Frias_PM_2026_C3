#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
  float RES;
  int R, T, Q;
  printf("\nIngrese los valores de R, T Y Q: ");
  scanf("%d %d %d", R, T, Q);
  RES = pow (R, 4) - pow (T, 3) - pow(Q, 2);
  if (RES < 820)
    printf("\nR = %d\tT = %d\t Q = %d", R, T, Q);

}
