#include <stdlib.h>
#include <stdio.h>

int* prim(int max){
  int array[max - 2];
  int count = 0;
  for(int i = 2; i < max; i++) {
    if(!array[i - 2]) {
      count++;
      for(int j = i; j < max; j += i) {
        array[j - 2] = 1;
      }
    }
  }
  int r[count];
  for(int i = 2; i < max; i++) {
    for(int j = 0; j < count; j++) {
      if(!array[i - 2]) {
        r[j] = i;
      } 
    }
  }
  return r;
}

int main(int argc, char** argv){
  if(argc != 2) printf("usage: ./prime_sieve.out {upper bound}");
  char* output;
  int max = strtol(argv[1], &output, 10);
}
