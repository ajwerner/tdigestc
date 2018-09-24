#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "centroid_tree.h"

////////////////////////////////////////////////////////////////////////////////
// node operations
////////////////////////////////////////////////////////////////////////////////

typedef unsigned long node_idx_t;

// empty_mask is used to mark the tree as initially empty.
// #define NO_ALLOC_MASK 0x80000000
// #define CAP_MASK      0x7FFFFFFF
#define IDX_NULL   0xFFFFFFFF
#define IDX_MASK   0x7FFFFFFF
#define RED_MASK   0x80000000

typedef struct node {  
  ct_centroid_t c;
  double sum;
  node_idx_t idx;
  node_idx_t left;
  node_idx_t right;
  node_idx_t parent;
} node_t;

static inline node_idx_t idx(node_t *);

static bool less(node_t *, node_t *);

static node_t *parent(node_t *);
static bool has_parent(node_t *);
static void set_parent(node_t *, node_t *);

static node_t *left(node_t *);
static inline bool has_left(node_t *);
static void set_left(node_t *, node_t *);

static node_t *right(node_t *);
static inline bool has_right(node_t *);
static void set_right(node_t *, node_t *);

static inline bool is_red(node_t *);
static void set_is_red(node_t *, bool);

////////////////////////////////////////////////////////////////////////////////
// node list things
////////////////////////////////////////////////////////////////////////////////

typedef struct node_list {
  int cap;
  int len;
  node_idx_t free;
  node_t nodes[0];
} node_list_t;

node_list_t *node_list_new(int cap);
void node_list_free(node_list_t *nl);

node_t *node_list_at(node_list_t *nl, node_idx_t idx);
node_t *node_list_get(node_list_t *n);
node_list_t *node_list_from_node(node_t *n);

// node_list_put puts the node back on the free list
// it assumes that the node came from a node_list_t *
void node_list_put(node_t *n);

////////////////////////////////////////////////////////////////////////////////
// tree operations
////////////////////////////////////////////////////////////////////////////////

static node_t *min(node_t *);
static node_t *max(node_t *);
static node_t *next(node_t *);
static node_t *prev(node_t *);

static node_t *add(node_t *n, node_t *to_add);
static node_t *del(node_t *n, node_t *to_del);
static node_t *del_min(node_t *n);
static node_t *update(node_t *root, node_t *to_update, ct_centroid_t new);

static node_t *fix_up(node_t *);
static node_t *rotate_left(node_t *);
static node_t *rotate_right(node_t *);
static void color_flip(node_t *);
static node_t *move_red_right(node_t *);
static node_t *move_red_left(node_t *);

static bool is_bst(node_t *n, double *min, double *max);

#define DEFAULT_SIZE 32

typedef struct ct_tree {
     node_idx_t root;
     node_list_t *nodes;
} ct_tree_t;


static inline node_t *root(ct_tree_t *t)
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

ct_tree_t *ct_new_with_size(int size)
{
     node_list_t *l = node_list_new(size);
     if (l == NULL) {
          return NULL;
     }
     return ct_new_with_node_list(l);
}

ct_tree_t *ct_new(void) 
{  
     return ct_new_with_size(DEFAULT_SIZE);
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
     t->root = idx(r);
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

////////////////////////////////////////////////////////////////////////////////
// Node impl
////////////////////////////////////////////////////////////////////////////////

static inline bool less(node_t *a, node_t *b) 
{
  if (a->c.val == b->c.val) {
    return idx(a) < idx(b);
  }
  return a->c.val < b->c.val;
}

static inline bool has_parent(node_t *n)  {
  return n ? n->parent != IDX_NULL : false;
}

static inline bool has_left(node_t *n) {
  return n ? n->left != IDX_NULL : false;
}

static inline bool has_right(node_t *n) {
  return n ? n->right != IDX_NULL : false;
}

static inline bool is_red(node_t *n) {
  return n ? ((n->idx&RED_MASK) > 0) : false;
}

static inline void set_is_red(node_t *n, bool red) {
  if (n) {
    if (red) {
      n->idx |= RED_MASK;
    } else {
      n->idx &= IDX_MASK;
    }
  }
}

static inline node_idx_t idx(node_t *n) 
{
     return n->idx&IDX_MASK;
}

static inline node_t *parent(node_t *n) {
  return (has_parent(n)) ? (n - idx(n)) + n->parent : NULL;
}

static inline node_t *left(node_t *n) {
  return (has_left(n)) ? (n - idx(n)) + n->left : NULL;
}

static inline node_t *right(node_t *n) {
  return (has_right(n)) ? (n - idx(n)) + n->right : NULL;
}


static inline void set_parent(node_t *child,
                node_t *parent) 
{
  if (child) {
    if (parent) {
      child->parent = idx(parent);
    } else {
      child->parent = IDX_NULL;
    }
  }
}

static inline void set_left(node_t *parent,
              node_t *child) {
  if (parent) {
    if (child) {
      parent->left = idx(child);
      set_parent(child, parent);
    } else {
      parent->left = IDX_NULL;
    }
  }
}

static inline void set_right(node_t *parent,
               node_t *child) {
  if (parent) {
    if (child) {
      parent->right = idx(child);
      set_parent(child, parent);
    } else {
      parent->right = IDX_NULL;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// node list
////////////////////////////////////////////////////////////////////////////////

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
          // assert(nl->free == IDX_NULL);
          n->left = idx(n);
          n->right = idx(n);
     } else {
          node_t *r = node_list_at(nl, nl->free);
          node_t *l = left(r);
          set_left(r, n);
          set_right(l, n);
          set_left(n, l);
          set_right(n, r);
     }
     nl->len--;
     nl->free = idx(n);
}

node_t *node_list_get(node_list_t *nl) 
{
     if (nl->free == IDX_NULL) {
          return NULL;
     }
     node_t *n = node_list_at(nl, nl->free);
     nl->len++;
     if (nl->len == nl->cap) {
          // assert(n->right == idx(n));
          nl->free = IDX_NULL;
     } else {
          node_t *r = right(n);
          node_t *l = left(n);
          set_right(l, r);
          set_left(r, l);
          nl->free = idx(r);
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


////////////////////////////////////////////////////////////////////////////////
// tree impl
////////////////////////////////////////////////////////////////////////////////

node_t *rotate_left(node_t *n)
{
     node_t *x = right(n);
     set_right(n, left(x));
     set_left(x, n);
     set_is_red(x, is_red(left(x)));
     set_is_red(left(x), true);
     set_parent(x, NULL);
     return x;
}

node_t *rotate_right(node_t *n)
{
     node_t *x = left(n);
     set_left(n, right(x));
     set_right(x, n);
     set_is_red(x, is_red(right(x)));
     set_is_red(right(x), true);
     set_parent(x, NULL);
     return x;
}

void color_flip(node_t *n)
{
     set_is_red(n, !is_red(n));
     set_is_red(right(n), !is_red(right(n)));
     set_is_red(left(n), !is_red(left(n)));
}

node_t *move_red_right(node_t *n) 
{
     color_flip(n);
     if (is_red(left(left(n)))) {
          n = rotate_right(n);
          color_flip(n);
     }
     return n;
}

node_t *move_red_left(node_t *n) 
{
     color_flip(n);
     if (is_red(left(right(n)))) {
          node_t *r = rotate_right(right(n));
          set_right(n, r);
          n = rotate_left(n);
          color_flip(n);    
     }
     return n;
}

inline node_t *fix_up(node_t *n) 
{
     if (is_red(right(n))) {
          n = rotate_left(n); 
     }
     if (is_red(left(n)) && is_red(left(left(n)))) {
          n = rotate_right(n);
     }
     if (is_red(left(n)) && is_red(right(n))) {
          color_flip(n);
     }
     double sum = n->c.count;
     node_t *l = left(n);
     if (l) {
          sum += l->sum;
     }
     node_t *r = right(n);
     if (r) {
          sum += r->sum;
     }
     n->sum = sum;
     // assert(is_bst(n, NULL, NULL));
     return n;
}

node_t *min(node_t *cur) 
{
     if (has_left(cur)) {
          return min(left(cur));
     }
     return cur;
}

node_t *max(node_t *cur) 
{
     if (has_right(cur)) {
          return max(right(cur));
     }
     return cur;
}

node_t *next(node_t *cur) 
{
     if (has_right(cur)) {
          return min(right(cur));
     }
     node_t *p = parent(cur);
     while (p != NULL) {
          if (less(cur, p)) {
               return p;
          }
          cur = p;
          p = parent(p);
     }
     return NULL;
}

node_t *prev(node_t *cur) 
{
     if (has_left(cur)) {
          return max(left(cur));
     }
     node_t *p = parent(cur);
     while (p != NULL) {
          if (less(p, cur)) {
               return p;
          }
          cur = p;
          p = parent(p);
     } 
     return NULL;
}

node_t *del_min(node_t *n) 
{
     if (!has_left(n)) {
          node_list_put(n);
          return NULL;
     }
     if (!is_red(left(n)) && !is_red(left(left(n)))) {
          n = move_red_left(n);
     }
     set_left(n, del_min(left(n)));
     return fix_up(n);
}


node_t *del(node_t *n, node_t *to_del) 
{  
     if (less(to_del, n)) {
          if (!is_red(left(n)) && !is_red(left(left(n)))) {
               n = move_red_left(n);
          }
          set_left(n, del(left(n), to_del));
     } else {
          if (is_red(left(n))) {
               n = rotate_right(n);
          }
          if (n == to_del && !has_right(n)) {
               node_list_put(n);
               return NULL;
          }
          if (!is_red(right(n)) && !is_red(left(right(n)))) {
               n = move_red_right(n);
          }
          if (n == to_del) {
               node_t *r = right(n);
               node_t *s = min(r);
               n->c = s->c;
               set_right(n, del_min(r));
          } else {
               set_right(n, del(right(n), to_del));
          }
     }
     return fix_up(n);
}

inline node_t *add(node_t *n, node_t *to_add) 
{
     if (!n) {
          set_is_red(to_add, true);
          return fix_up(to_add);
     }
     if (less(to_add, n)) {
          set_left(n, add(left(n), to_add));
     } else {
          set_right(n, add(right(n), to_add));
     }
     return fix_up(n);
}

bool is_bst(node_t *n, double *min, double *max) 
{
     if (n == NULL) {
          return true;
     }
     if (min && n->c.val < *min) {
          return false;
     }
     if (max && n->c.val > *max) {
          return false;
     }
     node_t *l = left(n);
     if (l != NULL && !less(l, n)) {
          return false;
     }
     node_t *r = right(n);
     if (r != NULL && !less(n, r)) {
          return false;
     }

     if (parent(r) != NULL &&
         !((left(parent(r)) == r) ||
           (right(parent(r)) == r))) {
          return false;
     }
     return is_bst(l, min, &n->c.val) && is_bst(r, &n->c.val, max);
}

static bool update_fast(node_t *cur, ct_centroid_t new) {
     node_t *p = prev(cur);
     node_t *n = next(cur);
     if (((p == NULL) ||
          (p->c.val < new.val) ||
          (p->c.val == new.val && idx(p) < idx(cur))) &&
         ((n == NULL) ||
          (n->c.val > new.val) ||
          (n->c.val == new.val && idx(n) > idx(cur)))) {
          cur->c = new;
          while (cur) {
               // safe because we know no nodes can move
               fix_up(cur);
               cur = parent(cur);
          }
          return true;
     }
     return false;
}

node_t *update(node_t *root, node_t *to_update, ct_centroid_t new) {
     if (update_fast(to_update, new)) {
          return root;
     }
     node_list_t *nl = node_list_from_node(to_update);
     root = del(root, to_update);
     node_t *to_add = node_list_get(nl);
     to_add->c = new;
     return add(root, to_add);
}
