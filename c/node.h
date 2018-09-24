#pragma once
#include <stdbool.h>

#include "centroid_tree.h"

typedef unsigned long node_idx_t;

// empty_mask is used to mark the tree as initially empty.
// #define NO_ALLOC_MASK 0x80000000
// #define CAP_MASK      0x7FFFFFFF
#define IDX_NULL      0xFFFFFFFF
#define PARENT_MASK   0x7FFFFFFF
#define RED_MASK      0x80000000

typedef struct node {  
  ct_centroid_t c;
  double sum;
  node_idx_t idx;
  node_idx_t left;
  node_idx_t right;
  node_idx_t parent;
} node_t;

bool less(node_t *, node_t *);

node_t *parent(node_t *);
bool has_parent(node_t *);
void set_parent(node_t *, node_t *);

node_t *left(node_t *);
bool has_left(node_t *);
void set_left(node_t *, node_t *);

node_t *right(node_t *);
bool has_right(node_t *);
void set_right(node_t *, node_t *);

bool is_red(node_t *);
void set_is_red(node_t *, bool);
