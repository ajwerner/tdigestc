from tdigestc import *

f = TDigest(100)
f.add(1)
f.add(2)

print(f.value_at(.5))
