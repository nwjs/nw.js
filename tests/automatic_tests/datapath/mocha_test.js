var assert = require('assert');
var fs = require('fs');

describe('datapath', function() {
	var package_info = JSON.parse(fs.readFileSync('package.json', 'utf8'));
	var info = package_info["chromium-args"].split(" ");
  it("setting datapath for chromium should pass",
    function() {
      assert.equal("--data-path='./'", info[1]);
    }
  )
})
