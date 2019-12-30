var assert = require('assert');
var path = require('path');
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
var os = require('os');
var result;

if (os.platform() != 'darwin')
  return;
describe('quit_with_secondary_window_on_top', function() {
    before(function(done) {
        this.timeout(0);
        var app_path = path.join(global.tests_dir, 'quit_with_secondary_window_on_top');
        var exec_path = process.execPath;
        exec_path = exec_path.replace(/ /g, '\\ ');
        exec(exec_path + ' ' + app_path, function(error, stdout, stderr) {
            result = error;
            done();
          });
      })
    it('should quit with secondary window on top without error', function() {
        assert.equal(result, null);
    });
});
