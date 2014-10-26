var gui = require('nw.gui');
var assert = require('assert');
var app_test = require('./nw_test_app');

describe('gui.App', function() {
    before(function(done) {
	this.timeout(0);
	var child = app_test.createChildProcess({
	    execPath: process.execPath,
	    appPath: path.join(global.tests_dir, 'app'),
	    end: function(data, app) {
		app.kill();
		done();
	    }
	});
    });

    describe('manifest', function() {
	it('`gui.App.manifest` should equle to value of package.json', function() {
	    assert.equal(gui.App.manifest.name, 'nw-tests');       
	});
	
	it('have window', function() {
	    assert.equal(typeof gui.App.manifest.window, 'object');       
	});
	
	it('have main', function() {
	    assert.equal(typeof gui.App.manifest.main, 'string');
	    assert.equal(gui.App.manifest.main, 'index.html');
	});
	
	it('have dependencies', function() {
	    assert.equal(typeof gui.App.manifest.dependencies, 'object');       
	});
    });
    
    describe('clearCache()', function(done) {
	it('should clear the HTTP cache in memory and the one on disk', function() {
	    var res_save = global.local_server.res_save;
	    
	    assert.equal(res_save[1].status, 304);
	    assert.equal(res_save[1].pathname, 'img.jpg');
	    assert.equal(res_save[2].status, 200);
	    assert.equal(res_save[2].pathname, 'img.jpg');
	});
    });
})
