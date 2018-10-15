#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "tdigest.h"

#define M_PI 3.14159265358979323846

// TODO: make a union on count to store the sum of counts below in
// merged nodes to make queries cheaper.

typedef struct node {
     double mean;
     double count;
} node_t;

struct td_histogram {
     // compression is a setting used to configure the size of centroids when merged.
     double compression; 
     
     // cap is the total size of nodes
     int cap;
     // merged_nodes is the number of merged nodes at the front of nodes.
     int merged_nodes;
     // unmerged_nodes is the number of buffered nodes.
     int unmerged_nodes;
     
     double merged_count;
     double unmerged_count;
     
     node_t nodes[0];
};

static inline bool cap_is_okay(double compression, int cap) {
     return (cap > ((2*compression) + 10));
}

static inline bool should_merge(td_histogram_t *h) {
     return ((h->merged_nodes + h->unmerged_nodes) == h->cap);
}

static inline int next_node(td_histogram_t *h) {
     return h->merged_nodes + h->unmerged_nodes;
}

static void merge(td_histogram_t *h);

////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////

td_histogram_t *td_new(double compression, int cap) {
     if (!cap_is_okay(compression, cap)) {
          return NULL;
     }
     size_t memsize = sizeof(td_histogram_t) + (cap * sizeof(node_t));
     td_histogram_t *h = (td_histogram_t *)(malloc(memsize));
     if (!h) {
          return NULL;
     }
     bzero((void *)(h), memsize);
     *h = (td_histogram_t) {
          .compression = compression,
          .cap = cap,
          .merged_nodes = 0,
          .merged_count = 0,
          .unmerged_nodes = 0,
          .unmerged_count = 0,
     };
     return h;
}

void td_free(td_histogram_t *h) {
     free((void *)(h));
}

double td_total_count(td_histogram_t *h) {
     return h->merged_count + h->unmerged_count;
}

double td_value_at(td_histogram_t *h, double q) {
     merge(h);
     if (q < 0 || q > 1 || h->merged_nodes == 0) {
          return NAN;
     }
     // if left of the first node, use the first node
     // if right of the last node, use the last node, use it
     double goal = q * h->merged_count;
     double k = 0;
     int i = 0;
     node_t *n = NULL;
     for (i = 0; i < h->merged_nodes; i++) {
          n = &h->nodes[i];
          if (k + n->count > goal) {
               break;
          }
          k += h->nodes[i].count;
     }
     double delta_k = goal - k - (n->count/2);
     if (delta_k < .000000001 && delta_k > -.000000001) {
          return n->mean;
     }
     bool right = delta_k > 0;
     if ((i == 0 && !right) || ((i+1) == h->merged_nodes && right)) {
          return n->mean;
     }
     node_t *nl;
     node_t *nr;
     if (right) {
          nl = n;
          nr = &h->nodes[i+1];
          k += (n->count/2);
     } else {
          nl = &h->nodes[i-1];
          nr = n;
          k -= (n->count/2);
     }
     double x = goal - k;
     // we have two points (0, nl->mean), (nr->count, nr->mean)
     // and we want x
     double m = (nr->mean - nl->mean) / (nr->count);
     return m * x + nl->mean;
}


void td_add(td_histogram_t *h, double mean, double count) {
     if (should_merge(h)) {
          merge(h);
     }
     h->nodes[next_node(h)] = (node_t) {
          .mean = mean,
          .count = count,
     };
     h->unmerged_nodes++;
     h->unmerged_count += count;
}

static int compar_nodes(const void *v1, const void *v2) {
     node_t *n1 = (node_t *)(v1);
     node_t *n2 = (node_t *)(v2);
     if (n1->mean < n2->mean) {
          return -1;
     } else if (n1->mean > n2->mean) {
          return 1;
     } else {
          return 0;
     }
}

static void merge(td_histogram_t *h) {
     if (h->unmerged_nodes == 0) {
          return;
     }
     int N = h->merged_nodes + h->unmerged_nodes;
     qsort((void *)(h->nodes), N, sizeof(node_t), &compar_nodes);
     double total_count = h->merged_count + h->unmerged_count;
     double denom = 2 * M_PI * total_count * log(total_count);
     double normalizer = h->compression / denom;
     int cur = 0;
     double count_so_far = 0;
     for (int i = 1; i < N; i++) {
          double proposed_count = h->nodes[cur].count + h->nodes[i].count;
          double z = proposed_count * normalizer;
          double q0 = count_so_far / total_count;
          double q2 = (count_so_far + proposed_count) / total_count;
          bool should_add = (z <= (q0 * (1 - q0))) && (z <= (q2 * (1 - q2)));
          if (should_add) {
               h->nodes[cur].count += h->nodes[i].count;
               double delta = h->nodes[i].mean - h->nodes[cur].mean;
               double weighted_delta = (delta * h->nodes[i].count) / h->nodes[cur].count;
               h->nodes[cur].mean += weighted_delta;
          } else {
               count_so_far += h->nodes[cur].count;
               cur++;
               h->nodes[cur] = h->nodes[i];
          }
          if (cur != i) {
               h->nodes[i] = (node_t) {
                    .mean = 0,
                    .count = 0,
               };
          }
     }
     h->merged_nodes = cur+1;
     h->merged_count = total_count;
     h->unmerged_nodes = 0;
     h->unmerged_count = 0;
}
