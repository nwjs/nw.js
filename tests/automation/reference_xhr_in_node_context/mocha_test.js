var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('reference xhr from node context',function(){

  var server, child, crash = true;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        crash = false;
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
  });

	it("reference xhr from node context should not crash",function(done){
		assert.equal(crash,false);
		done();
	});
});

