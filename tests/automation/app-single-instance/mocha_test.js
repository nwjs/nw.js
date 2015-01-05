var assert = require('assert');
var path = require('path');
var global = require('../globals');

describe('Single Instance APP', function() {

  var count = 0;
  var onConnection = function(socket) {
    socket.on('data', function(data) {
      count += 1;
    });
  };

  var server = undefined;

  before(function(done) {
    server = createTCPServer(13013);
    server.on('connection', onConnection);

    var appPath = path.join(global.tests_dir, 'app');
    var child_1 = spawnChildProcess(appPath);
    var child_2 = spawnChildProcess(appPath);
    var child_3 = spawnChildProcess(appPath);
    setTimeout(function() {
      child_1.kill();
      child_2.kill();
      child_3.kill();
      done();
    }, 1000);
  });

  after(function() {
    if (server) {
      server.removeListener('connection',onConnection);
      server.close();
    }
  });

  it('app should be single instance', function() {
    assert.equal(count,1);
  });
});
