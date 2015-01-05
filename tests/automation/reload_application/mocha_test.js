var assert = require('assert');
var spawn = require('child_process').spawn;
var path = require('path');
var net = require('net');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

var cb;

describe('AppTest', function(){

  describe('reload app (long-to-run)', function(){


  var server;

  before(function(done) {
    server = createTCPServer(13013);
	done();
  });
    
  after(function () {
        server.close();
  });

    afterEach(function(){
      try {
        server.removeAllListeners('connection');
      } catch (e) {
      }

    });



    var spawnChild = function(type) {
      return spawn(process.execPath, [path.join(curDir, 'internal'), type]);
    };


    it('close window after reload', function(done){
      this.timeout(0);
      var result = false;

      var app = spawnChild(0);
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
      }, 30000);

    });



    it('quit app after reload', function(done){
      this.timeout(0);
      var result = false;
      var app = spawnChild(1);
      app.on('exit', function (code){
        if (code) {
          done('error: the error code is: ' + code);
        } else {
          result = true;
          done();
        }
      });

      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not quit App.");
        }
      }, 30000);


    });


    it('close window after reload dev', function(done){
      this.timeout(0);
      var times = 0;
      var result = false;

      server.on('connection', cb = function(socket){

        //console.log('client connect in 1');
        socket.setEncoding('utf8');
        socket.on('error', function(er) {
          //console.log(er);
        });
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

        var app = spawnChild(2);

        app.on('exit', function (code){
        if (code) {
          done('error: the error code is: ' + code);
        } else {
          result = true;
          done();
        }
        });


      setTimeout(function(){
        if (!result) {
          app.kill();
          done("Timeout, Can not close window.");
        }
      }, 30000);


    });


   it('quit app after reload dev', function(done){
      this.timeout(0);
      var times = 0;
      var result = false;

      server.on('connection', cb = function(socket){
        socket.on('error', function(er) {
          //console.log(er);
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

        var app = spawnChild(3);

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
      }, 30000);


    });




  });

});



