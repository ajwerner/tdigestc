#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "centroid_tree.h"

typedef struct ct_node {  
  ct_centroid_t c;
  double sum;
  unsigned long idx;
  unsigned long left;
  unsigned long right;
  unsigned long parent;
} ct_node_t;

// empty_mask is used to mark the tree as initially empty.
#define NO_ALLOC_MASK 0x80000000
#define CAP_MASK      0x7FFFFFFF
#define NIL           0xFFFFFFFF
#define PARENT_MASK   0x7FFFFFFF
#define RED_MASK      0x80000000

typedef struct {
  // using uint64_t's to avoid thinking about packing
  // will use some bits in the high order on these as storage for object settings
  unsigned long free; // free is the offset of the first free node
  unsigned long cap;
  unsigned long len;
  unsigned long root;
} ct_head_t;

typedef struct ct_internal_t {
  ct_head_t head;
  ct_node_t nodes[0];
} ct_internal_t;

inline static bool less(ct_node_t *a, ct_node_t *b) {
  if (a->c.val == b->c.val) {
    return a->idx < b->idx;
  }
  return a->c.val < b->c.val;
}

inline static ct_node_t *root(ct_tree_t t) {
  unsigned long root_idx = t->head.root;
  return (root_idx == NIL) ? NULL : &t->nodes[root_idx];
}

inline static ct_tree_t node_tree(ct_node_t *n) {
  return (ct_tree_t)((char *)(n - n->idx) - sizeof(ct_internal_t));
}

inline static bool has_left(ct_node_t *n) {
  return n ? n->left != n->idx : false;
}

inline static bool has_right(ct_node_t *n) {
  return n ? n->right != n->idx : false;
}

inline static bool is_red(ct_node_t *n) {
  return n ? ((n->parent&RED_MASK) > 0) : false;
}

inline static void set_red(ct_node_t *n, bool red) {
  if (n) {
    if (red) {
      n->parent |= RED_MASK;
    } else {
      n->parent &= PARENT_MASK;
    }
  }
}

inline static bool has_parent(ct_node_t *n)  {
  return n ? (n->parent&PARENT_MASK) != n->idx : false;
}

inline static ct_node_t *parent(ct_node_t *n) {
  return (has_parent(n)) ? (n - n->idx) + (n->parent&PARENT_MASK) : NULL;
}

inline static ct_node_t *left(ct_node_t *n) {
  return (has_left(n)) ? (n - n->idx) + n->left : NULL;
}

inline static ct_node_t *right(ct_node_t *n) {
  return (has_right(n)) ? (n - n->idx) + n->right : NULL;
}

inline static void clear_parent(ct_node_t *n) {
  n->parent = n->idx;
}

inline static void clear_right(ct_node_t *n) {
  n->right = n->idx;
}

inline static void clear_left(ct_node_t *n) {
  n->left = n->idx;
}

inline static void set_parent(ct_node_t *child,
                              ct_node_t *parent) {
  if (child) {
    if (parent) {
      child->parent = parent->idx|(RED_MASK&child->parent);      
    } else {
      child->parent = child->idx|(RED_MASK&child->parent);      
    }
  }
}

inline static void set_left(ct_node_t *parent,
                            ct_node_t *child) {
  if (parent) {
    if (child) {
      parent->left = child->idx;

    } else {
      parent->left = parent->idx;
    }
  }
}

inline static void set_right(ct_node_t *parent,
                             ct_node_t *child) {
  if (parent) {
    if (child) {
      parent->right = child->idx;
      // child->parent = (PARENT_MASK&parent->idx)|(RED_MASK&child->parent);
    } else {
      parent->right = parent->idx;
    }
  }
}

static void print_tree(const char *msg, ct_tree_t t) {
  printf(msg);
  dump_tree(t);
  printf("\n");
}

static ct_node_t *rotate_left(ct_node_t *n) {
  print_tree("rotate left before \n", node_tree(n));
  ct_node_t *x = right(n);
  set_right(n, left(x));
  set_left(x, n);
  set_red(x, is_red(left(x)));
  set_red(left(x), true);
  print_tree("rotate left after \n", node_tree(n));
  return x;
}

static ct_node_t *rotate_right(ct_node_t *n) {
  print_tree("rotate right before \n", node_tree(n));
  ct_node_t *x = left(n);
  set_left(n, right(x));
  set_right(x, n);
  set_red(x, is_red(right(x)));
  set_red(right(x), true);
  print_tree("rotate right after \n", node_tree(n));
  return x;
}

static void color_flip(ct_node_t *n) {
  set_red(n, !is_red(n));
  set_red(right(n), !is_red(right(n)));
  set_red(left(n), !is_red(left(n)));
}

static ct_node_t *move_red_right(ct_node_t *n) {
  color_flip(n);
  if (is_red(left(left(n)))) {
    n = rotate_right(n);
    color_flip(n);
  }
  return n;
}

static ct_node_t *move_red_left(ct_node_t *n) {
  color_flip(n);
  if (is_red(left(right(n)))) {
    set_right(n, rotate_right(right(n)));
    n = rotate_left(n);
    color_flip(n);
  }
  return n;
}

void print_node(ct_node_t *n) {
  printf("{ .c = { %f, %f }, .sum = %f, .idx = %lu, .par = %lu, .l = %lu, .r = %lu (is_red: %d)}\n",
         n->c.val, n->c.count, n->sum, n->idx, n->parent&PARENT_MASK, n->left, n->right, is_red(n)? 1 : 0);
}

ct_tree_t *ct_new(void) {
  ct_internal_t *internal = (ct_internal_t *)(malloc(sizeof(ct_internal_t)));
  if (internal == NULL) {
    return NULL;
  }
  *internal = (ct_internal_t) {
    .head = { .cap = 0, .len = 0, .free = NIL, .root = NIL },
  };
  ct_internal_t **t = (ct_internal_t **)(malloc(sizeof(ct_internal_t )));
  if (t == NULL) {
    return NULL;
  }
  *t = internal;
  return (ct_tree_t*)(t);
}

void ct_free(ct_tree_t *t) {
  free(*t);
  free(t);
}

bool ct_realloc(ct_tree_t *t) {
  size_t new_cap = (*t)->head.cap == 0 ? 16 : 2 * (*t)->head.cap;
  size_t new_size = sizeof(ct_internal_t) + (new_cap * sizeof(ct_node_t));
  ct_internal_t *new = (ct_internal_t *)(malloc(new_size));
  if (new == NULL) {
    return false;
  }
  memset((char *)(new), 0, new_size);
  memcpy(new, *t, sizeof(ct_internal_t) + (*t)->head.cap);
  for (unsigned long i = (*t)->head.cap; i < new_cap; i++) {
    new->nodes[i] = (ct_node_t) {
      .c = { 0, 0 },
      .sum = 0.0,
      .idx = i,
      .left = i,
      .parent = i,
      .right = i+1 == new_cap ? i : i + 1,
    };
  }
  new->head.free = (*t)->head.cap;
  new->head.cap = new_cap;
  free(*t);
  *t = new;
  return true;
}

static ct_node_t *new_node(ct_tree_t *t) {
  if ((*t)->head.free == NIL) {
    return NULL;
  }
  ct_node_t *ret = &(*t)->nodes[(*t)->head.free];
  ct_node_t *next = right(ret);
  if (next) {
    (*t)->head.free = next->idx;
  } else {
    (*t)->head.free = NIL;
  }
  ret->left = ret->idx;
  ret->right = ret->idx;
  ret->parent = ret->idx*RED_MASK;
  return ret;
}

static void free_node(ct_node_t *n) {
  ct_tree_t t = node_tree(n);
  *n = (ct_node_t) {
    .c = { 0, 0 },
    .sum = 0.0,
    .idx = n->idx,
    .left = n->idx,
    .parent = n->idx,
    .right = t->head.free == NIL ? n->idx : t->head.free,
  };
  t->head.free = n->idx;
}

ct_tree_t *ct_init(char buf[], size_t buf_size) {
  size_t header_data_size = sizeof(ct_internal_t *) + sizeof(ct_internal_t);
  unsigned long cap = (buf_size - header_data_size) / sizeof(ct_node_t);
  if (cap <= 0) {
    return NULL;
  }
  memset(buf, 0,  buf_size);
  ct_tree_t *t = (ct_tree_t *)(&buf[0]);
  ct_internal_t *internal = (ct_internal_t *)(t+1);
  internal->head.cap = cap|NO_ALLOC_MASK;
  return t;
}

static ct_node_t *add(ct_node_t *n, ct_node_t *to_add);

inline static ct_node_t *fix_up(ct_node_t *n) {
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
  ct_node_t *l = left(n);
  if (l) {
    sum += l->sum;
  }
  ct_node_t *r = right(n);
  if (r) {
    sum += r->sum;
  }
  set_parent(left(n), n);
  set_parent(right(n), n);
  n->sum = sum;
  return n;
}

ct_centroid_t *ct_create(ct_tree_t *t, ct_centroid_t new) {
  print_tree("add before \n", *t);
  // ensure we have enough space or realloc if allowed
  if ((*t)->head.free == NIL) {
    if ((*t)->head.cap&NO_ALLOC_MASK) {
      return NULL;
    }
    if (!ct_realloc(t)) {
      return NULL;
    }
  }
  ct_node_t *n = new_node(t);
  n->c = new;
  n->sum = new.count;
  if ((*t)->head.root == NIL) {
    (*t)->head.root = n->idx;
  } else {
    ct_node_t *r = add(root(*t), n);
    clear_parent(r);
    (*t)->head.root = r->idx;
  }
  (*t)->head.len++;
  print_tree("add after \n", *t);
  return (ct_centroid_t *)(n);
}

static ct_node_t *min(ct_node_t *cur) {
  if (has_left(cur)) {
    return min(left(cur));
  }
  return cur;
}

static ct_node_t *max(ct_node_t *cur) {
  if (has_right(cur)) {
    return max(right(cur));
  }
  return cur;
}

static ct_node_t *next(ct_node_t *cur) {
  if (has_right(cur)) {
    return min(right(cur));
  }
  ct_node_t *p = parent(cur);
  if (p != NULL && left(p) == cur) {
    return p;
  }
  return NULL;
}

static ct_node_t *prev(ct_node_t *cur) {
  if (has_left(cur)) {
    return max(left(cur));
  }
  ct_node_t *p = parent(cur);
  while (p != NULL) {
    if (right(p) == cur) {
      return p;
    }
    cur = p;
    p = parent(p);
  } 
  return NULL;
}

ct_centroid_t *ct_min(ct_tree_t *t) {
  return (ct_centroid_t *)(min(root(*t)));
}

ct_centroid_t *ct_max(ct_tree_t *t) {
  return (ct_centroid_t *)(max(root(*t)));
}

ct_centroid_t *ct_prev(ct_centroid_t *cur) {
  return (ct_centroid_t *)(prev((ct_node_t *)(cur)));
}

ct_centroid_t *ct_next(ct_centroid_t *cur) {
  return (ct_centroid_t *)(next((ct_node_t *)(cur)));
}

static bool update_fast(ct_node_t *cur, ct_centroid_t new) {
  ct_node_t *p = prev(cur);
  ct_node_t *n = next(cur);
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

static ct_node_t *del_min(ct_node_t *n) {
  if (!has_left(n)) {
    free_node(n);
    return NULL;
  }
  if (!is_red(left(n)) && !is_red(left(left(n)))) {
    n = move_red_left(n);
  }
  set_left(n, del_min(left(n)));
  return fix_up(n);
}


static ct_node_t *del(ct_node_t *n, ct_node_t *to_del) {  
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
      free_node(n);
      return NULL;
    }
    if (!is_red(right(n)) && !is_red(left(right(n)))) {
      n = move_red_right(n);
    }
    if (n == to_del) {
      ct_node_t *r = right(n);
      ct_node_t *s = min(r);
      n->c = s->c;
      set_right(n, del_min(r));
    } else {
      set_right(n, del(right(n), to_del));
    }
  }
  return fix_up(n);
}

static void delete(ct_tree_t t, ct_node_t *to_del) {
  ct_node_t *r = del(root(t), to_del);
  if (r) {
    t->head.root = r->idx;
  } else {
    t->head.root = NIL;
  }
  t->head.len--;
}


void ct_delete(ct_centroid_t *to_del) {
  ct_node_t *n = (ct_node_t *)(to_del);
  delete(node_tree(n), n);
}


static void update(ct_node_t *cur, ct_centroid_t new) {
  ct_tree_t t = node_tree(cur);
  delete(t, cur);
  ct_create(&t, new);
}

void ct_update(ct_centroid_t *cur, ct_centroid_t new) {
  if (update_fast((ct_node_t *)(cur), new)) {
    return;
  }
  
  update((ct_node_t *)(cur), new);
}

static ct_node_t *add(ct_node_t *n, ct_node_t *to_add) {
  if (!n) {
    set_red(to_add, true);
    return fix_up(to_add);
  }
  if (less(to_add, n)) {
    set_left(n, add(left(n), to_add));
  } else {
    set_right(n, add(right(n), to_add));
  }
  return fix_up(n);
}

int ct_len(ct_tree_t *t) {
  return (*t)->head.len;
}

static bool is_bst(ct_node_t *n, double *min, double *max) {
  if (n == NULL) {
    return true;
  }
  if (min && n->c.val < *min) {
    return false;
  }
  if (max && n->c.val > *max) {
    return false;
  }
  ct_node_t *l = left(n);
  if (l != NULL && !less(l, n)) {
    return false;
  }
  ct_node_t *r = right(n);
  if (r != NULL && !less(n, r)) {
    return false;
  }
  return is_bst(l, min, &n->c.val) && is_bst(r, &n->c.val, max);
}

void dump_tree(ct_tree_t t) {
  printf("{ .len = %lu, .cap = %lu, .free = %lu, .root = %lu }\n",
         t->head.len, t->head.cap, t->head.free, t->head.root);
  ct_node_t *cur = min(root(t));
  while (cur) {
    print_node(cur);
    cur = next(cur);
  }
}


bool ct_is_bst(ct_tree_t *t) {
  return is_bst(root(*t), NULL, NULL);
}
