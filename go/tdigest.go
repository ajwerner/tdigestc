// Copyright (c) 2018 Andrew Werner, All rights reserved.

// Package tdigest provides a wrapper to native C TDigest library
package tdigest

// #cgo LDFLAGS: -lm
// #include "tdigest.h"
import "C"
import "runtime"

type Histogram struct {
	p *C.td_histogram_t
}

func New(size int) *Histogram {
	t := &Histogram{p: C.td_new(C.double(size))}
	if t.p == nil {
		panic("Failed to allocate")
	}
	runtime.SetFinalizer(t, free)
	return t
}

func free(t *Histogram) {
	C.td_free(t.p)
}

func (t *Histogram) Record(val float64) {
	C.td_add(t.p, C.double(val), C.double(1))
}

func (t *Histogram) Add(val, count float64) {
	C.td_add(t.p, C.double(val), C.double(count))
}

func (t *Histogram) ValueAt(q float64) float64 {
	return float64(C.td_value_at(t.p, C.double(q)))
}

func (t *Histogram) Merge(other *Histogram) {
	C.td_merge(t.p, other.p)
}

func (t *Histogram) QuantileOf(val float64) float64 {
	return float64(C.td_quantile_of(t.p, C.double(val)))
}
