#pragma once
#include "node.h"

typedef struct node_list {
  int cap;
  int len;
  node_idx_t free;
  node_t nodes[0];
} node_list_t;

node_list_t *node_list_new(int cap);
void node_list_free(node_list_t *);

node_t *node_list_at(node_list_t *l, node_idx_t idx);
node_t *node_list_get(node_list_t *);

// node_list_put puts the node back on the free list
// it assumes that the node came from a node_list_t *
void node_list_put(node_t *);


