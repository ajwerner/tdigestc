from ctypes import CDLL, c_void_p, c_double

def wrap_func(lib, func_name, restype, argtypes):
    f = getattr(lib, func_name)
    f.restype = restype
    f.argtypes = argtypes
    return f
    
libtdigest = CDLL("c/tdigest.so")
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
