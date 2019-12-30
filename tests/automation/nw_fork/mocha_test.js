var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

var result = false;
describe('fork()',function(){

	var child, server, result;

	before(function(done){
		this.timeout(0);
        server = createTCPServer(13013);
        child = spawnChildProcess(path.join(curDir, 'internal'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data+'');
      });
    });

		child.on('exit',function(){
			done();
		});


	});

	after(function(done){
		server.close();
		child.kill();
		done();
	});



	it("child_process.fork() should have a workaround solution",function(done){
		assert.equal(result,true);
		done();
	});
	
});

