var path =  require('path');
var exec = require('child_process').exec;
var assert = require('assert');
var func = require('./' + global.tests_dir +'/start_app/script.js');
var fs = require('fs-extra');
var result = new Array();
var child;
var isFileExist = true;
describe('chromedriver2_server', function() {
    this.timeout(0);
    before(function(done) {
	func.copyExecFiles(function () {
	    //chromedriver2_server needs to be put under folder 'nw/tests/automatic_tests/chromedriver2_server'
	    var src_file;
            if (process.platform == 'win32')
		src_file = path.join(global.tests_dir, 'chromedriver2_server', 'chromedriver2_server.exe');
	    else
		src_file = path.join(global.tests_dir, 'chromedriver2_server', 'chromedriver2_server');
	    if (!fs.existsSync(src_file)) {
		isFileExist = false;
		done();
	    }
	    var dst_file = path.join('tmp-nw', 'chromedriver2_server');
	    fs.copy(src_file, dst_file, function(err) {
		if(err) throw err;
		setTimeout(function () {
		exec('python ' + path.join(global.tests_dir, 'chromedriver2_server','chromedriver2_server_test.py'),
		     function (error, stdout, stderr) {
			 result.push(error);
			 result.push(stdout);
			 result.push(stderr);
			 done();
		     });
		}, 100);
	    });
	});
    });
    
    after(function () {
	fs.remove('tmp-nw', function (er) {
            if (er) throw er;
	});
    });

    it('should work' ,function(done) {
	if (!isFileExist)
	    done("chromedrier2_server does not exist");
	else {
	    assert.equal(result[0], null);
	    assert.equal(result[1], 'pass\n');
	    assert.equal(result[2], '');
	    done();
	}
    });
});
