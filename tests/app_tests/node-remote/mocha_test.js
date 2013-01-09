var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var server = global.server;

describe('node-remote', function() {
  describe('enable remote site http://127.0.0.1:80/', function() {
    var app;
    var exec_argv;
    var socket;
    var msg = ['',''];
    before(function(done) {
      this.timeout(0);
      exec_argv = [path.join('app_tests', 'node-remote'), 
                    '--port',
                    global.port];
 
      if (global.auto) exec_argv.push('--auto');

      server.on('connection', function(s){
        socket = s;
        s.setEncoding('utf8');

        socket.on('data', function(data) {
          var data_list = data.split('/');
          if (data_list[0] == '80'){
            if (data_list[1] == 'done'){        
              socket.write('80');
            } else if (data_list[1] == 'ok') {
              msg[0] = 'ok';
            } else {
              msg[0] = data_list[1];
            }
            
          }

          if (data_list[0] == '8080'){
            if (data_list[1] == 'done'){        
              socket.write('8080');
            } else {
              if (data_list[1] == 'ok') {
                msg[1] = 'ok';
              } else {
               msg[1] = data_list[1];
              }
            done();
            }
            
          }
          

        });
      });
      app = spawn(process.execPath, exec_argv);
      
      
    })

    
    after(function(done) {
      this.timeout(0);
      app.kill();
      done();      
    })
    

    afterEach(function(){
      //console.log('before Each');
      socket.removeAllListeners('data');
    })

     

    it('http://127.0.0.1/test.html should be able call Node',
      function(done) {
        if (msg[0] == 'ok'){
          done();
        } else {
          done(msg[0]);
        }
        
    })

    it('http://127.0.0.1:8080/test.html should be able call Node',
      function(done) {
        if (msg[0] == 'ok'){
          done();
        } else {
          done(msg[0]);
        }
        
    })

  })

})
