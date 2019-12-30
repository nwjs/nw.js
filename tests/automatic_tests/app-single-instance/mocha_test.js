var child_process = require('child_process')
var server = global.server;
var server_port = global.port;
var assert = require('assert');
var path = require('path');

describe('Single Instance APP', function() {
	var counter = 0;
    var pushData = function(socket) {
        socket.on('data', function(data) {
        	counter += 1;
        })
    };
    var runApp = function(path){
    	var spawn = child_process.spawn;
    	var child = spawn(process.execPath,[path,server_port]);
    	return child;
    };

    before(function(done) {
        server.on('connection',pushData);
        var app_path = path.join(global.tests_dir,'app-single-instance/app');
        console.log(app_path);
        console.log(process.cwd());
        var child1 = runApp(app_path);
        var child2 = runApp(app_path);
        var child3 = runApp(app_path);
        setTimeout(function(){
        	child1.kill();
        	child2.kill();
        	child3.kill();
        	done();
        },1000);
    });

    it('app should be single instance', function(done) {
        assert.equal(counter,1);
        done();
    });

    after(function(done) {
        server.removeListener('connection',pushData);
        done();
    });
});