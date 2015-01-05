var path = require('path');
var assert = require('assert');
var gui = require('nw.gui');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');
var results = new Array();
describe('document.cookies', function() {

var server;

  before(function(done) {
    server = createTCPServer(13013);
    results.push('dump');  // should be remove when the first test case is ready
	done();
  });
    
  after(function () {
        server.close();
  });

/*
    describe('http', function() {
	before(function(done) {
	    this.timeout(0);
	    var url = "http://127.0.0.1:8123/document_cookies.html";
	    var win = gui.Window.open(url);

	    setTimeout(function() {
		results.push(win.window.msg.textContent);
		done();
		win.window.close();
	    }, 1000);
	});
	it('should be set', function() {
	    assert.equal(results[0], '123');
	});
    });
*/

    describe('file', function() {
	before(function(done) {
	    this.timeout(0);
        var child = spawnChildProcess(path.join(curDir, 'file'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        setTimeout(function() {
          results.push(data);
          child.kill();
          done();
        }, 1000);
      });
    });
	    setTimeout(done, 10000);
	});

    after(function() {
      server.removeAllListeners('connection');
    });

	it ('should be set', function() {
	    assert.equal(results[1], '123');
	});
    });

    describe('app', function() {
	before(function(done) {
	    this.timeout(0);
        var child = spawnChildProcess(path.join(curDir, 'app'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        setTimeout(function() {
          results.push(data);
          child.kill();
          done();
        }, 1000);
      });
    });

	    setTimeout(done, 10000);
	});

    after(function() {
      server.removeAllListeners('connection');
    });

	it ('should be set', function() {
	    assert.equal(results[2], '123');
	});
    });

});
