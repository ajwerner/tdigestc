#include <emscripten.h>
#include "tdigest.h"

EMSCRIPTEN_KEEPALIVE 
td_histogram_t *new(double compression, int size) {
     return td_new(compression, size);
}

EMSCRIPTEN_KEEPALIVE
void free(td_histogram_t *h) {
     td_free(h);
}

EMSCRIPTEN_KEEPALIVE
void add(td_histogram_t *h, double mean, double count) {
     td_add(h, mean, count);
}

EMSCRIPTEN_KEEPALIVE
double valueAt(td_histogram_t *h, double q) {
     return td_value_at(h, q);
}

