# tdigestc

This library is a C implementation of the [t-digest](https://github.com/tdunning/t-digest) data structure for approximate quantile estimation.

It represents an experiment in cross-language development. 
The build tool chosen is [bazel](https://github.com/bazelbuild/bazel)..

The hope was to create an experience with the library where it was 

* natural to use from all targeted languaged
* build for each language comfortably with a single build tool
* produce easily includable artifacts

It's not clear that all of these goals have been totally acheived.

## C

C is the core implementation. The code comes in 1 .c file with 1 .h file. 
You need to just put this into your project or take a compiled so from a bazel build.
I haven't figured out a better C distribution story but if you have one, let me know.

### Usage

The API is straight forward.

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

### Installation

The package should be installable from (npm)[https://www.npmjs.com/package/tdigestc]

### Usage

The API seeks to be very similar to the tdigest npm library that exists.

For the testing I run a test to compare the output from that open source library and this one.

```[js]
const TDigest = require('./tdigest.js').TDigest;
var td = new TDigest(100);
td.add(1);
td.add(2);
assert(td.valueAt(0) == 1);
assert(td.valueAt(.5) == 1.5);
assert(td.valueAt(1) == 2);
td.destroy(); // this is the wart, an error will occur if td is used after
```
## Python

Python is packaged using ctypes and a bundled shared library.
This has obvious cross platform problems.
The python rules are set up to build and publish to PyPI but that has not yet been all the way set up.

### Usage

```[python]
from tdigestc import TDigest
f = TDigest(100)
f.add(1)
f.add(2)
assert f.value_at(0) == 1
assert f.value_at(.5) == 1.5
assert f.value_at(1) == 2
```

## Go

For go it's just that the C is symlinked into the directory and it uses cgo.

The wrapper is not a lot of code.
The downside here is the overhead of calling in to cgo but it seems like the
data structure from very preliminary testing is pretty fast.

### Installation

```
$ go get github.com/ajwerner/tdigestc/go
```

### Example

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

Java is hooked up through the JNI.
The //java:TDigest target will compile a jar file which contains the TDigest class and self-loading library.

### Usage

```[java]
import com.ajwerner.tdigestc.TDigest;

public class Example {
     public static void main(String[] args) {
          TDigest td = new TDigest(100);
          td.add(1);
          td.add(2);
          assert td.valueAt(0) == 1;
          assert td.valueAt(.5) == 1.5;
          assert td.valueAt(1) == 2;               
     }
}
```

# TODO

* Add doxygen for the C header
* Fix stamping for distribution of NPM and PyPI packages
* Improve python packaging and distribution
* Figure out java packaging and distribution
* Way more testing
* Consider using binary search in queries
