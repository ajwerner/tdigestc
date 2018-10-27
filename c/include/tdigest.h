#pragma once

////////////////////////////////////////////////////////////////////////////////
// tdigest
//
// Copyright (c) 2018 Andrew Werner, All rights reserved.
//
// tdigest is an implementation of Ted Dunning's streaming quantile estimation
// data structure. 
// This implementation is intended to be like the new MergingHistogram.
// It focuses on being in portable C that should be easy to integrate into other
// languages. In particular it provides mechanisms to preallocate all memory 
// at construction time.
//
// The implementation is a direct descendent of 
//  https://github.com/tdunning/t-digest/
//
// TODO: add a Ted Dunning Copyright notice.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

typedef struct td_histogram td_histogram_t;

// td_init will initialize a td_histogram_t inside buf which is buf_size bytes.
// If buf_size is too small (smaller than compression + 1) or buf is NULL,
// the returned pointer will be NULL.
//
// In general use td_required_buf_size to figure out what size buffer to
// pass.
td_histogram_t *td_init(double compression, size_t buf_size, char *buf);

size_t td_buf_size(double compression);

// td_new allocates a new histogram.
// It is similar to init but assumes that it can use malloc.
td_histogram_t *td_new(double compression);

// td_free may only be called if the histogram was created with td_new.
void td_free(td_histogram_t *h);

void td_add(td_histogram_t *h, double mean, double count);

double td_value_at(td_histogram_t *h, double q);

double td_quantile_of(td_histogram_t *h, double val);

double td_total_count(td_histogram_t *h);
