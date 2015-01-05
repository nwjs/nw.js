var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('node-main', function() {

  var server;

  before(function(done) {
    server = createTCPServer(13013);
    done();
  });
    
  after(function () {
        server.close();
  });

  describe('create http server in node-main', function() {
    it('nw should not close by itself after show devtool',
	  function(done) {
        this.timeout(0);
        var result = false;

        var child = spawnChildProcess(path.join(curDir, '..', 'show_devtool_after_http_server_created_in_node_main'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data);
        child.kill();
        if (result.success) done();
        else done('error');
        result = true;
        server.removeAllListeners('connection');
      });
    });


        setTimeout(function(){
          if (!result) {
            done('nw close by itself')
          }
        }, 3000);
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});

    });
  });



  describe('call require() in app', function() {
    it('nw should can require modules', function(done){
      this.timeout(0);
      var result = false;
        var child = spawnChildProcess(path.join(curDir, '..', 'call_require_with_node-main_set'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data);
        child.kill();
        if (result.ok) done();
        else done('error:'+result.error);
        result = true;
        server.removeAllListeners('connection');
      });
    });

    });
  });

  describe('reference node-main module',function(){
    it('nw should be able to reference node-main module',function(done){
      this.timeout(0);

      var result = false;
        var child = spawnChildProcess(path.join(curDir, '..', 'reference-node-main'));

    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data);
        child.kill();
        assert.equal(result,true);
        done();
        server.removeAllListeners('connection');
      });
    });

    });
  });



});


