var assert = require('assert');
var spawn = require('child_process').spawn;
var fs = require('fs-extra');
var path = require('path');
var curDir = fs.realpathSync('.');
var func = require(path.join(curDir, '..', 'start_app', 'script.js'));

var is_chromedriver_exists = false;

describe("buff_from_string", function() {
  var results = [];
  var onConnection = function(socket) {
    socket.on('data', function(data) {
      results = JSON.parse(data.toString());
    });
  };

  var server = undefined;
  var tmpnwPath = path.join(curDir, 'tmp-nw');

  before(function(done) {
    this.timeout(0);

    server = createTCPServer(13013);
    server.on('connection', onConnection);

    if (is_chromedriver_exists) {
      func.copyExecFiles(function() {
        var src_files = [
          path.join(curDir, 'internal', 'index.html'),
          path.join(curDir, 'internal', 'package.json'),
          path.join(curDir, 'internal', 'test.py')
        ];
        var dst_files = [
          path.join(curDir, 'tmp-nw', 'index.html'),
          path.join(curDir, 'tmp-nw', 'package.json'),
          path.join(curDir, 'tmp-nw', 'test.py')
        ];

        if (process.platform == 'win32'){
          src_files.push(path.join(curDir, 'chromedriver2_server.exe'));
          dst_files.push(path.join(curDir, 'tmp-nw', 'chromedriver2_server.exe'));
        } else {
          src_files.push(path.join(curDir, 'chromedriver2_server'));
          dst_files.push(path.join(curDir, 'tmp-nw', 'chromedriver2_server'));
        }

        for (var i=0;i<src_files.length;++i){
          fs.copySync(src_files[i],dst_files[i]);
        }
        if (process.platform != 'win32'){
          fs.chmodSync(path.join(curDir, 'tmp-nw', 'chromedriver2_server'),365);
        }

        var test_file = path.join(curDir, 'tmp-nw', 'test.py');
        setTimeout(function(){
          var child = spawn('python',[test_file]);
          child.on('exit',function(code,signal){
            console.log(code,signal);
            done();
          });
          child.on('error',function(err){
            console.error(err);
          });
          child.stderr.on('data',function(data){
            console.error(data.toString());
          });
          child.stdout.on('data',function(data){
            console.log(data.toString());
          });
        },1000);  // setTimeout
		});
    } else {
      var originMsg = 'Hello 上海';
      var bufMsg = new Buffer(originMsg, 'utf8');
      if (bufMsg == originMsg)
        results.push(true);
      else
        results.push(false);
      done();
    }

  });


  after(function(done) {
    this.timeout(0);
    if (server) {
      server.removeListener('connection', onConnection);
      server.close();
    }
    setTimeout(function(){
      if (fs.existsSync(tmpnwPath)) {
        fs.removeSync(tmpnwPath);
      }
      done();
    },2000);

  });

  it("string and buff.toString() should equal", function() {
    assert.equal(results.length,1);
    assert.equal(results[0],true);
  });


});
