#include <emscripten.h>
#include <stdlib.h>
#include "tdigest.h"

EMSCRIPTEN_KEEPALIVE
int requiredBufferSize(double compression) {
     
     return td_buf_size(compression);
}

EMSCRIPTEN_KEEPALIVE 
td_histogram_t *newHistogram(double compression) {
     return td_new(compression);
}

EMSCRIPTEN_KEEPALIVE
td_histogram_t *initHistogram(double compression, size_t buf_size, char *buf) {
     return td_init(compression, buf_size, buf);
}

EMSCRIPTEN_KEEPALIVE
void freeHistogram(td_histogram_t *h) {
     td_free(h);
}

EMSCRIPTEN_KEEPALIVE
void addValue(td_histogram_t *h, double mean, double count) {
     td_add(h, mean, count);
}

EMSCRIPTEN_KEEPALIVE
double valueAt(td_histogram_t *h, double q) {
     return td_value_at(h, q);
}

