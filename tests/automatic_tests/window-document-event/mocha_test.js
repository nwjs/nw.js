var path = require('path');
var spawn = require('child_process').spawn;
var server = global.server;
var server_port = global.port;
var assert = require('assert');

describe('document-start/end',function(){

	var results = undefined;
	var connection_handler_callback = undefined;
	var connection_handler = function(socket){
		socket.on('data',function(data){
			results = JSON.parse(data+"");
			if (typeof connection_handler_callback == 'function'){
				(connection_handler_callback)();
			}
		})
	};
	var child = null;
	before(function(done){
		this.timeout(0);
		server.on('connection',connection_handler);
		var app_path = path.join(global.tests_dir,'window-document-event');
		var exec_argv = [app_path,server_port];
		child = spawn(process.execPath,exec_argv);
		connection_handler_callback = done;
		//we call done when we receive data from child
	});
	after(function(done){
		server.removeListener('connection',connection_handler);
		child.kill();
		done();
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