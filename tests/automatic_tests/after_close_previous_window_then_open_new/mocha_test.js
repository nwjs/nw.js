var path = require('path');
var app_test = require('./nw_test_app');
var assert  =require('assert');
var fs = require('fs');
var fs_extra = require('fs-extra');
var spawn = require('child_process').spawn;
var result;

describe('after close previous window then open new', function() {
    after(function () {
    	fs_extra.remove('./automatic_tests/after_close_previous_window_then_open_new/tmp', function (er) {
            if (er) throw er;
        });
    });

    before(function(done) {
        this.timeout(0);
	fs.mkdirSync('./automatic_tests/after_close_previous_window_then_open_new/tmp');

        var appPath = path.join(global.tests_dir, 'after_close_previous_window_then_open_new');

	var exec_argv = [appPath];
	app = spawn(process.execPath, exec_argv);
	setTimeout(function() {
	    done();
	}, 2000);
    });

    it('should not crash', function() {
	result = fs.readdirSync('./automatic_tests/after_close_previous_window_then_open_new/tmp');	
	assert.equal(result.length, 0);
    });

});
