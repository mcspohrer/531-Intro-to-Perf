#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h> 
#include <sys/stat.h> 
#include <sys/mman.h>

#define SAMPLE_SIZE 10 
#define TOT_DIFF SAMPLE_SIZE * 3
// for 90% confidence interval
#define Z_TABLE 1.645;

static char *inputs[] = {
[0] "8",
[1] "64",
[2] "512",
[3] "1024",
[4] "4096",
[5] "64k",
};

// the first index of each column keeps track of how many differences
// have been calculated, otherwise each column holds the calculated
// differences for each size.
int diff[6][TOT_DIFF+1];
float mean[6], s[6], sum[6], c1[6], c2[6];

// creates the file with name = name of size = size
// byt just wrting a bunch of a's.
void
create_file(char * name, int size){
  char a = 'a';
  FILE * fp = fopen(name, "w");
  for(int i = 0; i < size; i++)
    fprintf(fp,"%c", a);
  fclose(fp);
}

// helper to clean up main a bit
void init_everything(){
  for(int i = 0; i < 6; i++){
    memset(&diff[i], 0, TOT_DIFF+1);
  }
  memset(&mean,0,sizeof(float)*6);
  memset(&s,0,sizeof(float)*6);
  memset(&sum,0,sizeof(float)*6);
  memset(&c1,0,sizeof(float)*6);
  memset(&c2,0,sizeof(float)*6);
  

  create_file("8",8);
  create_file("64",64);
  create_file("512",512);
  create_file("1024",1024);
  create_file("4096",4096);
  create_file("65536",65536);
}

// runs through each system call, reading the files sent to it
void getem(char * name, int size, int i){
  int tread, tfread, tmmap, k;
  struct timeval start, stop;
  char buf[size];
  FILE * fp = fopen(name,"r");
  int fd = open(name, O_RDONLY);
  memset(&buf, 0, size);

  gettimeofday(&start, NULL);
  read(fd, &buf, size);
  gettimeofday(&stop, NULL);
  tread = (stop.tv_sec*1000000 + stop.tv_usec) 
    - (start.tv_sec*1000000 +start.tv_usec);
  memset(&buf, 0, size);
//  printf("tread %d\n",tread);

  gettimeofday(&start, NULL);
  fread(&buf, 2, size, fp);
  gettimeofday(&stop, NULL);
  tfread = (stop.tv_sec*1000000 + stop.tv_usec) 
    - (start.tv_sec*1000000 +start.tv_usec);

  gettimeofday(&start, NULL);
  void * buffer = mmap(NULL, size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
  gettimeofday(&stop, NULL);
  tmmap = (stop.tv_sec*1000000 + stop.tv_usec) 
    - (start.tv_sec*1000000 +start.tv_usec);
  munmap(buffer, size);

  k = ++diff[i][0];
  diff[i][k] = tread - tfread;
  k = ++diff[i][0];
  diff[i][k] = tread - tmmap;
  k = ++diff[i][0];
  diff[i][k] = tfread - tmmap;

  close(fd);
  fclose(fp);
}

// helper to keep main all purtty and clean
void read_all() {
  getem("8",8,0);
  getem("64",64,1);
  getem("512",512,2);
  getem("1024",1024,3);
  getem("4096",4096,4);
  getem("65536",65536,5);
}

void
calcs(){
  int i,k;
  for(i = 0; i < 6; i++){
    for(k = 1; k <= TOT_DIFF; k++)
      mean[i] += (float)(diff[i][k]);
    mean[i] = mean[i]/TOT_DIFF; 
  }
  
  for(i = 0; i < 6; i++) {
    for(k = 1; k <= TOT_DIFF; k++)
      sum[i] += (diff[i][k] - mean[i]) * (diff[i][k] - mean[i]);
    s[i] = sqrt(sum[i]/(TOT_DIFF-1));
    
  }
}

void
pretty_print(){
  int i;
  for(i = 0; i < 6; i++){
    printf("differences for input %s:\n", inputs[i]);
    for(int k = 1; k < TOT_DIFF +1; k++) printf("%d ", diff[i][k]);
    printf("\n");
  }

  printf("--------------------------------------------------------\n");
  printf("| Input | Means  | Standard Deviation |     [c1,c2]    |\n");
  for(i = 0; i < 6; i++) {
    printf("| %-5s | %-6.2f | %-7.4f           | %-6.2f, %-6.2f |\n", 
        inputs[i], mean[i], s[i], c1[i], c2[i]);
  }
  printf("--------------------------------------------------------\n");

}

void
confidence(){
  for(int i = 0; i < 6; i++){
    c1[i] = mean[i] + Z_TABLE(s[i]/(sqrt(TOT_DIFF)));
    c2[i] = mean[i] - Z_TABLE(s[i]/(sqrt(TOT_DIFF)));
  }
}

void
main(){
  init_everything();
  for(int i = 0; i < SAMPLE_SIZE; i++)
    read_all();
  calcs();
  confidence();
  pretty_print();

}
