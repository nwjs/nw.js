var path = require('path');
var assert = require('assert');
var spawn = require('child_process').spawn;
var server = global.server;
var port = global.port||13013;

describe('reference xhr from node context',function(){
	
	var crash = true;
	var child = undefined;
	var connection_handler_callback = undefined;
	var connection_handler = function(socket){
		socket.on('data',function(data){
			crash = false;
			connection_handler_callback();
		});
	};


	before(function(done){
		this.timeout(0);
		server.on('connection',connection_handler);
		var exec_path = path.join(global.tests_dir,'reference_xhr_in_node_context');
		var exec_argv = [exec_path,port]
		child = spawn(process.execPath,exec_argv);
		connection_handler_callback = done;
		child.on('exit',function(){
			if (crash){
				done();
			}
		});
	});

	after(function(done){
		server.removeListener('connection',connection_handler);
		child.kill();
		done();
	});

	it("reference xhr from node context should not crash",function(done){
		assert.equal(crash,false);
		done();
	});
});