// Copyright (c) 2018 Andrew Werner, All rights reserved.

// Package tdigest provides a wrapper to native C TDigest library
package tdigest

// #cgo LDFLAGS: -lm
// #include "tdigest.h"
import "C"
import (
	"fmt"
	"runtime"
)

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

func (t *Histogram) InnerMean(innerQ float64) float64 {
	if innerQ > 1 || innerQ < 0 {
		panic(fmt.Errorf("InnerMean: innerQ must be between 0 and 1, got %v", innerQ))
	}
	outer := 1 - innerQ
	lo := outer / 2
	hi := 1 - lo
	return float64(C.td_trimmed_mean(t.p, C.double(lo), C.double(hi)))
}

func (t *Histogram) TrimmedMean(lo, hi float64) float64 {
	if lo < 0 || lo > 1 || hi < 0 || hi > 1 || lo > hi {
		panic(fmt.Errorf("TrimmedMean: illegal arguments, lo and hi must be in [0,1] and lo < hi, got %v and %v", lo, hi))
	}
	return float64(C.td_trimmed_mean(t.p, C.double(lo), C.double(hi)))
}

func (t *Histogram) TotalCount() float64 {
	return float64(C.td_total_count(t.p))
}

func (t *Histogram) TotalSum() float64 {
	return float64(C.td_total_sum(t.p))
}

func (t *Histogram) Reset() {
	C.td_reset(t.p)
}

func (t *Histogram) Decay(factor float64) {
	C.td_decay(t.p, C.double(factor))
}
