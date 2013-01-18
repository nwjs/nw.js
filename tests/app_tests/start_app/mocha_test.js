var spawn = require('child_process').spawn;
var exec = require('child_process').exec;
var path = require('path');
var net = require('net');
var os = require('os');
var server = global.server;
var func = require('./app_tests/start_app/script.js');
var execPath = func.getExecPath();

describe('different method of startint app (long-to-run)', function() {
   
  before(function(done){
    this.timeout(0);
    func.copyExecFiles();  
    func.copySourceFiles();
    func.zipSourceFiles(function() {
      func.makeExecuableFile();     
      done();
    });
    
  })
  
  
  it('start from nw that package.json in the same folder', function(done) {
    this.timeout(0);
    
    if (!os.platform() == 'win32') {
      done();
      return;
    }
    
    var app;
    server.on('connection', function(s){
      s.setEncoding('utf8');            
    });
    app = spawn(execPath);
    app.on('exit', function() {
      done();
    });
  
  })
   
  it('start from app.nw', function(done) {
    this.timeout(0);
    
    if (!os.platform() == 'win32') {
      done();
      return;
    }
    
    var app;
    server.on('connection', function(s){
      s.setEncoding('utf8');            
    });
    app = spawn(execPath, [path.join('tmp', 'app.nw')]);
    app.on('exit', function() {
      done();
    });
  })
  
  it('start from an executable file app.exe', function(done) {
    this.timeout(0);
    
    if (!os.platform() == 'win32') {
      done();
      return;
    }
    
    var app;
    server.on('connection', function(s){
      s.setEncoding('utf8');            
    });
    if (os.platform() == 'win32') {
      execPath = path.join('tmp', 'app.exe');
    }
    app = spawn(execPath);
    app.on('exit', function() {
      done();
    });
  })
  
   
})