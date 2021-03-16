#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int BinarySearch(int searchkey, int low, int high, int array[]) {
  int Midpoint;
  while (low <= high) {
    Midpoint = array[(low + high) / 2];
    if (searchkey < Midpoint) {
      high = Midpoint - 1;
    } else if (searchkey > Midpoint) {
      low = Midpoint + 1;
    } else if (searchkey == Midpoint){
      return Midpoint;
    }
  }
  return -1;
}


int main() {
  printf("Welcome to the binary search, see if you can make me faster.\n");
  srand(time(NULL));

  int i, j, searchkey, answer;
  int number = 5000;
  int low = 0;
  int high = 1000000;
  int *array;
  array = (int*) malloc(sizeof(int)*high);

  for (i = 0; i <= (high-1); i++) {
    array[i] = i;
  }


  printf("How many searches would you like to make? ");
  scanf("%d", &number);
  for ( j = 0; j < (number-1); j++ ) {
    searchkey = rand() % high;
    answer = BinarySearch(searchkey, low, high, array);
    printf("Search for %d Found %d\n", searchkey, answer);
  }

  return 0;

}

