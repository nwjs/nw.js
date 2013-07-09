var assert = require('assert');
var fs = require('fs');
var path = require('path');
var app_test = require('./nw_test_app');
describe('data-path', function() {
  it('setting datapath for chromium should pass',
    function(done) {
      this.timeout(0);

      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath'),
        end: function(data, app) {
        	app.kill();
            assert.equal("--data-path='./'", data);
            done();
        }
      });
  })
})
