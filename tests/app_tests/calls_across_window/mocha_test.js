var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var server = global.server

describe('AppTest', function(){
  describe('call across window', function(){
  
    var app;
    var exec_argv;
    var socket;
    before(function(done){
      //this.timeout(0);
      exec_argv = [path.join('app_tests', 'calls_across_window'), 
                    '--port',
                    global.port];
 
      if (global.auto) exec_argv.push('--auto');

      server.on('connection', function(s){
        socket = s;
        s.setEncoding('utf8');
        s.on('end', function(){
          console.log('client dissconnect');
        });
        done();
      });
      app = spawn(process.execPath, exec_argv);
      
    })

    after(function(done){
      this.timeout(0);
      /*
      server.removeAllListeners('connection');
      app.on('exit', function(c){
        done();
      });
      */
      app.kill();
      done();
    })

    afterEach(function(){
      //console.log('before Each');
      socket.removeAllListeners('data');
    })

    it ('nw window function call', function(done) {
      
      socket.on('data', function(data) {
        if (data == 'ok') {
          done();
        } else {
          done(data);
        }
      });
      socket.write('newWindow');

    })

    it ('brower window function call', function(done) {
      //this.timeout(0);
      socket.on('data', function(data) {
        if (data == 'ok') {
          done();
        } else {
          done(data);
        }
      });
      socket.write('newBrowserWindow');

    })

  })
})
