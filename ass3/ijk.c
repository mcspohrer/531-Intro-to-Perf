/* matrix multiply permutations */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "mm.h"



/* global arrays */
array ga, gb, gc;

/* check the result array for correctness */
void checkresult(array c, int n)
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      if (c[i][j] != (double)n) {
        printf("Error: bad number (%f) in result matrix (%d,%d)\n", 
            c[i][j], i, j);
        fflush(stdout);
        exit(0);
      }
}

/* initialize input arrays to 1 */
void init(array a, array b, int n) 
{
  int i,j;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      a[i][j] = 1.0;
      b[i][j] = 1.0;
    }
  }
}

/* reset result array to zero */
void reset(array c, int n)
{
  int i,j;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      c[i][j] = 0.0;
    }
  }
}

/* print an array (debug) */
void printarray(array a, int n)
{
  int i, j;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      printf("%5.1f ", a[i][j]);
    }
    printf("\n");
  }
}

/***********************************************
 * Six different versions of matrix multiply 
 ***********************************************/
void ijk(array A, array B, array C, int n) 
{
  int i, j, k;
  double sum;

  /* $begin mm-ijk */
  for (i = 0; i < n; i++) 
    for (j = 0; j < n; j++) {
      sum = 0.0;
      for (k = 0; k < n; k++)
        sum += A[i][k]*B[k][j];
      C[i][j] += sum;
    }
  /* $end mm-ijk */

}

void jik(array A, array B, array C, int n) 
{
  int i, j, k;
  double sum;

  /* $begin mm-jik */
  for (j = 0; j < n; j++) 
    for (i = 0; i < n; i++) {
      sum = 0.0;
      for (k = 0; k < n; k++)
        sum += A[i][k]*B[k][j];
      C[i][j] += sum;
    }
  /* $end mm-jik */
}

void ikj(array A, array B, array C, int n) 
{
  int i, j, k;
  double r;

  /* $begin mm-ikj */
  for (i = 0; i < n; i++)
    for (k = 0; k < n; k++) {
      r = A[i][k];
      for (j = 0; j < n; j++)
        C[i][j] += r*B[k][j];
    }
  /* $end mm-ikj */
}

void kij(array A, array B, array C, int n)
{
  int i, j, k;
  double r;

  /* $begin mm-kij */
  for (k = 0; k < n; k++)
    for (i = 0; i < n; i++) {
      r = A[i][k];
      for (j = 0; j < n; j++)
        C[i][j] += r*B[k][j];
    }
  /* $end mm-kij */
}

void kji(array A, array B, array C, int n)
{
  int i, j, k;
  double r;

  /* $begin mm-kji */
  for (k = 0; k < n; k++)
    for (j = 0; j < n; j++) {
      r = B[k][j];
      for (i = 0; i < n; i++)
        C[i][j] += A[i][k]*r;
    }
  /* $end mm-kji */
}

void jki(array A, array B, array C, int n)
{
  int i, j, k;
  double r;

  /* $begin mm-jki */
  for (j = 0; j < n; j++)
    for (k = 0; k < n; k++) {
      r = B[k][j];
      for (i = 0; i < n; i++)
        C[i][j] += A[i][k]*r;
    }
  /* $end mm-jki */
}


void sd(int time[]){
  double mean=0, sd=0;
  int i;
  for(i = 0; i < 30; i++) 
    mean += time[i]; 
  mean = mean /30;
  for(i = 0; i < 30; i++) 
    sd += (time[i]-mean) * (time[i]-mean); 
  sd = sd/29;
  sd = sqrt(sd);
  printf("mean = %.2f, sd = %.2f\n", mean, sd); 
  
}
int main(int argc, char * argv[])
{
  if(argc < 2) {
    dprintf(2, "usage: <mult ver> <size of matrices>\n");
    exit(EXIT_FAILURE);
  }

  //FILE * fp = fopen(argv[1], "a");
  int n = atoi(argv[1]);
  struct timeval start, stop;
  uint tot = 0;
  int times[30], i;

  init(ga,gb,n);
  printf("ijk\n");
  for(i =0; i < 30; i++){
    if(gettimeofday(&start,NULL) < 0){
      printf("u-oh, error with gettimeofday()\n");
      exit(EXIT_FAILURE);
    }

    //ijk(ga,gb,gc,n);
    //jik(ga,gb,gc,n);
    //ikj(ga,gb,gc,n);
    kij(ga,gb,gc,n);
    //kji(ga,gb,gc,n);
    //jki(ga,gb,gc,n);

    if(gettimeofday(&stop,NULL) < 0){
      printf("u-oh, error with gettimeofday()\n");
      exit(EXIT_FAILURE);
    }
    reset(gc,n);
    tot = ((stop.tv_sec * 1000000)+ stop.tv_usec) - 
      ((start.tv_sec * 1000000)+ start.tv_usec);
    times[i] = tot;
  }
  for(i=0; i<30; i++) 
    printf("%d\n",times[i]);

  sd(times);
  //fclose(fp);
}
