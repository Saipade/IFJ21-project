#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

typedef struct { // binary search tree struct
  int symbol;
  symbol_BST *Left;
  symbol_BST *Right;
} symbol_BST;

typedef struct { // pointer to head of binary search tree
  symbol_BST *head;
} BST;

enum data_type {
  DT_INTEGER,
  DT_NUMBER,
  DT_STRING,
};

void BST_init (BST* bst);
void BST_insert_node ()
