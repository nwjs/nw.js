var path = require('path');
var spawn = require('child_process').spawn;
var server = global.server;
var server_port = global.port;
var assert = require('assert');



describe('inject-js',function(){
	var result = null;
	var child = null;

	var connection_handler_callback = undefined;
	var connection_handler = function(socket){
		socket.on('data',function(data){
			result = JSON.parse(data);
			if (typeof connection_handler_callback === 'function'){
				(connection_handler_callback)();
			}
		});
	};
	
	before(function(done){
		this.timeout(0);
		var app_path = path.join(global.tests_dir,'inject-js');
		var exec_argv = [app_path,server_port];
		child = spawn(process.execPath,exec_argv);
		server.on('connection',connection_handler);
		connection_handler_callback = done;
	});
	
	after(function(done){
		server.removeListener('connection',connection_handler);
		child.kill();
		done();
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