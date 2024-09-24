#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack {
  int top;
  int array[64];
  void (*push)(struct stack *this, int disk);
  int (*pop)(struct stack *this);
} stack;

void push(stack *this, int disk) {
  this->top++;
  this->array[this->top] = disk;
}

int pop(stack *this) {
  int r = this->array[this->top];
  this->array[this->top] = 0;
  this->top--;
  return r;
}

typedef struct hanoi {
  const int height;
  unsigned long int count;
  stack towers[3];
  int (*checkMove)(struct hanoi *this, int from, int to);
  void (*moveWithCheck)(struct hanoi *this, int from, int to);
  void (*move)(struct hanoi *this, int from, int to);
  int (*status)(struct hanoi *this);
  void (*print)(struct hanoi *this);
} hanoi;

int checkMove(hanoi* this, int from, int to) {
  if(from < 0 || from > 2) return 0;
  if(to < 0 || to > 2) return 0;
  const int a = this->towers[from].array[this->towers[from].top];
  const int b = this->towers[to].array[this->towers[to].top];
  if(a > b) return 0;
  return 1;
}

void moveWithCheck(hanoi* this, int from, int to) {
  if(!checkMove(this, from, to)) return;
  this->count++;
  this->towers[to].push(&this->towers[to], this->towers[from].pop(&this->towers[from]));
}

void move(hanoi* this, int from, int to) {
  this->count++;
  this->towers[to].push(&this->towers[to], this->towers[from].pop(&this->towers[from]));
}

int status(hanoi* this) {
  return this->towers[1].top + 1 == this->height || this->towers[2].top + 1 == this->height;
}

void print(hanoi* h) {
  for (int j = h->height - 1; j >= 0; j--) {
    for(int i = 0; i < 3; i++) {
      if(h->towers[i].top < j) {
        for(int n = 0; n < h->height; n++) printf(" ");
        printf("0");
        for(int n = 0; n < h->height; n++) printf(" ");
        continue;
      }
      int disk = h->towers[i].array[j];
      const int shift = disk > 9;
      for(int n = 0; n < h->height - disk; n++) printf(" ");
      for(int n = 0; n < h->towers[i].array[j] - shift; n++) printf("-");
      printf("%d", h->towers[i].array[j]);
      for(int n = 0; n < h->towers[i].array[j]; n++) printf("-");
      for(int n = 0; n < h->height - disk; n++) printf(" ");
    }
    printf("\n");
  }
}

hanoi initialize(int height) {
  hanoi r = {
  .height = height,
  .count = 0,
  .checkMove = checkMove,
  .moveWithCheck = moveWithCheck,
  .move = move,
  .status = status,
  .print = print,
  };
  for(int i = 0; i < 3; i++) {
    r.towers[i].top = -1; 
    r.towers[i].push = push; 
    r.towers[i].pop = pop;
  }
  for(int i = height; i > 0; i--) r.towers[0].push(&r.towers[0], i);
  return r;
}

void solveIterative(hanoi* h) {
  const unsigned long long int one = 1;
  const unsigned long int max = (one << h->height) - 1;
  while(max >  h->count) {
    const int disk = ffs(h->count + 1);
    const int move_direction = (disk ^ h->height - 1) & 1;
    const int from = (h->count >> (disk - move_direction)) % 3;
    h->move(h, from, (from + 1 + move_direction) % 3);  
    //h->print(h);
    //printf("\x1b[%dF", h->height);
  }
}

void solveRecursive(hanoi* h, int disk, int from, int to, int aux) {
  if(disk == 0) return;
  solveRecursive(h, disk - 1, from, aux, to);
  h->move(h, from, to);
  //h->print(h);
  //printf("\x1b[%dF", h->height);
  solveRecursive(h, disk - 1, aux, to, from);
}

void game(hanoi* h) {}

int main(int argc, char** argv) {
  if(argc != 3) {
    printf("usage: hanoi {user, iterative, recursive} {number of plates}\n");
    return -1;
  }
  char* output;
  int height = (int) strtol(argv[2], &output, 10);
  hanoi h = initialize(height);
  h.print(&h);
  printf("\x1b[%dF", height);

  if(strcmp(argv[1], "user") == 0) game(&h);
  if(strcmp(argv[1], "iterative") == 0) solveIterative(&h);
  if(strcmp(argv[1], "recursive") == 0) solveRecursive(&h, height, 0, 2, 1);
  h.print(&h);
  
  return 0;
}
