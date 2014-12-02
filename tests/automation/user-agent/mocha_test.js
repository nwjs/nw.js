var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('user-agent', function() {

  var server, child, results;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        for (var i = 0, len = results.length; i < len; i++) {
            console.log('User agent['+i+']:' + results[i]);
        }
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
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
	var package_info = JSON.parse(fs.readFileSync(path.join(curDir, 'internal', 'package.json'), 'utf8'));
	it('name should be same to the one in package.json',
	   function() {
	       assert.equal(user_agent[0], package_info.name);
	   });
	
	it('version should be same to the one in package.json',
	   function() {
	       assert.equal(user_agent[2], package_info.version);
	   });
    });

});


