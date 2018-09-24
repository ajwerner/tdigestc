#include <assert.h>
#include <stdlib.h>

#include "node.h"
#include "node_list.h"

// node_list is a list of nodes which maintains a free list as a doubly-linked
// list

static void node_list_init(node_list_t *, int cap);

node_list_t *node_list_from_node(node_t *n) 
{
     return (node_list_t *)((char *)(n - n->idx) - sizeof(node_list_t));
}

node_list_t *node_list_new(int cap) 
{
     node_list_t *l = (node_list_t *)(malloc(sizeof(node_list_t) + (cap * sizeof(node_t))));
     node_list_init(l, cap);
     return l;
}

void node_list_free(node_list_t *nl) {
     free(nl);
}

void node_list_put(node_t *n) {
     n->parent = IDX_NULL;
     n->c = (ct_centroid_t) { 0, 0 };
     n->sum = 0;
     node_list_t *nl = node_list_from_node(n);
     if (nl->len == nl->cap) {
          assert(nl->free == IDX_NULL);
          n->left = n->idx;
          n->right = n->idx;
     } else {
          node_t *r = node_list_at(nl, nl->free);
          node_t *l = left(r);
          set_left(r, n);
          set_right(l, n);
          set_left(n, l);
          set_right(n, r);
     }
     nl->len--;
     nl->free = n->idx;
}

node_t *node_list_get(node_list_t *nl) 
{
     if (nl->free == IDX_NULL) {
          return NULL;
     }
     node_t *n = node_list_at(nl, nl->free);
     nl->len++;
     if (nl->len == nl->cap) {
          assert(n->right == n->idx);
          nl->free = IDX_NULL;
     } else {
          node_t *r = right(n);
          node_t *l = left(n);
          set_right(l, r);
          set_left(r, l);
          nl->free = r->idx;
     }
     n->right = IDX_NULL;
     n->left = IDX_NULL;
     n->parent = IDX_NULL;
     return n;
}

static void node_list_init(node_list_t *l, int cap) 
{
     *l = (node_list_t) { 
          .len = 0,
          .cap = cap,
          .free = 0,
     };
     for (node_idx_t i = 0; i < l->cap; i++) {
          // initialize the free list
          l->nodes[i] = (node_t) {
               .c = { 0, 0 },
               .sum = 0,
               .idx = i,
               .parent = IDX_NULL,
               .left = i == 0 ? cap-1 : i-1,
               .right = (i+1) == cap ? 0 : i+1,
          };
     }
}

node_t *node_list_at(node_list_t *l, node_idx_t idx) 
{
     if (idx == IDX_NULL) {
          return NULL;
     }
     return &(l->nodes[idx]);
}
