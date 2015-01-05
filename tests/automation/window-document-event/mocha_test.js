var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('document-start/end',function(){

  var server, child, results;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(''+data);
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
  });


	it('results should not equal undefined',function(done){
		assert.notEqual(results,undefined);
		done();
	})

	it('new window document-start run first',function(done){
		assert.equal(results[0]['flag'],true)
		assert.equal(results[0]['name'],'top-window-document-start')
		done();
	});
	it('new window script run between document-start and document-end',function(done){
		assert.equal(results[1],'new-window-script');
		done();
	});
	it('new window document-end should run before onload event',function(done){
		assert.equal(results[2]['flag'],true)
		assert.equal(results[2]['name'],'top-window-document-end')
		done();
	});
	it('new window onload should run last',function(done){
		assert.equal(results[3],'onload-from-new-window');
		done();
	});

	it('iframe document-start should run first',function(done){
		assert.equal(results[4]['flag'],true)
		assert.equal(results[4]['name'],'iframe-document-start')
		done();
	});
	it('iframe script run between document-start and document-end',function(done){
		assert.equal(results[5],'iframe-script');
		done();
	});
	it('iframe document-end should run later',function(done){
		assert.equal(results[6]['flag'],true)
		assert.equal(results[6]['name'],'iframe-document-end')
		done();
	});
	it('iframe onload should run last',function(done){
		assert.equal(results[7],'onload-from-iframe');
		done();
	});
});


