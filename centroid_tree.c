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


static node_t *root(ct_tree_t *t)
{
     if (t->root == IDX_NULL) {
          return NULL;
     }
     return node_list_at(t->nodes, t->root);
}

static void set_root(ct_tree_t *t, node_t *root)
{
     if (root) {
          t->root = root->idx;
     } else {
          t->root = IDX_NULL;
     }
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

ct_tree_t *ct_new(void) 
{  
     node_list_t *l = node_list_new(DEFAULT_NODE_LIST_SIZE);
     if (l == NULL) {
          return NULL;
     }
     return ct_new_with_node_list(l);
}

void ct_free(ct_tree_t *t)
{
     node_list_free(t->nodes);
     free(t);
}

ct_centroid_t *ct_create(ct_tree_t *t, ct_centroid_t new) 
{
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

ct_centroid_t *ct_min(ct_tree_t *t) 
{
     return (ct_centroid_t *)(min(root(t)));
}

ct_centroid_t *ct_max(ct_tree_t *t)
{
     return (ct_centroid_t *)(max(root(t)));
}

ct_centroid_t *ct_prev(ct_centroid_t *cur)
{
     return (ct_centroid_t *)(prev((node_t *)(cur)));
}

ct_centroid_t *ct_next(ct_centroid_t *cur)
{
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

void ct_delete(ct_tree_t *t, ct_centroid_t *to_del)
{
     set_root(t,  del(root(t), (node_t *)(to_del)));
}


void ct_update(ct_tree_t *t, ct_centroid_t *cur, ct_centroid_t new) 
{
     set_root(t, update(root(t), (node_t *)(cur), new));
}


bool ct_is_bst(ct_tree_t *t)
{
     return is_bst(root(t), NULL, NULL);
}
