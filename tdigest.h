// Copyright (c) 2018 Andrew Werner, All rights reserved.

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  size_t size;
  void (*sum)(void *n, void *left, void *right);
  int (*cmp)(void *a, void *b);
} sum_tree_node_type_t;

typedef struct sum_tree sum_tree_t;

*sum_tree_t sum_tree_new(sum_tree_node_type_t type);
void sum_tree_assoc(
void sum_tree_delete(voud *data);
void sum_tree_free(sum_tree);
