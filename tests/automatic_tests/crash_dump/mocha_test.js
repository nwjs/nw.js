var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var fs = require('fs');
var fs_extra = require('fs-extra');
var results;
describe('crash dump', function() {
    before(function(done) {
	fs.mkdirSync('./automatic_tests/crash_dump/tmp');
	done();
    });
    
    after(function () {
    	fs_extra.remove('./automatic_tests/crash_dump/tmp', function (er) {
            if (er) throw er;
    	});
    });

    describe('crashBrowser()', function() {
	before(function(done) {
	    this.timeout(0);
	    var child = app_test.createChildProcess({
		execPath: process.execPath,
		appPath: path.join(global.tests_dir, 'crash_dump'),
		args:[0],
		end: function(data, app) {
		    setTimeout(function() {
			result = fs.readdirSync('./automatic_tests/crash_dump/tmp');
			done();
		    }, 2000);
		}
	    });
	});

	it('should work fine', function() {
	    assert.equal(result.length, 1);
	    var r = result[0].indexOf("dmp");
	    assert.notEqual(r, -1);
	});
	
    })

    describe('crashRenderer()', function() {
	before(function(done) {
	    this.timeout(0);
	    var child = app_test.createChildProcess({
		execPath: process.execPath,
		appPath: path.join(global.tests_dir, 'crash_dump'),
		args:[1],
		end: function(data, app) {
		    setTimeout(function() {
			result = fs.readdirSync('./automatic_tests/crash_dump/tmp');
			done();
		    }, 2000);
		}
	    });
	});

	it('should work fine', function() {
	    assert.equal(result.length, 2);
	    var r = result[1].indexOf("dmp");
	    assert.notEqual(r, -1);
	});
    })

   describe('App.setCrashDumpDir(dir)', function() {
       	before(function(done) {
	    this.timeout(0);
	    result = fs.readdirSync('./automatic_tests/crash_dump/tmp');
            done();
	});

	it('should work fine', function() {
	    assert.equal(result.length, 2);
	});
    })
	

});
