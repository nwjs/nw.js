var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('Window.eval',function(){
  var server, child, result;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = (data.toString() == "success");
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
  });

	it("Window.eval should works",function(done){
		assert.equal(result,true);
		done();
	});
});


