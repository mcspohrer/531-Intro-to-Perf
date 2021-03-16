#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables. 
//
// k of kxk matrix.
// T = number of threads.
// num = the number the matrices are init'd to.
// mA, mB = arrays to be multiplied.
// mC = results array.
int k,T,num = 1;
int ** mA, **mB, **mC;

// initializes everything
void
init()
{
  int i,j;
  // make some space, 
  mA = malloc(k * sizeof(int*));
  for(i = 0; i < k; i++)
    mA[i] = malloc(k * sizeof(int));
  mB = malloc(k * sizeof(int*));
  for(i = 0; i < k; i++)
    mB[i] = malloc(k * sizeof(int));
  mC = malloc(k * sizeof(int*));
  for(i = 0; i < k; i++)
    mC[i] = malloc(k * sizeof(int));
  
  // inits the matrices, by row 
  for(i = 0; i < k; i++)
    for(j = 0; j < k; j++)
      mA[i][j] = num;
  for(i = 0; i < k; i++)
    for(j = 0; j < k; j++)
      mB[i][j] = num;
  for(i = 0; i < k; i++)
    for(j = 0; j < k; j++)
      mC[i][j] = 0;
}

void * mult(int tid)
{
  int start,stop,sum,i,j,n;
  start= (k/(T)) * tid;
  if(tid == T-1) {
    stop = k-1;
  } else {
    stop = (((k/T) * tid) + (k/T))-1;
  }

  for(i = start; i <= stop; i++){
    for(n = 0; n < k; n++){
      sum = 0;
      for(j = 0; j < k; j++){
        sum += mA[i][j] * mB[j][n]; 
      }
      mC[i][n] = sum;
    }
  }
}

int main (int argc, char* argv[])
{
  int i,j; 
  if(argc < 3) {
    printf("Usage: ./s <k of kxk matrix> <# of threads>\n");
    exit(EXIT_FAILURE);
  } else {
    k = atoi(argv[1]);
    T = atoi(argv[2]);
  }
  pthread_t thread[T-1];
  init();
  // creates the threads to do some work, the main thread does its portion of the work, too
  for(i=0; i<T-1; i++){
    pthread_create(&thread[i], NULL, (void*)mult,(void*)i);
  }
  mult(T-1);
  
  /*
  printf("result: \n");
  for(i = 0; i < k; i++){
    for(j = 0; j < k; j++)
      printf("%d ", mC[i][j]);
    printf("\n");
  }
*/
  // if the main thread finishes before the worker threads, it waits on them to finish
  for(i=0; i<T-1; i++)   {
    pthread_join(thread[i], NULL);
  }
  for(i=k-1; i>=0;i--){
    free(mA[i]);
    free(mB[i]);
    free(mC[i]);
  }
  free (mA);
  free (mB);
  free (mC);
}
