function match(a, b) {
  let percentiles = [0, .001, .01, .02, .1, .2, .3, .4, .5, .6, .7, .8, .9, .98, .99, .999, 1];
  for (i in percentiles) {
    let p = percentiles[i];
    let precision = (p < .01 || p > .99) ? 3
        : (p < .1 || p > .9) ? 2 
        : 1;
    expect(a.valueAt(p)).toBeCloseTo(b.percentile(p), precision);
  }
}

describe("TDigest Basics", function () {
  it("is this thing on?", function () {
    const TDigest = require('./tdigest.js').TDigest;
    let td = new TDigest(100);
    expect(td).toBeDefined();
    expect(td.valueAt(1)).toBeNaN();
    td.push(1);
    expect(td.valueAt(0)).toBe(1);
    expect(td.valueAt(.99)).toBe(1);
    expect(td.valueAt(1)).toBe(1);
    td.push(2);
    expect(td.valueAt(0)).toBe(1);
    expect(td.valueAt(1)).toBe(2);
    expect(td.valueAt(.5)).toBe(1.5);
    td.push(3);
    expect(td.valueAt(.5)).toBe(2);
    expect(td.isDestroyed()).toBe(false);
    td.destroy();
    expect(td.isDestroyed()).toBe(true);
    expect(function() { td.push(0); }).toThrow();
  });
  it("compare to tdigest", function () {
    const TDigest = require('./tdigest.js').TDigest;
    const Other = require("tdigest").TDigest;
    let a = new TDigest(100);
    let b = new Other();
    let N = 100000;
    for (i = 0; i < N; i++) {
      let v = Math.random();
      a.push(v);
      b.push(v);
    }
    match(a, b);
  });
})
