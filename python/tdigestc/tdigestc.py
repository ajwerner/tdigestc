from ctypes import CDLL, c_void_p, c_double
from os.path import dirname, join, exists
def wrap_func(lib, func_name, restype, argtypes):
    f = getattr(lib, func_name)
    f.restype = restype
    f.argtypes = argtypes
    return f

def accumulate(vals, func):
    prev=None
    for v in vals:
        prev = func(prev, v) if prev is not None else v
        yield prev

PATH="python/tdigestc/tdigest.so"
places=list(accumulate(reversed(PATH.split("/")),
                       lambda a, b: join(b, a)))
libtdigest=None
for place in places:
    if exists(place):
        libtdigest = CDLL(place)
        break
if libtdigest is None:
    raise RuntimeError("can't find so")

td_new = wrap_func(libtdigest, "td_new", c_void_p, [c_double])
td_free = wrap_func(libtdigest, "td_free", None, [c_void_p])
td_add = wrap_func(libtdigest, "td_add", None, [c_void_p, c_double, c_double])
td_value_at = wrap_func(libtdigest, "td_value_at", c_double, [c_void_p, c_double])

class TDigest(object):
    
    def __init__(this, compression):
        this._as_parameter_ = td_new(compression)
    
    def add(this, value):
        td_add(this, value, 1)

    def value_at(this, q):
        return td_value_at(this, q)
    
    def __del__(this):
        if this._as_parameter_ is not None:
            td_free(this._as_parameter_)
            this._as_parameter_ = None
