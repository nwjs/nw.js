var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var server = global.server;
var cb;

describe('AppTest', function(){

  describe('reload app (long-to-run)', function(){
        

    var exec_argv = [path.join('app_tests', 'reload_application'), 
                    '--port',
                    global.port,
                    '--auto'];
    
    
    afterEach(function(){
      try {
        server.removeListener('connection', cb);
      } catch (e) {
      }
      
    });
    

    it('close window after reload', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(0);      
      var result = false;
      
      var app = spawn(process.execPath, exec_argv);
      app.on('exit', function (code){
      if (code != 0) return done('error');
        result = true;
        done();
      });

      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not close window.");
        }
      }, 10000);
      
    })
  
    
    it('quit app after reload', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(1);
      var result = false;
      
      var app = spawn(process.execPath, exec_argv);
      app.on('exit', function (code){
      if (code != 0) return done('error');
        result = true;
        done();
      });

      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not quit App.");
        }
      }, 10000);

  
    })

    it('close window after reload dev', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(2);
      var times = 0;
      var result = false;
      
      server.on('connection', cb = function(socket){
        
        //console.log('client connect in 1');
        socket.setEncoding('utf8');
        socket.on('data', function(data){
          if (data == 'open'){
            
            if (times == 0){
              times += 1;
              socket.write('reload');
            } else if (times == 1){
              socket.write('quit');
            }            
          
          }// if(data == 'open')

        });
      });
    
        var app = spawn(process.execPath, exec_argv);
        
        app.on('exit', function (code){
          if (code != 0) return done('error');
          result = true;
          done();
        });

      
      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not close window.");
        }
      }, 10000);
      
  
    })

   it('quit app after reload dev', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(3);
      var times = 0;
      var result = false;
      
      server.on('connection', cb = function(socket){

        //console.log('client connect');
        socket.setEncoding('utf8');
        socket.on('data', function(data){
          if (data == 'open'){
            
            if (times == 0){
              times += 1;
              socket.write('reload');
            } else if (times == 1){
              socket.write('quit');
            }            
          
          }// if(data == 'open')

        });
      });
    
        var app = spawn(process.execPath, exec_argv);
        
        app.on('exit', function (code){
          if (code != 0) return done('error');
          result = true;
          done();
        });

      
      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not quit App.");
        }
      }, 10000);
      
  
    }) 


  })
})

