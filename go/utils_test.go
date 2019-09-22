package tdigest_test

import (
	"fmt"
	"strconv"
	"strings"
	"testing"

	"github.com/ajwerner/tdigest"
	"github.com/ajwerner/tdigest/testutils/accuracytest"
	tdigestc "github.com/ajwerner/tdigestc/go"
)

func TestAccuracy2(t *testing.T) {

	tests := accuracytest.MakeTests(accuracytest.CombineOptions(
		[]accuracytest.Option{accuracytest.N(100000)},
		accuracytest.Distributions,
		accuracytest.Orders,
		accuracytest.Constructors(
			"tdigestc",
			func(opts ...tdigest.Option) tdigest.Sketch {
				compression := 128.0
				const compressionStr = "compression="
				for _, o := range opts {
					if str := fmt.Sprint(o); strings.HasPrefix(str, compressionStr) {
						var err error
						compression, err = strconv.ParseFloat(strings.TrimPrefix(str, compressionStr), 64)
						if err != nil {
							t.Error(err)
						}
						break
					}
				}
				fmt.Println(compression)
				return tdigestc.New(int(compression))
			},
			[][]tdigest.Option{
				{tdigest.Compression(16)},
				{tdigest.Compression(64)},
				{tdigest.Compression(128)},
				{tdigest.Compression(256)},
				{tdigest.Compression(512)},
			},
		),
	)...)
	for _, at := range tests {
		t.Run(at.String(), func(t *testing.T) { at.Run(t) })
	}
}
