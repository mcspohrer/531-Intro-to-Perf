#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main(){
  int i=1, k, s= 0;

  while(1) {
    if(s % 21 == 1) {printf("%d\n", i);return 0;}
    printf("i = %d, s = %d", i, s);
    s = pow(5,i);
    printf("i = %d, s = %d", i, s);
    ++i;
  }
}
