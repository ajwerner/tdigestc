package tdigest_test

import (
	"math/rand"
	"testing"

	"github.com/ajwerner/tdigest/go"
)

func BenchmarkInsert(b *testing.B) {
	n := tdigest.New(b.N)
	type centroid struct {
		Count float64
		Val   float64
	}
	d := make([]centroid, 0, b.N)
	for i := 0; i < b.N; i++ {
		d = append(d, centroid{rand.Float64(), rand.Float64()})
	}
	b.ResetTimer()
	for _, c := range d {
		n.Add(c.Val, c.Count)
	}
}

func TestCentroidTree(t *testing.T) {
	n := tdigest.New(123)
	n.Add(2, 2)
	n.Add(1, 4)
	c := n.Min()
	AssertCentroid(t, c, 1, 4)
	c = c.Next()
	AssertCentroid(t, c, 2, 2)
	n.Add(23, 2)
	c = n.Max()
	AssertCentroid(t, c, 23, 2)
	c = c.Prev()
	AssertCentroid(t, c, 2, 2)
	c = c.Prev()
	AssertCentroid(t, c, 1, 4)
	if c = c.Prev(); c != nil {
		t.Fatalf("it wasn't nil?")
	}
}

func AssertCentroid(t *testing.T, c *tdigest.Centroid, val, count float64) {
	if got := c.Count(); got != count {
		t.Fatalf("Min should be %v, got %v", count, got)
	} else if got := c.Val(); got != val {
		t.Fatalf("Val should be %v, got %v", val, got)
	}
}
