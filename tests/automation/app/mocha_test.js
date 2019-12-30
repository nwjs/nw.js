var gui = require('nw.gui');
var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('gui.App', function() {


  var server, child, result = false;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = true;
        child.kill();
        done();
      });
    });

    setTimeout(function() {
      if (!result) {
        child.kill();
        done('timeout');
      }
    }, 4500);

  });
    
  after(function () {
        server.close();
  });


    describe('manifest', function() {
	it('`gui.App.manifest` should equle to value of package.json', function() {
	    assert.equal(gui.App.manifest.name, 'nw-gui.App-test.wrapper');       
	});

	it('have main', function() {
	    assert.equal(typeof gui.App.manifest.main, 'string');
	    assert.equal(gui.App.manifest.main, 'index.html');
	});


	it('have window', function() {
        console.log('====have window:' + typeof gui.App.manifest.window);
	    assert.equal(typeof gui.App.manifest.window, 'object');       
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

});


