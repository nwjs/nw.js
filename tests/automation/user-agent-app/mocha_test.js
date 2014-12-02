var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('user-agent-app', function() {

  var result = false, package_name, package_info;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = true;
        package_name = data;
        package_info = JSON.parse(fs.readFileSync(path.join(curDir, 'internal', 'package.json'), 'utf8'));
        child.kill();
        done();
      });
    });

      setTimeout(function(){
          if (!result) {
            child.kill();
            done('loaded event does not been fired');
          }
        }, 7000);
    
  });
    
  after(function () {
        server.close();
  });

  it('user agent shoud be the same when open a new window',
    function() {
      assert.equal(result, true);
      assert.equal(package_name, package_info.name);
  });


});

