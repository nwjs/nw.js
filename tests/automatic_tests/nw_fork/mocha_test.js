var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');
var result = false;
describe('fork()',function(){
	var server = global.server;
	var get_result = function(socket){
		socket.on('data',function(data){
			if (data.toString() == "true"){
				result = true;
			}
		});
	};

	before(function(done){
		this.timeout(0);
		server.on('connection',get_result);
		var app_path = path.join('automatic_tests','nw_fork');
		var port = global.port || 13013;
		var child = spawn(process.execPath,[app_path,port]);
		child.on('exit',function(){
			done();
		});
	});

	it("child_process.fork() should have a workaround solution",function(done){
		assert.equal(result,true);
		done();
	});
	
	after(function(done){
		server.removeListener('connection',get_result);
		done();
	});
});