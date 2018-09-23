#pragma once

////////////////////////////////////////////////////////////////////////////////
// Centroid Tree
//
// Copyright (c) 2018 Andrew Werner, All rights reserved.
//
// A centroid_tree is a binary search tree which keeps
// track of counts associated with values. Internally
// the tree keeps sums of counts in children. This
// enables efficeint travseral of nodes between a certain
// count sum. The values in this data tree are doubles.
// The counts are also going to be doubles. Notice that
// if the counts were to always be the integer value 1
// for each node, this data structure devoles into a rank-
// order set for doubles. In more traditional tree
// parlance, you can think of this as being a ordered
// map of double to double w
//
// The tree is somewhat uncommon in that it allows for
// a node with the same value multiple times.
// After any mutation, the relative ordering between nodes
// of the val is not stable.
//
// The tree is not intended to get gigantic. It allows at
// most 1 << 31 elements to be added.
//
// The implementation is directly derived from:
//  https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
//
////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef struct ct_tree ct_tree_t;

typedef struct  {
  double val;
  double count;
} ct_centroid_t;

// Constructors:

// ct_new creates a new centroid_tree using dynamic allocation.
// It should only fail to insert if it cannot allocate more memory (which is
// pretty unlikely). The case where failure can occur is only insert which
// returns a boolean to report failure.
// 
// The caller must call ct_free to destroy the tree.
ct_tree_t *ct_new();

// ct_free frees a centroid_tree allocated through new.
void ct_free(ct_tree_t *t);

// ct_alloc creates a new centroid_tree inside of a buffer.
// The tree may fail to add new centroids if there is no more space.
//
// It is the cleint's responsibility to free the memory in buf after the
// returned tree will no longer be used.
ct_tree_t *ct_init(char buf[], size_t buf_size);

// Mutations

// ct_add adds a centroid to the tree 
ct_centroid_t *ct_create(ct_tree_t *t, ct_centroid_t new);

// ct_update will update the value of the current centroid in the tree
// TODO: what if cur not in t? add a bool?
void ct_update(ct_tree_t *t, ct_centroid_t *cur, ct_centroid_t new);

// ct_delete will delete to_delete from t
// TODO: what if to_delete not in t? add a bool?
void ct_delete(ct_tree_t *t, ct_centroid_t *to_delete);

// Queries
// NOTE: queries inputs are only valid between mutations of the tree.
// It is undefined behavior to use a Centroid pointer 

ct_centroid_t *ct_next(ct_centroid_t *cur);
ct_centroid_t *ct_prev(ct_centroid_t *cur);

ct_centroid_t *ct_min(ct_tree_t *t);
ct_centroid_t *ct_max(ct_tree_t *t);
ct_centroid_t *ct_floor(ct_tree_t *t, double val);
ct_centroid_t *ct_ceil(ct_tree_t *t, double val);
ct_centroid_t *ct_sum_floor(ct_tree_t *t, double sum);
ct_centroid_t *ct_sum_ceil(ct_tree_t *t, double sum);

int ct_len(ct_tree_t *t);

bool ct_is_bst(ct_tree_t *t);
