
var path = require('path');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('file input',function(){
  describe('set value',function(){

  var server;

  before(function(done) {
    server = createTCPServer(13013);
	done();
  });
    
  after(function () {
        server.close();
  });

    it('the value should be set without exception',function(done){
      this.timeout(0);
      var child = spawnChildProcess(path.join(curDir, 'internal'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        setTimeout(function() {
          child.kill();
          if (data == 'mytestfile')
            done();
          else 
            done('the value of the file input is not set correctly');
        }, 2000);
      });
    });

    });

  });

});


