var spawn = require('child_process').spawn;
var exec = require('child_process').exec;
var path = require('path');
var net = require('net');
var os = require('os');
var server = global.server;
var func = require('./app_tests/start_app/script.js');
var execPath = func.getExecPath();
var fs = require('fs-extra');

describe('Startup', function() {
describe('different method of starting app (long-to-run)', function() {
  var temp_root = 'tmp-nw';
  before(function(done){
    this.timeout(0);
    func.copyExecFiles();  
    func.copySourceFiles();
    func.zipSourceFiles(function() {
      func.makeExecuableFile();
      done(); 
    });
    
  })
  
  
  after(function() {
    setTimeout(function() {      
      fs.remove('tmp-nw', function (er) {
        if (er) throw er;
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
    
    var app = spawn(execPath, [path.join('tmp-nw', 'app.nw')]);
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
      
    if (os.platform() == 'win32') {
      execPath = path.join('tmp-nw', 'app.exe');
    }
    if (os.platform() == 'linux') {
      execPath = path.join('tmp-nw', 'app');
    }
    if (os.platform() == 'darwin') {
      var app_path = 'tmp-nw/node-webkit.app/Contents/Resources/app.nw';
      fs.mkdir(app_path);
      fs.copy('tmp-nw/index.html', path.join(app_path, 'index.html'));
      fs.copy('tmp-nw/package.html', path.join(app_path, 'package.html'));
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
  
   
})
})