var assert = require('assert');
var exec = require('child_process').exec;
var fs = require('fs-extra');
var path = require('path');
var curDir = fs.realpathSync('.');
var func = require(path.join(curDir, '..', 'start_app', 'script.js'));
var is_chromedriver_exists = false;
var result = [];
var tmpnwPath = path.join(curDir, 'tmp-nw');

describe('chromedriver2_server', function() {
  this.timeout(0);

  before(function(done) {
    var srcFile;
    var dstFile;
    if (process.platform == 'win32'){
      srcFile = path.join(curDir, 'chromedriver2_server.exe');
      dstFile = path.join(curDir, 'tmp-nw', 'chromedriver2_server.exe');
    } else {
      srcFile = path.join(curDir, 'chromedriver2_server');
      dstFile = path.join(curDir, 'tmp-nw', 'chromedriver2_server');
    }
    if (!fs.existsSync(srcFile)) {
      is_chromedriver_exists = false;
      done();
    } else {
      func.copyExecFiles(function() {
        fs.copySync(srcFile, dstFile);
		exec('python ' + path.join(curDir, 'chromedriver2_server_test.py'),
		  function (error, stdout, stderr) {
			result.push(error);
			result.push(stdout);
			result.push(stderr);
			done();
		 });  // exec
      });  // copyExecfiles
    }

  });

  after(function () {
    if (fs.existsSync(tmpnwPath)) {
      fs.removeSync(tmpnwPath);
    }
  });

  it('should work' ,function(done) {
	if (!is_chromedriver_exists) {
	  done('chromedrier2_server does not exist');
    } else {
      assert.equal(result[0], null);
      assert.equal(result[1], 'pass\n');
      assert.equal(result[2], '');
      done();
	}
  });

});


