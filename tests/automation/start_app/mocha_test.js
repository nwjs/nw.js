var spawn = require('child_process').spawn;
var exec = require('child_process').exec;
var path = require('path');
var net = require('net');
var os = require('os');
var fs = require('fs-extra');
var curDir = require('fs').realpathSync('.');
var func = require(path.join(curDir, 'script.js'));
var execPath = path.join(curDir, func.getExecPath());

describe('Startup', function() {
describe('different method of starting app (long-to-run)', function() {

  before(function(done){
    this.timeout(10000);
    func.copyExecFiles(function() {
      func.copySourceFiles('internal');
      func.zipSourceFiles(function() {
        func.makeExecuableFile();
        done();
      });
    });

  });

  after(function(done) {
    setTimeout(function() {
      fs.remove(path.join(curDir, 'tmp-nw'), function (er) {
        if (er) {
          console.log('Failed to remove the temporary folder tmp-nw: ' + er);
          throw er;
        }
        done();
      })
    }, 1000);
  })

  it('start from nw that package.json in the same folder', function(done) {
    this.timeout(0);
    var result = false;

    if (os.platform() == 'darwin') {
      //mac don't have this method
      done();
      return;
    }

    var app = spawn(execPath);
    app.on('exit', function() {
      result = true;
      done();
    });

    setTimeout(function() {
      if (!result) {
        done('timeout');
        app.kill();
      }
    }, 10000);

  })

  it('start from app.nw', function(done) {
    this.timeout(0);
    var result = false;

    var app = spawn(execPath, [path.join(curDir, 'tmp-nw', 'app.nw')]);
    app.on('exit', function() {
      result = true;
      done();
    });
    setTimeout(function() {
      if (!result) {
        done('timeout');
        app.kill();
      }
    }, 10000);

  })

  it('start from folder contains `../`', function(done) {
    this.timeout(0);
    var result = false;
    var app = spawn(execPath, [path.join(curDir, '..', '..', 'tmp-nw')]);


    app.on('exit', function() {
      result = true;
      done();
    });

    setTimeout(function() {
      if (!result) {
        done('timeout');
        app.kill();
      }
    }, 10000);
  })

  it('start from an executable file app.exe', function(done) {
    this.timeout(0);
    var result = false;
    function launch(appPath) {
      var app = spawn(appPath);
      app.on('exit', function() {
        result = true;
        done();
      });

      setTimeout(function() {
        if (!result) {
          done('timeout');
          app.kill();
        }
      }, 10000);
    }

    if (os.platform() == 'win32') {
      launch(path.join(curDir, 'tmp-nw', 'app.exe'));
    }
    if (os.platform() == 'linux') {
      launch(path.join(curDir, 'tmp-nw', 'app'));
    }
    if (os.platform() == 'darwin') {
      var app_path = curDir + 'tmp-nw/node-webkit.app/Contents/Resources/app.nw';
      fs.mkdir(app_path, function(err) {
        if(err && err.code !== 'EEXIST') throw err
        fs.copy(path.join(curDir, 'tmp-nw', 'internal', 'index.html'), path.join(app_path, 'index.html'));
        fs.copy(path.join(curDir, 'tmp-nw', 'internal', 'package.json'), path.join(app_path, 'package.json'), function() {
          launch(app_path);
        });
      });
    }

  })

})
})
