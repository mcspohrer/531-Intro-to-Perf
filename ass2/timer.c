// determines the differences in granularity between 
// time() and gettimeofday() 
#include <sys/time.h> 
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// total number of iterations
#define tot_i 15

void
main(){
  time_t tstart, tstop, tdelta[tot_i];
  struct timeval gstart, gstop;
  unsigned long long int gdelta[tot_i];
  // init the delta arrays
  memset(&gdelta, 0, sizeof(struct timeval) * tot_i);
  memset(&tdelta, 0, sizeof(time_t) * tot_i);

  for(int i = 0; i < tot_i; i++){
    // measures the granularity of time() by doing some nonsense
    // work for x amount of time, finding the delta of the 
    // starting and stopping times
    time(&tstart);
    for(int j = 0; j < i*100000000; ++j);
    time(&tstop);
    tdelta[i] = tstop - tstart;
    
    printf("i= %d, time start= %ld, time stop= %ld, tdelta = %ld\n",
        i, tstart, tstop, tdelta[i]);

    // measures the granularity of gettimeofday() by doing some nonsense
    // work for x amount of time, finding the delta of the 
    // starting and stopping times
    gettimeofday(&gstart,NULL);
    for(int j = 0; j < i*100000000; ++j);
    gettimeofday(&gstop,NULL);

    gdelta[i] = (gstop.tv_sec*1000000 + gstop.tv_usec)
        - (gstart.tv_sec*1000000 +gstart.tv_usec);

    printf("i= %d, gsart=%ld, gstop=%ld, gdelta = %lld\n",
         i, (gstop.tv_sec*1000000 + gstop.tv_usec), 
         (gstart.tv_sec*1000000 +gstart.tv_usec),
         gdelta[i]);
  }

}
