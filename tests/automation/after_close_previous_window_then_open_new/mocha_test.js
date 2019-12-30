var path = require('path');
var app_test = require('../../nw_test_app');
var assert  =require('assert');
var fs = require('fs');
var fs_extra = require('fs-extra');
var spawn = require('child_process').spawn;
var global = require('../globals');
var result;

var dumpDir = path.join(global.tests_dir, 'internal', 'tmp');
console.log('dumpDir:' + dumpDir);

describe('after close previous window then open new', function() {
    after(function () {
    	fs_extra.remove(dumpDir, function (er) {
            if (er) throw er;
        });
    });

    before(function(done) {
      this.timeout(0);
      if (!fs.existsSync(dumpDir))
	    fs.mkdirSync(dumpDir);

      var appPath = path.join(global.tests_dir, 'internal');

	  var exec_argv = [appPath];
      app = spawn(process.execPath, exec_argv);
      setTimeout(function() {
        done();
      }, 2000);
    });

    it('should not crash', function() {
      result = fs.readdirSync(dumpDir);	
      assert.equal(result.length, 0);
    });

});
