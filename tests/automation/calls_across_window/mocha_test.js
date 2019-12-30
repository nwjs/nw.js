var fs = require('fs');
var path = require('path');
var global = {tests_dir: fs.realpathSync('.')};

describe('AppTest', function(){
  describe('call across window', function(){
    var child;
    var server;
    var exec_argv;
    var socket;

    before(function(done){
      this.timeout(0);
      server = createTCPServer(13013);
      server.on('connection', function(theSocket) {
        socket = theSocket;
        socket.setEncoding('utf8');
        done();
      });
      exec_argv = path.join(global.tests_dir, 'internal');
      child = spawnChildProcess(exec_argv);
    });

    after(function(done){
      this.timeout(0);
      child.kill();
      server.close();
      done();
    });

    afterEach(function(){
      socket.removeAllListeners('data');
    });

    it ('nw window function call', function(done) {
      socket.on('data', function(data) {
        if (data == 'ok') {
          done();
        } else {
          done(data);
        }
      });
      socket.write('newWindow');
    });

    it ('brower window function call', function(done) {
      socket.on('data', function(data) {
        if (data == 'ok') {
          done();
        } else {
          done(data);
        }
      });
      socket.write('newBrowserWindow');
    });
  });
});


