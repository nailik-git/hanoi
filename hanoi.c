#include <stdio.h>
#include <stdlib.h>

typedef struct stack {
  int array[64];
  int top;
} stack;

stack push(stack tower, int disk) {
  tower.top++;
  tower.array[tower.top] = disk;
  return tower;
}

typedef struct hanoi {
  const int height;
  int count;
  stack towers[3];
} hanoi;

hanoi initialize(int height) {
  #define HANOI_INITIALIZER {.height = height, .count = 1}
  hanoi h = HANOI_INITIALIZER;
  for(int i = 0; i < 3; i++) h.towers[i].top = -1;
  for(int i = height; i > 0; i--) h.towers[0] = push(h.towers[0], i);
  return h;
}

void print(hanoi h) {
  for (int j = h.height - 1; j >= 0; j--) {
    for(int i = 0; i < 3; i++) {
      if(h.towers[i].top < j) {
        for(int n = 0; n < h.height; n++) printf(" ");
        printf("0");
        for(int n = 0; n < h.height; n++) printf(" ");
        continue;
      }
      int disk = h.towers[i].array[j];
      const int shift = disk > 9;
      for(int n = 0; n < h.height - disk + shift; n++) printf(" ");
      for(int n = 0; n < h.towers[i].array[j] - shift; n++) printf("-");
      printf("%d", h.towers[i].array[j]);
      for(int n = 0; n < h.towers[i].array[j]; n++) printf("-");
      for(int n = 0; n < h.height - disk; n++) printf(" ");
    }
    printf("\n");
  }
}

int main(int argc, char** argv) {
  if(argc != 3) {printf("usage: hanoi {user, iterative, recursive} {number of plates}"); return -1;}
  char* output;
  hanoi h = initialize(strtol(argv[2], &output, 10));
  print(h);
  return 0;  
}
