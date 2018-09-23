#include "node.h"

bool less(node_t *a, node_t *b) 
{
  if (a->c.val == b->c.val) {
    return a->idx < b->idx;
  }
  return a->c.val < b->c.val;
}

bool has_parent(node_t *n)  {
  return n ? (n->parent&PARENT_MASK) != (IDX_NULL&PARENT_MASK) : false;
}

bool has_left(node_t *n) {
  return n ? n->left != IDX_NULL : false;
}

bool has_right(node_t *n) {
  return n ? n->right != IDX_NULL : false;
}

node_t *parent(node_t *n) {
  return (has_parent(n)) ? (n - n->idx) + (n->parent&PARENT_MASK) : NULL;
}

node_t *left(node_t *n) {
  return (has_left(n)) ? (n - n->idx) + n->left : NULL;
}

node_t *right(node_t *n) {
  return (has_right(n)) ? (n - n->idx) + n->right : NULL;
}


void set_parent(node_t *child,
                node_t *parent) 
{
  if (child) {
    bool child_is_red = is_red(child);
    if (parent) {
      child->parent = parent->idx;
    } else {
      child->parent = IDX_NULL;
    }
    set_is_red(child, child_is_red);
  }
}

void set_left(node_t *parent,
              node_t *child) {
  if (parent) {
    if (child) {
      parent->left = child->idx;
      set_parent(child, parent);
    } else {
      parent->left = IDX_NULL;
    }
  }
}

void set_right(node_t *parent,
               node_t *child) {
  if (parent) {
    if (child) {
      parent->right = child->idx;
      set_parent(child, parent);
    } else {
      parent->right = IDX_NULL;
    }
  }
}

bool is_red(node_t *n) {
  return n ? ((n->parent&RED_MASK) > 0) : false;
}

void set_is_red(node_t *n, bool red) {
  if (n) {
    if (red) {
      n->parent |= RED_MASK;
    } else {
      n->parent &= PARENT_MASK;
    }
  }
}

