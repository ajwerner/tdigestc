package tdigest

// #cgo CFLAGS: -O3
// #cgo LDFLAGS: -lm
// #include "tdigest.h"
import "C"
import (
	"runtime"
)

type Histogram struct {
	p *C.td_histogram_t
}

func New(size int) *Histogram {
	t := &Histogram{p: C.td_new(C.double(size), C.int(5*size+10))}
	if t.p == nil {
		panic("Failed to allocate")
	}
	runtime.SetFinalizer(t, free)
	return t
}

func free(t *Histogram) {
	C.td_free(t.p)
}

func (t *Histogram) Add(val, count float64) {
	C.td_add(t.p, C.double(val), C.double(count))
}

func (t *Histogram) ValueAt(q float64) float64 {
	return float64(C.td_value_at(t.p, C.double(q)))
}

// func (t *Histogram) QuantileOf(val float64) float64 {
// 	return float64(C.td_quantile_of(t.p, C.double(val)))
// }
