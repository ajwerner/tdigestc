package tdigest


// #cgo CFLAGS: -O3
// #include "centroid_tree.h"
import "C"
import (
	"runtime"
)

type Tree struct {
	p *C.ct_tree_t
}

func New(size int) *Tree {
	t := &Tree{p: C.ct_new_with_size(C.int(size))}
	if t.p == nil {
		panic("Failed to allocate")
	}
	runtime.SetFinalizer(t, free)
	return t
}

func free(t *Tree) {
	C.ct_free(t.p)
}

func (t *Tree) Add(val, count float64) *Centroid {
	return (*Centroid)(C.ct_create(t.p, C.ct_centroid_t{
		val:   C.double(val),
		count: C.double(count),
	}))
}

type Centroid C.ct_centroid_t

func (c *Centroid) Count() float64 {
	return float64((C.ct_centroid_t)(*c).count)
}

func (c *Centroid) Val() float64 {
	return float64((C.ct_centroid_t)(*c).val)
}

func (c *Centroid) Next() *Centroid {
	return (*Centroid)(C.ct_next((*C.ct_centroid_t)(c)))
}

func (c *Centroid) Prev() *Centroid {
	return (*Centroid)(C.ct_prev((*C.ct_centroid_t)(c)))
}

func (t *Tree) Min() *Centroid {
	return (*Centroid)(C.ct_min(t.p))
}

func (t *Tree) Max() *Centroid {
	return (*Centroid)(C.ct_max(t.p))
}
