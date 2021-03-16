#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>

int C, A;
char* f = "gcc-1K.trace";
FILE * fp; 
// first row keeps a count of the number of addresses being held in that set
int ** cache;


// initializes everything
void
init(int argc, char ** argv){
  int fend = 0, i, j;
  if(argc < 3) {
    printf("Usage: ./trace <set size> <cache size> [optional]<filename>\n");
    exit(EXIT_FAILURE);
  }

  // seed random
  srand(time(0));

  // set associativity can be 2 or 4
  A = atoi(argv[1]);
  if(A != 2 && A !=4 && A != 1){
    printf("A == 2 or A == 4\n");
    exit(EXIT_FAILURE);
  }

  // cache size has to be divisible by 4
  C = atoi(argv[2]);
  if(C%4 != 0){
    printf("4 | Cache size\n");
    exit(EXIT_FAILURE);
  }

  // initialize the array representing the cache
  cache = malloc((A+1) * sizeof *cache);
  for(i=0; i <= C/A; i++){
    cache[i] = malloc(C/A * sizeof(int));
  }
  for(i=0; i < A + 1; i++){
    for(j=0; j <= C/A; j++){
      cache[i][j] = 0;
    }
  }

  // if the user entered a trace file (default is the 1K)
  if(argc == 4)
    f = argv[3];

  // opens the file
  if((fp = fopen(f, "r"))==NULL){
    printf("error in openeing file, exiting\n");
    exit(EXIT_FAILURE);
  }
}

// reads the address from the file 
unsigned int
in(){
  unsigned int buf = 0;
  int rc = 0;
  fseek(fp,2,SEEK_CUR);
  rc = fscanf(fp, "%x%*[^\n]", &buf);
  if(rc == -1) buf = 0;
  return buf;
}

// increments the set then inserts the new address to the appropriate 
// address
int
cache_it(unsigned int buf) {
  int row=0, set, f=0;
  // finds the set the address line would belong to
  set = buf % (C/A);

  // starts at the beginning of the set
  // cache[0] holds a total count of how many addresses
  // have been stored in the set
  for(row = 1; row < A + 1; row++){
    // traverses the set, checking if the address space is already in
    // the cache, returning a 0 because it was not a miss
    if(cache[row][set] == buf)
      return 0;
    // Because the sets are populated starting at cache[1] (the first
    // line in the set) if a 0 is encountered, then the address is
    // not in the cache and should be entered in the first available 
    // open address space, returning one b/c of a compulsory miss 
    else if(cache[row][set] == 0){
      cache[0][set]++;
      cache[row][set] = buf;
      return 1;
    }
  }

  // good old fashioned cache miss. The address was not in the set and 
  // the set was full so we evict a random cache line to hold the 
  // address space.
  cache[((rand())%A) +1][set] = buf;
  cache[0][set]++;
  return 1;
}

void
main(int argc, char * argv[]){
  int fend, miss=0, tot = 0;
  unsigned int buf=1;
  init(argc, argv);

  while(buf = in()){
    miss += cache_it(buf);
    tot++;
  }

  printf("Ass = %d, Cache = %d, file = %s, total= %d,miss = %d, miss rate = %3.2f\n",
      A, C, f,tot, miss,((float)miss/(float)tot));

  free(cache);
  fclose(fp);
}
