var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var server = global.server;
var cb;

describe('node-remote', function() {
  describe('enable remote site http://127.0.0.1:8123/', function() {
    var app;
    var exec_argv;
    var socket;
    before(function(done) {
      //this.timeout(0);
      exec_argv = [path.join(global.tests_dir, 'node-remote'),
                    '--port',
                    global.port];

      if (global.auto) exec_argv.push('--auto');

      server.on('connection', cb = function(s){
        socket = s;
        s.setEncoding('utf8');
        done();
        socket.on('error', function(e) {
          console.log(e);
        });
      });
      app = spawn(process.execPath, exec_argv);
    })

    after(function(done) {
      this.timeout(0);
      app.kill();   
      server.removeListener('connection', cb);
      done();
    })

    afterEach(function(){
      socket.removeAllListeners('data');
    })

    it('http://127.0.0.1:8123/node_remote_test.html should be able call Node',
	  function(done) {
        this.timeout(0);
        socket.on('data', function(data) {
          if (data == 'ok'){
            done();
          } else {
            done(data);
          }

        });
        socket.write('8123');
    })

    it('http://127.0.0.1:8124/node_remote_test.html should not be able call Node',
      function(done) {
        this.timeout(0);
        socket.on('data', function(data) {
         if (data == 'ok'){
            done('should not call node');
          } else {
            done();
          }

        });
        socket.write('8124');
    })
  })
})
