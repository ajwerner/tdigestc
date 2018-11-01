#include <emscripten.h>
#include <stdlib.h>
#include "tdigest.h"

EMSCRIPTEN_KEEPALIVE 
td_histogram_t *newHistogram(double compression) {
     return td_new(compression);
}

EMSCRIPTEN_KEEPALIVE
void freeHistogram(td_histogram_t *h) {
     td_free(h);
}

EMSCRIPTEN_KEEPALIVE
double quantileOf(td_histogram_t *h, double val) {
     return td_quantile_of(h, val);
}

EMSCRIPTEN_KEEPALIVE
void addValue(td_histogram_t *h, double mean, double count) {
     td_add(h, mean, count);
}

EMSCRIPTEN_KEEPALIVE
double valueAt(td_histogram_t *h, double q) {
     return td_value_at(h, q);
}

EMSCRIPTEN_KEEPALIVE
void merge(td_histogram_t *into, td_histogram_t *from) {
     td_merge(into, from);
}
