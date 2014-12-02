var path = require('path');
var assert = require('assert');
var spawn = require('child_process').spawn;
var fs = require('fs');
var curDir = fs.realpathSync('.');

var changed;

describe('Window.cookies', function() {

  var server;

  var spawnChild = function(action) {
    return spawn(process.execPath, [path.join(curDir, 'internal'), action]);
  };

  before(function(done){
    server = createTCPServer(13013);
    done();
  });

  after(function(done) {
    this.timeout(0);
    server.close();
    done();
  });

//// 1 
    describe("get, getAll", function() {

    var child;
    var results;

	before(function(done) {
	    this.timeout(0);
    child = spawnChild(1);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        child.kill();
        done();
      });
    });
	});

    after(function() {
      server.removeAllListeners('connection');
    });

	it('should get cookies', function() {
	    assert.equal(results[0], true);
	    assert.equal(results[1], true);
	});
    });


/////////// 2
    describe("set", function() {
    	describe("set lang en", function() {
    	    before(function(done) {
    		this.timeout(0);
            child = spawnChild(2);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        changed = results[1];
        child.kill();
        done();
      });
    });
    	    });

    after(function() {
      server.removeAllListeners('connection');
    });

    	    it('should get cookies', function() {
    		assert.equal(results[0], true);
    	    })
    	});

	describe("set lang zh", function() {
	    before(function(done) {
		this.timeout(0);
            child = spawnChild(3);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        changed = results[1];
        child.kill();
        done();
      });
    });
	    });

    after(function() {
      server.removeAllListeners('connection');
    });

	    it('should get cookies', function() {
		assert.equal(results[0], true);
	    })
	});

    });
    
//// 3, 
    describe('remove', function() {
    	before(function(done) {
    	    this.timeout(0);
            child = spawnChild(4);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        //changed = results[1];
        child.kill();
        done();
      });
    });
    	});

    after(function() {
      server.removeAllListeners('connection');
    });

    	it('should remove lang', function() {
    	    assert.equal(results[1], false);
    	});
    });
    
///////// 4, 
    describe('onChanged', function() {
    	it('should be called when changed', function() {
    	    assert.equal(changed, true);
    	})
    });


});
	 
