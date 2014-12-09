var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('Plugin', function() {

  describe('flash', function(){

  var server, child, result = false;

  before(function(done) {
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'flash'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = true;
        child.kill();
        done();
      });
    });

  });
    
  after(function () {
        server.close();
  });

    it('should not crash', function() {

      assert.equal(result, true);

    });

  });

});


