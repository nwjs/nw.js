var assert = require('assert');

describe('chromium-args', function() {
  it("window.Intl should be undefined beacuse disabled in package.json",
    function() {
      assert.equal(window.Intl, undefined);
    }
  )
})
