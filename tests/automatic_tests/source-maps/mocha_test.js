var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var results;

describe('source maps', function() {
    before(function(done) {
        this.timeout(0);
        var child = app_test.createChildProcess({
            execPath: process.execPath,
            appPath: path.join(global.tests_dir, 'source-maps'),
            end: function(data, app) {
                results = data;
                app.kill();
                done();
            }
        });
    });

    it('should support Closure Compile', function() {
	assert.equal("script.closure.js", results[0]);
	assert.equal("script1.js", results[8]);
    });
    
    it('should support JSMin and Grunt', function() {
	assert.equal("script.jsmin-grunt.js", results[4]);
	assert.equal("script2.js", results[9]);
    });
    
    it('should support Uglifyjs', function() {
	assert.equal('script.uglify.js', results[7]);
	assert.equal('script.js', results[3]);
    });
    
    it('should support CoffeeScript', function() {
	assert.equal('script.coffee.min.js', results[2]);
	assert.equal('script.coffee.coffee', results[1]);
    });
    
    it('should support TypeScript', function() {
	assert.equal('script.typescript.min.js', results[5]);
	assert.equal('script.typescript.ts', results[6]);
    });
});
