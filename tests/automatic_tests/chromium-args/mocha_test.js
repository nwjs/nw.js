var assert = require('assert');
var cp = require('child_process');
var fs = require('fs');
var fs_extra = require('fs-extra');
var exec_path = process.execPath;
var app_path = path.join(global.tests_dir, 'chromium-args');
var app_test = require('./nw_test_app');

describe('chromium-args', function() {
    describe('--disable-javascript', function() {
    	before(function(done) {
    	    this.timeout(0);
	    fs_extra.copy(path.join(app_path, 'package1.json'), path.join(app_path, 'package.json'), function (err) {
		if (err) {
		    throw err;
		}
    		cp.exec(exec_path + " " + app_path, function(err, stdout, stderr) {
		})
    	    });
	    setTimeout(done, 2500);
    	});
	
    	it('javascript should not be executed', function() {
    	    var result = fs.existsSync(path.join(app_path, 'hi'));
    	    assert.equal(result, false);
    	});
	
    	after(function() {
    	    fs.unlink(path.join(app_path, 'hi'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(app_path, 'package.json'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	});
    });

    describe('--app=url', function() {
	var result2 = false;
    	before(function(done) {
    	    this.timeout(0);
	    fs_extra.copy(path.join(app_path, 'package2.json'), path.join(app_path, 'package.json'), function (err) {
		if (err) {
		    throw err;
		}
    		var child = app_test.createChildProcess({
		    execPath: exec_path,
		    appPath: app_path,
		    end: function(data, app) {
			result2 = data;
			app.kill();
			done()
		    }
		});
    	    });
    	});
	
    	it('website should be the url', function() {
    	    assert.equal(result2, true);
    	});
    
    	after(function() {
    	    fs.unlink(path.join(app_path, 'package.json'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(app_path, 'hi'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	});
    });

});
