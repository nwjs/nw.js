var assert = require('assert');
var fs = require('fs');
var app_test = require('./nw_test_app');
var path = require('path');
var results;
describe('user-agent', function() {
    before(function(done) {
	this.timeout(0);
	var child = app_test.createChildProcess({
	    execPath: process.execPath, 
	    appPath: path.join(global.tests_dir, 'user-agent'), 
	    end: function(data, app) {
		results = data;
		app.kill();
		done();
	    }
	});
    });
    
    describe('user-agent with child window opened', function() {
	it('should be same to the one in parent window', function() {
	    assert.equal(results[0], results[1]);
	});
    });
    
    describe('user-agent with reload', function() {
	it('should be same with all reload method', function() {
	    assert.equal(results[0], results[2]);
	    assert.equal(results[0], results[3]);
	    assert.equal(results[0], results[4]);
	    assert.equal(results[0], results[5]);
	});
    });
    
    describe('user-agent with package.json', function() {
	var user_agent = navigator.userAgent.split('/');
	var package_info = JSON.parse(fs.readFileSync('package.json', 'utf8'));
	it('name should be same to the one in package.json',
	   function() {
	       assert.equal(user_agent[0], package_info.name);
	   });
	
	it('version should be same to the one in package.json',
	   function() {
	       assert.equal(user_agent[2], package_info.version);
	   });
    });
})
