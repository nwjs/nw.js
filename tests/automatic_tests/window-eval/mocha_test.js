var server = global.server;
var server_port = global.port;
var result = false;
var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');

var connection_handler_callback = undefined;
var connection_handler = function(socket){
	socket.on('data',function(data){
		if (data.toString() == "success"){
			result = true;
		}
		if (typeof connection_handler_callback === 'function'){
			(connection_handler_callback)();
		}
	})
};

describe('Window.eval',function(){
	var child = null;
	
	before(function(done){
		this.timeout(0);
		server.on('connection',connection_handler);
		var app_path = path.join(global.tests_dir,'window-eval');
		var exec_args = [app_path,server_port];
		child = spawn(process.execPath,exec_args);
		connection_handler_callback = done;
	});

	after(function(done){
		server.removeListener('connection',connection_handler);
		child.kill();
		done();
	});

	it("Window.eval should works",function(done){
		assert.equal(result,true);
		done();
	});
})