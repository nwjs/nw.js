
var path = require('path');
var app_test = require('./nw_test_app');
var server = global.server;
var cb;

describe('new-instance', function() {

  after(function() {
     server.removeListener('connection', cb);
  });

	it('new window has different pid', function(done) {
	    this.timeout(0);
	    var result = false;
	    var times = 0;
	    var pid1, pid2;

	    var child = app_test.createChildProcess({
	      execPath: process.execPath,
	      appPath: path.join(global.tests_dir, 'new-instance'),
	      no_connect: true,

	    });


	    server.on('connection', cb = function(socket){
	      socket.setEncoding('utf8');
          socket.on('error', function(er) {
            console.log(er);
          })
	      socket.on('data', function(data) {

	        if (times == 0) {
	          pid1 = data;
	          times += 1;
	        } else {
	          pid2 = data;

	          if (pid1 != pid2) {
	            done();
	          } else {
	            done('they are in the same process');
	          }

	          child.app.kill();
	        } // if (times == 0)
	      });
	    }); // server.on('connection', cb = function(socket)


	})


})

