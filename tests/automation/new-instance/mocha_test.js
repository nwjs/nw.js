var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('new-instance', function() {
  var server, child, result = false;

  before(function(done) {
    server = createTCPServer(13013);
    done();
  });
    
  after(function () {
        server.close();
  });

	it('new window has different pid', function(done) {
	    this.timeout(0);
	    var result = false;
	    var times = 0;
	    var pid1, pid2;

    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
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
              child.kill();
            }

      });
    });


	});


});


