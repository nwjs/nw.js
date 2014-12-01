var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('inject-js',function(){

  var server, child, result;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data);
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
  });
	

	it('result.length should equal 4',function(done){
		assert.notEqual(result,null);
		assert.equal(result.length,4);
		done();
	});
	it('inject-js-start should run first',function(done){
		assert.equal(result[0],'inject-js-start');
		done();
	});
	it('document script should run after inject-js-start',function(done){
		assert.equal(result[1],'script-start');
		done();
	});
	it('inject-js-end should before window.onload',function(done){
		assert.equal(result[2],'inject-js-end');
		done();
	});
	it('inject-js-start should run last',function(done){
		assert.equal(result[3],'onload');
		done();
	});
});


