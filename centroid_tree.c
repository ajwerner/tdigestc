#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "centroid_tree.h"
#include "node.h"
#include "node_list.h"
#include "centroid_tree_internal.h"

#define DEFAULT_NODE_LIST_SIZE 32

typedef struct ct_tree {
     node_idx_t root;
     node_list_t *nodes;
} ct_tree_t;


static node_t *root(ct_tree_t *t) {
     if (t->root == IDX_NULL) {
          return NULL;
     }
     return node_list_at(t->nodes, t->root);
}

node_t *node_list_at(node_list_t *l, node_idx_t idx) 
{
     if (idx == IDX_NULL) {
          return NULL;
     }
     return &(l->nodes[idx]);
}



void print_node(node_t *n) 
{
     printf("{ .c = { %f, %f }, .sum = %f, .idx = %lu, .par = %lu, .l = %lu, .r = %lu (is_red: %d)}\n",
            n->c.val, n->c.count, n->sum, n->idx, n->parent&PARENT_MASK, n->left, n->right, is_red(n)? 1 : 0);
}

ct_tree_t *ct_new_with_node_list(node_list_t *nl) 
{
     ct_tree_t *tree = (ct_tree_t *)(malloc(sizeof(ct_tree_t)));
     if (tree == NULL) {
          return NULL;
     }
     *tree = (ct_tree_t) { 
          .root = IDX_NULL,
          .nodes = nl,
     };
     return tree;
}

ct_tree_t *ct_new(void) {  
     node_list_t *l = node_list_new(DEFAULT_NODE_LIST_SIZE);
     if (l == NULL) {
          return NULL;
     }
     return ct_new_with_node_list(l);
}

void ct_free(ct_tree_t *t) {
     node_list_free(t->nodes);
     free(t);
}

/* bool ct_realloc(ct_tree_t *t) { */
/*   size_t new_cap = (*t)->head.cap == 0 ? 16 : 2 * (*t)->head.cap; */
/*   size_t new_size = sizeof(ct_internal_t) + (new_cap * sizeof(node_t)); */
/*   ct_internal_t *new = (ct_internal_t *)(malloc(new_size)); */
/*   if (new == NULL) { */
/*     return false; */
/*   } */
/*   memset((char *)(new), 0, new_size); */
/*   memcpy(new, *t, sizeof(ct_internal_t) + (*t)->head.cap); */
/*   for (unsigned long i = (*t)->head.cap; i < new_cap; i++) { */
/*     new->nodes[i] = (node_t) { */
/*       .c = { 0, 0 }, */
/*       .sum = 0.0, */
/*       .idx = i, */
/*       .left = i, */
/*       .parent = i, */
/*       .right = i+1 == new_cap ? i : i + 1, */
/*     }; */
/*   } */
/*   new->head.free = (*t)->head.cap; */
/*   new->head.cap = new_cap; */
/*   free(*t); */
/*   *t = new; */
/*   return true; */
/* } */

ct_centroid_t *ct_create(ct_tree_t *t, ct_centroid_t new) 
{
     // print_tree("add before \n", *t);
     // ensure we have enough space or realloc if allowed
     node_t *n = node_list_get(t->nodes);
     if (n == NULL) {
          return NULL;
     }
     n->c = new;
     n->sum = new.count;
     node_t *r = add(root(t), n);
     t->root = r->idx;

     return (ct_centroid_t *)(n);
}

ct_centroid_t *ct_min(ct_tree_t *t) {
     return (ct_centroid_t *)(min(root(t)));
}

ct_centroid_t *ct_max(ct_tree_t *t) {
     return (ct_centroid_t *)(max(root(t)));
}

ct_centroid_t *ct_prev(ct_centroid_t *cur) {
     return (ct_centroid_t *)(prev((node_t *)(cur)));
}

ct_centroid_t *ct_next(ct_centroid_t *cur) {
     return (ct_centroid_t *)(next((node_t *)(cur)));
}

int ct_len(ct_tree_t *t) 
{
     node_t *cur = root(t);
     cur = min(cur);
     int n = 0;
     while (cur != NULL) {
          cur = next(cur);
          n++;
     }
     return n;
}

static bool update_fast(node_t *cur, ct_centroid_t new) {
     node_t *p = prev(cur);
     node_t *n = next(cur);
     if (((p == NULL) ||
          (p->c.val < new.val) ||
          (p->c.val == new.val && p->idx < cur->idx)) &&
         ((n == NULL) ||
          (n->c.val > new.val) ||
          (n->c.val == new.val && n->idx > cur->idx))) {
          cur->c = new;
          while (cur) {
               fix_up(cur);
               cur = parent(cur);
          }
          return true;
     }
     return false;
}


void ct_delete(ct_tree_t *t, ct_centroid_t *to_del) {
     node_t *r = del(root(t), (node_t *)(to_del));
     if (r) {
          t->root = r->idx;
     } else {
          t->root = IDX_NULL;
     }
}

static void update(ct_tree_t *t, node_t *cur, ct_centroid_t new) {
     ct_delete(t, (ct_centroid_t *)(cur));
     ct_create(t, new);
}

void ct_update(ct_tree_t *t, ct_centroid_t *cur, ct_centroid_t new) {
     if (update_fast((node_t *)(cur), new)) {
          return;
     }
  
     update(t, (node_t *)(cur), new);
}


bool ct_is_bst(ct_tree_t *t) {
     return is_bst(root(t), NULL, NULL);
}
