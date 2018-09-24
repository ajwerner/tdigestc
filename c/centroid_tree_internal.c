#include <assert.h>

#include "node_list.h"
#include "centroid_tree_internal.h"

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
     assert(is_bst(n, NULL, NULL));
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

node_t *add(node_t *n, node_t *to_add) 
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
          (p->c.val == new.val && p->idx < cur->idx)) &&
         ((n == NULL) ||
          (n->c.val > new.val) ||
          (n->c.val == new.val && n->idx > cur->idx))) {
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
