(function() {
  let td = require('./tdigest_native.js');
  td().then(function(tdigest) {
    let requiredBufferSize = tdigest.cwrap("requiredBufferSize", "number", ["number"]);
    let newHistogram = tdigest.cwrap("newHistogram", "number", ["number"]);
    let addValue = tdigest.cwrap("addValue", null, ["number", "number", "number"]);
    let valueAt = tdigest.cwrap("valueAt", "number", ["number", "number"]);
    let freeHistogram = tdigest.cwrap("freeHistogram", null, ["number"]);
    let exists = function(td) {
      if (td.isDestroyed()) {
        throw "cannot use destroyed digest";
      }
    }
    let TDigest = class {
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
