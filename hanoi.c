#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a stack modeled through a bounded array with push and pop functions
typedef struct stack {
  int top;
  int array[64];
} stack;

void push(stack* s, int disk) {
  // pushes disk onto the stack
  s->top++;
  s->array[s->top] = disk;
}

int pop(stack* s) {
  // pops disk from the stack
  const int r = s->array[s->top];
  s->top--;
  return r;
}

// a struct to model the towers of hanoi game
typedef struct hanoi {
  const char height;
  uint64_t count; // number of steps
  stack towers[3];
} hanoi;

int check_move(hanoi* h, int from, int to) {
  // checks if a move is legal
  if(from < 0 || from > 2) return 0;
  if(to < 0 || to > 2) return 0;
  if(h->towers[from].top == -1) return 0;
  if(h->towers[to].top == -1) return 1;
  const int a = h->towers[from].array[h->towers[from].top];
  const int b = h->towers[to].array[h->towers[to].top];
  if(a > b) return 0;
  return 1;
}

void move_with_check(hanoi* h, int from, int to) {
  // moves disk from tower from to tower to and checks the move
  if(!check_move(h, from, to)) return;
  h->count++;
  push(&h->towers[to], pop(&h->towers[from]));
}

void move(hanoi* h, int from, int to) {
  // moves disk from tower from to tower to without checking the move
  // h->count++;
  push(&h->towers[to], pop(&h->towers[from]));
}

int status(hanoi* h) {
  // returns true if the towers of hanoi are solved
  return h->towers[1].top + 1 == h->height || h->towers[2].top + 1 == h->height;
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

hanoi initialize(char height) {
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

void solve_iterative(hanoi* h) {
  // solves the towers of hanoi iteratively
  // constants:
  const char height = h->height;
  const uint64_t max = 0xFFFFFFFFFFFFFFFF;
  const uint64_t bound = max >> (64 - height); // max number of moves needed to solve

  // look up table to mitigate mod operation for move function
  const int lookup[5] = {0, 1, 2, 0, 1};

  uint64_t i = 0; // iterator
  for(; i < bound; i++) {
    const int disk = __builtin_ctz(i + 1) + 1; // gray codes method to find the disk to move
    const int move_direction = (disk ^ height - 1) & 1; // 0 or 1, decides wether to move left or right
    const int from = (i >> (disk - move_direction)) % 3; /* determines the tower the disk is currently on
                                                          * h works somewhat similar to the gray codes,
                                                          * through how many times we have moved bigger disks 
                                                          * impossible according to wikipedia :) */
    move(h, from, lookup[from + 1 + move_direction]);  
    //h->print(h);
    //printf("\x1b[%dF", h->height);
  }
  h->count = i;
}

void solve_recursive(hanoi* h, int disk, int from, int to, int aux) {
  // solves the towers of hanoi recursively
  if(disk == 0) return; // break cond
  solve_recursive(h, disk - 1, from, aux, to); // move bigger disks
  move(h, from, to); // move h disk
  h->count++;
  //h->print(h);
  //printf("\x1b[%dF", h->height);
  solve_recursive(h, disk - 1, aux, to, from); // move bigger disks again
  }

void game(hanoi* h) {
  // user mode to play the towers of hanoi
  while(!status(h)) {
    print(h); // print the current state
    int from;
    int to;
    printf("\n                       \n           \x1b[2F");
    printf("move disk from tower:  ");
    printf("\b");
    scanf("%d", &from); // user input to move the disks
    printf("to tower: ");
    scanf("%d", &to);
    move_with_check(h, --from, --to); // execute move with check
    printf("\x1b[%dF", h->height + 2); // return cursor to top of the screen
    }
}

int main(int argc, char** argv) {
  if(argc != 3) {
    printf("usage: hanoi <user|iterative|recursive> <number of plates (less than 64)>\n");
    return -1;
  }
  char* output;
  char height = (char) strtol(argv[2], &output, 10);
  if(height > 64) return -1;
  hanoi h = initialize(height);
  print(&h);
  printf("\x1b[%dF", height);

  if(strcmp(argv[1], "user") == 0) game(&h);
  else if(strcmp(argv[1], "iterative") == 0) solve_iterative(&h);
  else if(strcmp(argv[1], "recursive") == 0) solve_recursive(&h, height, 0, 2, 1);
  else {
    printf("usage: hanoi {user, iterative, recursive} {number of plates}\n");
    return -1;
  }
  print(&h);
  
  return 0;
}
