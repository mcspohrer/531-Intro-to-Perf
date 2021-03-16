

/*****************************************************************************
 * FILE: dotpar.c
 * DESCRIPTION:
 *   This is a simple \\ program that computes the dot product of two 
 *   vectors.  
 * SOURCE: Matthew Spohrer
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*   
     The following structure contains the necessary information to allow the 
     function "dotprod" to access its input data and place its output so that 
     it can be accessed later. 
     */

double *a, *b, SUM;
int N,T;
#define VECLEN 500000000
#define NUMTHRD 5
pthread_mutex_t mtx;   

// work to be done by each thread. Takes the thread's ID (tid) and builds its 
// start and end indices using the tid supplied. If it is the main thread, it
// does the normal amount of work and whatever was leftover from T not dividing
// N.
// The only shared resource where contention can occur is SUM so that is the
// only thing guarded with locks.
void dotprod(int tid)
{
  int start, end, i; 
  double mysum;

  // start and end indices in the shared arrays determined here.
  start= (N/(T)) * tid;
  if(tid == T-1) {
    end = N;
  } else {
    end = (((N/T) * tid) + (N/T));
  }

  mysum = 0;
  for (i=start; i<end ; i++) 
  {
    mysum += (a[i] * b[i]);
  }
  pthread_mutex_lock(&mtx);
  SUM += mysum;
  pthread_mutex_unlock(&mtx);
}

int main (int argc, char* argv[])
{
  int i;
  SUM = 0;
  pthread_mutex_init(&mtx, NULL);     /* initialize mutex */

  // program uses VECLEN and NUMTHRD by default or the values can be entered
  // from the command line
  if(argc < 3) {
    printf("Usage: ./par <# of elements> <# of threads>\n");
    printf("By default; # of elements = %d; # of threads = %d\n", VECLEN, NUMTHRD);
    N = VECLEN;
    T = NUMTHRD;
  } else {
    N = atoi(argv[1]);
    T = atoi(argv[2]);
  }
  pthread_t thread[T-1];
  
  // initializes the two vectors
  a = (double*) malloc (N*sizeof(double));
  b = (double*) malloc (N*sizeof(double));
  for (i=0; i<N; i++) {
    a[i]=1;
    b[i]=a[i];
  }

  // creates the threads to do some work, the main thread does its portion of the work, too
  for (i=0; i < T-1; i++){
    pthread_create(&thread[i], NULL, (void*)dotprod,(void*) i);
  }
  dotprod(T-1);
  
  // if the main thread finishes before the worker threads, it waits on them to finish
  for (i=0; i < T-1; i++)   
    pthread_join(thread[i], NULL);

  printf ("Done. Parallel version: sum = %f; T = %d\n", SUM, T);
  free (a);
  free (b);
}
