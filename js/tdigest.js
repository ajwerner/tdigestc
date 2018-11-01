(function() {
  const td = require('./tdigest_native.js');
  td().then(function(tdigest) {
    const newHistogram = tdigest.cwrap("newHistogram", "number", ["number"]);
    const addValue = tdigest.cwrap("addValue", null, ["number", "number", "number"]);
    const valueAt = tdigest.cwrap("valueAt", "number", ["number", "number"]);
    const freeHistogram = tdigest.cwrap("freeHistogram", null, ["number"]);
    const quantileOf = tdigest.cwrap("quantileOf", "number", ["number", "number"]);
    const merge = tdigest.cwrap("merge", null, ["number", "number"]);
    const exists = function(td) {
      if (td.isDestroyed()) {
        throw "cannot use destroyed digest";
      }
    }
    const TDigest = class {
      constructor(n) {
        this.data = newHistogram(n);
      }
      add(v, count) {
        exists(this);
        if (count === undefined) {
          count = 1;
        }
        addValue(this.data, v, count);
      }
      push(v) {
        this.add(v);
      }
      valueAt(q) {
        exists(this);
        return valueAt(this.data, q);
      }
      quantileOf(val) {
        exists(this);
        return quantileOf(this.data, val);
      }
      merge(other) {
        exists(this);
        merge(this.data, other.data);
      }
      destroy() {
        exists(this);
        freeHistogram(this.data);
        this.data = undefined;
      }
      isDestroyed() {
        return (this.data === undefined);
      }
    }
    module.exports = {
      TDigest: TDigest,
    }
  })
})();
