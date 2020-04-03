#include "math.h"

int mod(int a,int b) //a mod b
{
  int mod;
  mod = a;
  while (mod >= b) {
    mod-=b;
  }
  return mod;
}


int div (int a, int b) //a div b
{
  int div;
  div  = 0;
  while (div * b <= a) {
    div++;
  }
  return div - 1;
}