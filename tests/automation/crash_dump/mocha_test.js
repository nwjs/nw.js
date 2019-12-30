var path = require('path');
var assert = require('assert');
var fs = require('fs');
var fs_extra = require('fs-extra');
var curDir = fs.realpathSync('.');
var tmpDir = path.join(curDir, 'internal', 'tmp');

var results;

describe('crash dump', function() {

  var spawnChild = function(action) {
    return spawn(process.execPath, [path.join(curDir, 'internal'), action]);
  };

  var server;
  
  before(function(done) {

    var ready = function() {
    fs.mkdirSync(tmpDir);
    server = createTCPServer(13013);
	done();
    };

    if (fs.existsSync(tmpDir)) {
      fs_extra.remove(tmpDir, function() {
        ready();
      });
    } else {
      ready();
    }

  });
    
  after(function () {
        server.close();
    	fs_extra.remove(tmpDir, function (er) {
            if (er) throw er;
    	});
  });

////////// 1

  describe('crashBrowser()', function() {
	before(function(done) {
	    this.timeout(0);
        var child = spawnChild(0);

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        setTimeout(function() {
          results = fs.readdirSync(tmpDir);
          //child.kill();
          done();
        }, 2000);
      });
    });

	});

    after(function() {
      server.removeAllListeners('connection');
    });

	it('should work fine', function() {
	    assert.equal(results.length, 1);
	    var r = results[0].indexOf("dmp");
	    assert.notEqual(r, -1);
	});
	
    });


///// 2 

    describe('crashRenderer()', function() {
	before(function(done) {
	    this.timeout(0);
        var child = spawnChild(1);

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        setTimeout(function() {
          results = fs.readdirSync(tmpDir);
          //child.kill();
          done();
        }, 2000);
      });
	});
      });

    after(function() {
      server.removeAllListeners('connection');
    });

	it('should work fine', function() {
	    assert.equal(results.length, 2);
	    var r = results[1].indexOf("dmp");
	    assert.notEqual(r, -1);
	});
    });

//// 3,
   describe('App.setCrashDumpDir(dir)', function() {
       	before(function(done) {
	    this.timeout(0);
	    results = fs.readdirSync(tmpDir);
            done();
	});

	it('should work fine', function() {
	    assert.equal(results.length, 2);
	});
    });


});



