/*--------------------------------------------------
  Gprof Example 2
  --------------------------------------------------
  main
  |-- func1 -- func3
  |-- func2 
  --------------------------------------------------
  */
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
using namespace std;
//declaration of functions
int func1();
int func2();
int func3();
void func4(int);
void func5(int);

int func1(void) {
  int i=0,g=0;
  while(i++<50) {
    g+=i;
    func3();
  }
  return g;
}

int func2(void) {
  int i=0,g=0;
  while(i++<50) {
    g+=i;
  }
  return g;
}


int func3() {
  int i=0,g=0;
  while(i++ < 50) {
    g++;
  }
  return g;
}

void func5(int i) {
  if (i < 50) func4(++i);
  usleep(1);
}

void func4(int i) {
  if (i < 5) func5(++i);
}


int main(int argc, char** argv) {
  int iterations = 100000;
  printf("Number of iterations = %d\n", iterations);
  while(iterations--) {
    printf("the iteration No. %d\n", 1000-iterations);
    func1();
    func2();
    func4(0);
  }
}  
