var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var window_counts = 3;
var results;

describe('window', function() {
    this.timeout(0);
    before(function(done) {
	this.timeout(0);
	var child = app_test.createChildProcess({
	    execPath: process.execPath,
	    appPath: path.join(global.tests_dir, 'window'),
	    end: function(data, app) {
		results = data;
                app.kill();
                done();
	    }
	});

    });
    describe('focus()', function() {
	it('should focus on the window', function() {
	    for (var i = 0; i < window_counts; i++)
		assert.notEqual(results[i], 0)
	});
    })
})
	 
