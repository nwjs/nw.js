var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var results;
var changed;
describe('Window.cookies', function() {

    describe("get, getAll", function() {
	before(function(done) {
	    this.timeout(0);
	    var child = app_test.createChildProcess({
		execPath: process.execPath,
		appPath: path.join(global.tests_dir, 'cookies_api'),
		args: [1],
		end: function(data, app) {
		    results = data;
		    app.kill();
		    console.log("1");
		    done();
		}
	    });
	});
	it('should get cookies', function() {
	    assert.equal(results[0], true);
	    assert.equal(results[1], true);
	})
    })

    describe("set", function() {
    	describe("set lang en", function() {
    	    before(function(done) {
    		this.timeout(0);
    		var first = false;
    		var child = app_test.createChildProcess({
    		    execPath: process.execPath,
    		    appPath: path.join(global.tests_dir, 'cookies_api'),
    		    args: [2],
    		    end: function(data, app) {
    			results = data;
			changed = results[1];
    			app.kill();
    			console.log("2");
    			done();
    		    }
    		});
    	    });
    	    it('should get cookies', function() {
    		assert.equal(results[0], true);
    	    })
    	});

	describe("set lang zh", function() {
	    before(function(done) {
		this.timeout(0);
		var child = app_test.createChildProcess({
		    execPath: process.execPath,
		    appPath: path.join(global.tests_dir, 'cookies_api'),
		    args: [3],
		    end: function(data, app) {
			console.log("3");
			results = data;
			done();
			app.kill();
		    }
		});
	    });

	    it('should get cookies', function() {
		assert.equal(results[0], true);
	    })
	});

    });
    
    describe('remove', function() {
    	before(function(done) {
    	    this.timeout(0);
    	    var child = app_test.createChildProcess({
    		execPath: process.execPath,
    		appPath: path.join(global.tests_dir, 'cookies_api'),
    		args: [4],
    		end: function(data, app) {
    		    console.log("4");
    		    results = data;
    		    app.kill();
    		    done();
    		}
    	    });
    	});

    	it('should remove lang', function() {
    	    assert.equal(results[1], false);
    	});
    });
    
    describe('onChanged', function() {
    	it('should be called when changed', function() {
    	    assert.equal(changed, true);
    	})
    });
});
	 
