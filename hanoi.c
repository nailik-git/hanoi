#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a stack modeled through a bounded array with push and pop functions
typedef struct stack {
  int top;
  int array[64];
} stack;

void push(stack *this, int disk) {
  // pushes disk onto the stack
  this->top++;
  this->array[this->top] = disk;
}

int pop(stack *this) {
  // pops disk from the stack
  const int r = this->array[this->top];
  this->top--;
  return r;
}

// a struct to model the towers of hanoi game
typedef struct hanoi {
  const unsigned char height;
  unsigned long int count; // number of steps
  stack towers[3];
} hanoi;

int checkMove(hanoi* this, int from, int to) {
  // checks if a move is legal
  if(from < 0 || from > 2) return 0;
  if(to < 0 || to > 2) return 0;
  if(this->towers[from].top == -1) return 0;
  if(this->towers[to].top == -1) return 1;
  const int a = this->towers[from].array[this->towers[from].top];
  const int b = this->towers[to].array[this->towers[to].top];
  if(a > b) return 0;
  return 1;
}

void moveWithCheck(hanoi* this, int from, int to) {
  // moves disk from tower from to tower to and checks the move
  if(!checkMove(this, from, to)) return;
  this->count++;
  push(&this->towers[to], pop(&this->towers[from]));
}

void move(hanoi* this, int from, int to) {
  // moves disk from tower from to tower to without checking the move
  // this->count++;
  push(&this->towers[to], pop(&this->towers[from]));
}

int status(hanoi* this) {
  // returns true if the towers of hanoi are solved
  return this->towers[1].top + 1 == this->height || this->towers[2].top + 1 == this->height;
}

void print(hanoi* h) {
  // prints the towers to terminal
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

hanoi initialize(unsigned char height) {
  // creates an object of the hanoi struct
  hanoi r = {
  .height = height,
  .count = 0,
  };
  for(int i = 0; i < 3; i++) {
    r.towers[i].top = -1; 
  }
  for(int i = height; i > 0; i--) push(&r.towers[0], i);
  return r;
}

int modLookup(int n) {
  // look up table to mitigate mod operation for move function
  const int lookup[5] = {0, 1, 2, 0, 1};
  return lookup[n];
}

void solveIterative(hanoi* h) {
  // solves the towers of hanoi iteratively
  // constants:
  const unsigned char height = h->height;
  const unsigned long long one = 1;
  const unsigned long max = (one << height) - 1; // max number of moves needed to solve
  unsigned long int i = 0; // iterator
  for(; i < max; i++) {
    const int disk = __builtin_ctz(i + 1) + 1; // gray codes method to find the disk to move
    const int move_direction = (disk ^ height - 1) & 1; // 0 or 1, decides wether to move left or right
    const int from = (i >> (disk - move_direction)) % 3; /* determines the tower the disk is currently on
                                                          * this works somewhat similar to the gray codes,
                                                          * through how many times we have moved bigger disks 
                                                          * impossible according to wikipedia :) */
    move(h, from, modLookup(from + 1 + move_direction));  
    //h->print(h);
    //printf("\x1b[%dF", h->height);
  }
  h->count = i;
}

void solveRecursive(hanoi* h, int disk, int from, int to, int aux) {
  // solves the towers of hanoi recursively
  if(disk == 0) return; // break cond
  solveRecursive(h, disk - 1, from, aux, to); // move bigger disks
  move(h, from, to); // move this disk
  h->count++;
  //h->print(h);
  //printf("\x1b[%dF", h->height);
  solveRecursive(h, disk - 1, aux, to, from); // move bigger disks again
  }

void game(hanoi* h) {
  // user mode to play the towers of hanoi
  while(!status(h)) {
    print(h); // print the current state
    int from;
    int to;
    printf("move disk from tower: ");
    scanf("%d", &from); // user input to move the disks
    printf("to tower: ");
    scanf("%d", &to);
    moveWithCheck(h, from, to); // execute move with check
    printf("\x1b[%dF", h->height + 2); // return cursor to top of the screen
    }
}

int main(int argc, char** argv) {
  if(argc != 3) {
    printf("usage: hanoi {user, iterative, recursive} {number of plates}\n");
    return -1;
  }
  char* output;
  unsigned char height = (unsigned char) strtol(argv[2], &output, 10);
  hanoi h = initialize(height);
  print(&h);
  printf("\x1b[%dF", height);

  if(strcmp(argv[1], "user") == 0) game(&h);
  else if(strcmp(argv[1], "iterative") == 0) solveIterative(&h);
  else if(strcmp(argv[1], "recursive") == 0) solveRecursive(&h, height, 0, 2, 1);
  else {
    printf("usage: hanoi {user, iterative, recursive} {number of plates}\n");
    return -1;
  }
  print(&h);
  
  return 0;
}
