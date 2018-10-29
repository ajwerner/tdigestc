# tdigestc

This library is a C implementation of the [t-digest](https://github.com/tdunning/t-digest) data structure for approximate quantile estimation.

It represents an experiment in cross-language development. 
The build tool chosen is [bazel](https://github.com/bazelbuild/bazel)..

The hope was to create an experience with the library where it was 
    * natural to use from all targeted languaged
    * build for each language comfortably with a single build tool
    * produce easily includable artifacts

## C

C is the core implementation. The code comes in 1 .c file with 1 .h file. 
You need to just put this into your project or take a compiled so from a bazel build.
I haven't figured out a better C distribution story but if you have one, let me know.

The API is straight forward.

TODO: add doxygen to the C header

```
#include <assert.h>
#include "tdigest.h"

int main(void) {
    td_histogram_t *h = td_new(100);
    td_add(h, 1);
    td_add(h, 2);
    assert(td_value_at(h, 0) == 1);
    assert(td_value_at(h, .5) == 1.5);
    assert(td_value_at(h, 1) == 2);
    td_histogram_t *h2 = td_new(100);
    td_add(h2, 0);
    td_add(h2, 3);
    td_merge(h, h2);
    assert(td_value_at(h, 0) == 0);
    assert(td_value_at(h, .5) == 1.5);
    assert(td_value_at(h, 1) == 3);
    td_free(h2);
    td_free(h);
}
```

## JS

The javascript integration works pretty well except for one wart.
The memory used by the object will need to manually freed to release the underlying memory.

The API seeks to be very similar to the tdigest npm library that exists.

For the testing I run a test to compare the output from that open source library and this one.

## Python

I don't have a wonderful story for what to do with python.

Python has two different stories for C integration.

One works for pypy and one doesn't. 

The one that doesn't is a liitle bit more annoying to package.
Ideally I'd have a way to construct a fat binary "wheel?"
It's not clear how to cross compile in bazel inside the same execution.
It'd be great to have a PyPI package that one could pip install that just works.
I'm not sure how this is done in a cross platform way.
Bears further investigation.

## Go

For go it's just that the C is symlinked into the directory and it uses cgo.

The wrapper is not a lot of code.
The downside here is the overhead of calling in to cgo but it seems like the
data structure from very preliminary testing is pretty fast.

### How To

```
$ go get github.com/ajwerner/tdigestc/go
```
```[go]
package main

import (
    "math/rand"
    "github.com/ajwerner/tdigestc/go"
)

func main() {
     td := tdigestc.New(100);
     td.Add(1)
     td.Add(2)

}
```

## Java

It's just JNI bindings that I need to write.
I could consider learning JNA or maybe they've released something new.

