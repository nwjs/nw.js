var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var server = global.server;

describe('AppTest', function(){

  describe('reload app', function(){
        
    var exec_argv;  
    
    beforeEach(function(){
      exec_argv = [path.join('app_tests', 'reload_application'), 
                    '--port',
                    global.port];
 
      if (global.auto) exec_argv.push('--auto');

    });

    afterEach(function(){
      server.removeAllListeners('connection');
    });

    it('close window after reload', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(0);
      server.on('connection', function(socket){
        socket.on('end', function(){
          //console.log('client disconnect');
        });

        console.log('client connect');
        socket.setEncoding('utf8');
        socket.on('data', function(data){
        });
      });
       
        var app = spawn(process.execPath, exec_argv);
        app.on('exit', function (code){
          if (code != 0) return done('error');
          //console.log('end');
          done();
        });

        if (global.auto){
        setTimeout(function(){
          
          app.kill();
          done("Timeout, Can not close window.");
        }, 10000);
        }
        
        
      
      
    })
  
    
    it('quit app after reload', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(1);
      server.on('connection', function(socket){
        socket.on('end', function(){
        });

        console.log('client connect');
        socket.setEncoding('utf8');
        socket.on('data', function(data){
        });
      });
    
        var app = spawn(process.execPath, exec_argv);
        
        app.on('exit', function (code){
          if (code != 0) return done('error');
          //console.log('in 2');
          done();
        });

        if (global.auto){
        setTimeout(function(){          
          app.kill();
          done("Timeout, Can not quit App.");
        }, 10000);
        }
  
    })

    it('close window after reload dev', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(2);
      var times = 0;
      server.on('connection', function(socket){
        socket.on('end', function(){
          //console.log('client diss');
        });

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
          done();
        });

      if (global.auto && 1 == 2){
        setTimeout(function(){          
          app.kill();
          done("Timeout, Can not close window.");
        }, 10000);
      }
  
    })

   it('quit app after reload dev', function(done){
      this.timeout(0);
      exec_argv.push('--type');
      exec_argv.push(3);
      var times = 0;
      server.on('connection', function(socket){
        socket.on('end', function(){
          //console.log('client diss');
        });

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
          done();
        });

      if (global.auto){
        setTimeout(function(){          
          app.kill();
          done("Timeout, Can not quit App.");
        }, 10000);
      }
  
    }) 


  })
})

