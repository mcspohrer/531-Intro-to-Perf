#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h> 
#include <math.h>

int
main() {
  int i;
  int pg_size = getpagesize();
  int size = 10 * pg_size;
  int * waste = malloc(size);
  if(!waste){
    fprintf(stderr, "malloc failed\n");
    exit(EXIT_FAILURE);
  }
  struct timeval start, stop;
  FILE * fp = fopen("raw_page.txt", "a");

  
  gettimeofday(&start,NULL);
  for(i = 0; i < size/sizeof(int) ; i+=pg_size/sizeof(int)){
    waste[i] = 1;
  }
  gettimeofday(&stop,NULL);
  fprintf(fp, "%ld ", 
      (stop.tv_sec*1000000 + stop.tv_usec) 
      - (start.tv_sec*1000000 +start.tv_usec));
  fclose(fp);
  free(waste);
}
