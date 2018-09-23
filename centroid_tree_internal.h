#pragma once

#include "node.h"

node_t *min(node_t *);
node_t *max(node_t *);
node_t *next(node_t *);
node_t *prev(node_t *);

node_t *add(node_t *n, node_t *to_add);
node_t *del(node_t *n, node_t *to_del);
node_t *del_min(node_t *n);

node_t *fix_up(node_t *);
node_t *rotate_left(node_t *);
node_t *rotate_right(node_t *);
void color_flip(node_t *);
node_t *move_red_right(node_t *);
node_t *move_red_left(node_t *);

bool is_bst(node_t *n, double *min, double *max);
