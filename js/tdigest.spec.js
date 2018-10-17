

describe("A suite", function () {
  it("is this thing on?", function () {
    var tdigest = require('./tdigest.js');
    var a = 1;
    var b = tdigest._new(10, 1000);
    tdigest._add(b, 1, 1);
    tdigest._add(b, 2, 1);
    expect(tdigest._valueAt(b, .5)).toBe(1.5);
  })
})
