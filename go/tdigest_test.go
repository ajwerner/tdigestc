package tdigest

import (
	"math/rand"
	"testing"
	"strconv"
)

func TestTDigest(t *testing.T) {
	h := New(123)
	h.Add(1, 1)
	h.Add(2, 1)
	if val := h.ValueAt(0); val != 1 {
		t.Fatalf("Unexpected quantile of large value: got %v, expected %v",
			val, 1)

	}
}

func benchmarkAddSize(size int) func(*testing.B) {
	return func(b *testing.B) {
		h := New(size)
		data := make([]float64, 0, b.N)
		for i := 0; i < b.N; i++ {
			data = append(data, rand.Float64())
		}
		b.ResetTimer()
		for _, v := range data {
			h.Add(v, 1)
		}
	}
}

func BenchmarkAdd(b *testing.B) {
	for _, size := range []int{10, 100, 200, 500, 1000, 5000, 10000, 50000} {
		b.Run(strconv.Itoa(size), benchmarkAddSize(size))
	}
}
