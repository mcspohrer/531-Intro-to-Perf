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
// mC = resulting array.
// I[] = the indices of mC a thread should calculate.
// semI = semaphore guarding I[].
int k,T,I[2];
double ** mA, **mB, **mC, num = 1.0;
sem_t semI;

// initializes everything
void
init()
{
  sem_init(&semI, 0, 1);
  I[0]=-1;
  I[1]=-1;

  // make some space
  mA = malloc(k * sizeof(double*));
  mB = malloc(k * sizeof(double*));
  mC= malloc(k * sizeof(double*));
  for(int i = 0; i < k; i++){
    mA[i] = malloc(k * sizeof(double));
    mB[i] = malloc(k * sizeof(double));
    mC[i] = malloc(k * sizeof(double));
  }
  // inits the matrices, column first
  for(int i = 0; i < k; i++){
    for(int j = 0; j < k; j++){
      mA[j][i] = num;
      mB[j][i] = num;
      mC[j][i] = 0.0;
    }
  }
}

// populates the shared array, I, with the indices of 
// the matrix holding the results. Guarded by a semaphore.
// It will iterate over each row to increase false sharing.
void
sendI(){
  int i = 0;
  int j = 0;

  while(i<k){
    j=0;
    while(j<k){
     // printf("i%dsendI\n, j", i);
      sem_wait(&semI);
      if(I[0] == -1 && I[1] == -1){
        I[0] = i;
        I[1] = j;
        sem_post(&semI);
        j++;
      } else 
      {
        sem_post(&semI);
      }
    }
    i++;
    if(i==k){
      I[0] = k;
      I[1] = k;
    }
  }
}

// performs the multiplication. Nabs the vertex it's calculating.
// Calculates the value to be stored in mC by row first, increasing
// false sharing.
void * mult(int tid)
{
  int r,c;
  double sum;
  // Nabs the indices or busy waits otherwise. 
  while(I[0]!=k){
    //if(I[0] != -1) printf("i%d multI\n, j", I[0]);
    sum = 0.0;
    sem_wait(&semI);
    if(I[0]<k) {
      if(I[0]!=-1){
        r = I[0];
        c = I[1];
        I[0]=-1;
        I[1]=-1;
        sem_post(&semI);
      }else{
        sem_post(&semI);
        continue;
      }
    }else{
      sem_post(&semI);
      pthread_exit(NULL);
    }

    for(int i = 0; i < k; i++)
      sum += mA[r][i] * mB[i][c]; 
    
    mC[r][c] = sum;
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
  pthread_t thread[T];
  init();
  // creates the threads to do some work, the main thread does its portion of the work, too
  for(i=0; i<T; i++){
    pthread_create(&thread[i], NULL, (void*)mult,(void*)i);
  }
  
  sendI();

  /*
  printf("result: \n");
  for(int i = 0; i < k; i++){
    for(int j = 0; j < k; j++)
      printf("%.0f ", mC[i][j]);
    printf("\n");
  }
*/
  // if the main thread finishes before the worker threads, it waits on them to finish
  for(i=0; i<T; i++)   {
    //printf("i:%d T=%d\n", i, T);
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
  sem_destroy(&semI);
}
