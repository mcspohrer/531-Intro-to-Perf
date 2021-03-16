#include <sys/time.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define num_buckets 5
#define i90 (23/num_buckets) + 1
#define i95 (33/num_buckets) + 1

// Measures a the usecs for a single, 10, 1000, and 10000
// call(s) to usleep(1). returns the average number of 
// usecs per usleep(1).
long int
timer(int num_j){
  struct timeval tstart, tstop;
  unsigned int tot=0;
  gettimeofday(&tstart, NULL);
  for(int j = 0; j < num_j; j++){
    usleep(1);
  } 
  gettimeofday(&tstop, NULL);
  tot = (tstop.tv_sec*1000000 + tstop.tv_usec) 
    - (tstart.tv_sec*1000000 +tstart.tv_usec);
  return tot/num_j;
}

// measures usecs for a single call to usleep(1) w/o the overhead 
// of a loop. Returns the number of usecs to complete the call.
long int
single(){
  struct timeval tstart, tstop;
  unsigned int tot=0;
  usleep(1);
  gettimeofday(&tstart, NULL);
  usleep(1);
  gettimeofday(&tstop, NULL);
  tot = (tstop.tv_sec*1000000 + tstop.tv_usec)
    - (tstart.tv_sec*1000000 +tstart.tv_usec);
  return tot;
}

int
calc(int ci){
  int i,j, num_j;
  int per1[ci], per10[ci], per100[ci], per1000[ci], per10000[ci];
  int total = 0;
  float mean=0, var=0, sd=0;
  printf("total sample size = %d\n", ((ci==1) ? ci : ci * num_buckets));

  // does for when we` want 90% confidence interval
  for(int k = 0; k<ci; k++) {
    for(i = 0; i < 5; i++){
      num_j = 1;
      for(j = 0; j < i; j++) num_j *= 10;
      switch(i) {
        case 0:
          per1[k] = single();
          total += per1[k];
          break;
        case 1:
          per10[k] = timer(num_j);
          total += per10[k];
          break;
        case 2:
          per100[k] = timer(num_j);
          total += per100[k];
          break;
        case 3:
          per1000[k] = timer(num_j);
          total += per1000[k];
          break;
        case 4:
          per10000[k] = timer(num_j);
          total += per10000[k];
          break;
      }
    }
  }

  mean = ((float)total)/((float)(ci*5));
  for(i = 0; i < 5; i++){
    num_j = 1;
    for(j = 0; j < i; j++) num_j *= 10;
    printf("per %d: ", num_j);
    switch(i) {
      case 0:
        for(int k = 0; k<ci; k++) {
          printf(" %d", per1[k]);
          var += pow((((float)per1[k]) - mean),2);
        }
        break;
      case 1:
        for(int k = 0; k<ci; k++) {
          printf(" %d", per10[k]);
          var += pow((((float)per10[k]) - mean),2);
        }
        break;
      case 2:
        for(int k = 0; k<ci; k++) {
          printf(" %d", per100[k]);
          var += pow((((float)per100[k]) - mean),2);
        }
        break;
      case 3:
        for(int k = 0; k<ci; k++) {
          printf(" %d", per1000[k]);
          var += pow((((float)(per1000[k])) - mean),2);
        }
        break;
      case 4:
        for(int k = 0; k<ci; k++) {
          printf(" %d", per10000[k]);
          var += pow((((float)per10000[k]) - mean),2);
        }
        break;
    }
    printf("\n");
  }

  printf("mean = %.2f\n", mean);
  printf("variance = %.2f\n", var);
  printf("Standard Deviation = %.2f\n",sqrt(var));
  return 0;
}

void
main(){
  calc(1);
  calc(i90);
  calc(i95);
}
