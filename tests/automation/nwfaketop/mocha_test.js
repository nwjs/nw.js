var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('nwfaketop',function(){

	var child, server, result;

	before(function(done){
		this.timeout(0);
        server = createTCPServer(13013);
        child = spawnChildProcess(path.join(curDir, 'internal'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data+'');
        done();
      });
    });

	});

	after(function(done){
		server.close();
		child.kill();
		done();
	});

	it("nwfaketop attribute set",function(done){
		assert(result[0],true);
		done();
	});
	it("nwfaketop attribute default",function(done){
		assert(result[1],true);
		done();
	});


});


