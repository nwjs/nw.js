var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('loaded event', function() {

  var server, child, result = false;

  before(function(done) {
    server = createTCPServer(13013);
    done();
  });
    
  after(function () {
        server.close();
  });

    it('loaded event can been fired',
	  function(done) {
        this.timeout(0);

    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = true;
        child.kill();
        done();
      });
    });

        setTimeout(function(){
          if (!result) {
            child.close();
            //child.app.kill();
            //child.removeConnection();
            done('loaded evenet does not been fired');
          }
        }, 3000);
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});

    })


})

