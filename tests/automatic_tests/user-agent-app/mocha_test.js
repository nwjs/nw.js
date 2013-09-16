var assert = require('assert');
var fs = require('fs');
var path = require('path');
var app_test = require('./nw_test_app');
describe('user-agent-app', function() {
  it('user agent shoud be the same when open a new window',
    function(done) {
      this.timeout(0);
      var result = false;

      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'user-agent-app'),
        end: function(data, app) {
			result = true;
			var package_info = JSON.parse(fs.readFileSync('automatic_tests/user-agent-app/package.json', 'utf8'));
			app.kill();
      if (data == package_info.name) {
				done();
			}
			else{
				done('the user agent has changed');
			}
        }
      });
      setTimeout(function(){
          if (!result) {
            child.close();
            done('loaded event does not been fired');
          }
        }, 10000);
  })
})
