var gui = require('nw.gui');
var assert = require('assert');
var app_test = require('../../nw_test_app');
//var local_server = require('../../server/server');
var global = require('../globals');

describe('gui.App', function() {
    before(function(done) {
	this.timeout(0);
    var checkDone = function() {
      if (test_done)
        done();
      else
        setTimeout(function() {checkDone();}, 2000);
    }
    checkDone();

    });

    describe('manifest', function() {
	it('`gui.App.manifest` should equle to value of package.json', function() {
	    assert.equal(gui.App.manifest.name, 'nw-gui.App-test');       
	});

	it('have main', function() {
	    assert.equal(typeof gui.App.manifest.main, 'string');
	    assert.equal(gui.App.manifest.main, 'index.html');
	});

/*
	it('have window', function() {
        console.log('====have window:' + typeof gui.App.manifest.window);
	    assert.equal(typeof gui.App.manifest.window, 'object');       
	});

	it('have dependencies', function() {
	    assert.equal(typeof gui.App.manifest.dependencies, 'object');       
	});
*/
    });
/*    
    describe('clearCache()', function(done) {
	it('should clear the HTTP cache in memory and the one on disk', function() {
	    var res_save = global.local_server.res_save;
	    
	    assert.equal(res_save[1].status, 304);
	    assert.equal(res_save[1].pathname, 'img.jpg');
	    assert.equal(res_save[2].status, 200);
	    assert.equal(res_save[2].pathname, 'img.jpg');
	});
    });
*/
});


