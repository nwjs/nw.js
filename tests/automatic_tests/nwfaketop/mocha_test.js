var path = require('path');
var assert = require('assert');
var spawn = require('child_process').spawn;
var server = global.server;
var server_port = global.port;


describe('nwfaketop',function(){

	var results = [];
	var connection_handler_callback = undefined;
	var connection_handler = function(socket){
		socket.on('data',function(data){
			results = JSON.parse(data+"");
			connection_handler_callback();
		});
	};
	var child = undefined;

	before(function(done){
		this.timeout(0);
		server.on('connection',connection_handler);
		var app_path = path.join(global.tests_dir,'nwfaketop');
		var exec_argv = [app_path,server_port];
		child = spawn(process.execPath,exec_argv);
		connection_handler_callback = done;
	});

	after(function(done){
		server.removeListener('connection',connection_handler);
		child.kill();
		done();
	});

	it("nwfaketop attribute set",function(done){
		assert(results[0],true);
		done();
	});
	it("nwfaketop attribute default",function(done){
		assert(results[1],true);
		done();
	});


});