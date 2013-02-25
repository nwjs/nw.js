var path = require('path');
var app_test = require('./nw_test_app');
var os = require('os');
var fs = require('fs-extra');
var cp = require('child_process');

describe('snapshot', function() {

    before(function(done) {
      var snapshotExec;
      if (os.platform() == 'darwin' || os.platform() == 'linux') {
        snapshotExec = 'nwsnapshot';
      }
      if (os.platform() == 'win32') {
        snapshotExec = 'nwsnapshot.exe';
      }
      
      var snapshotPath = path.join(process.execPath, '..', snapshotExec);
      
      cp.execFile(snapshotPath,
                  ['--extra_code', 'mytest.js', 'mytest.bin'], 
                  {cwd:'./app_tests/snapshot/'}, 
                  function (error, stdout, stderr) {
                    done();
                  }
   
      );  
    })  
    
    after(function() {
      fs.unlink(path.join('app_tests', 'snapshot', 'mytest.bin'));
      fs.unlink(path.join('app_tests', 'snapshot', 'v8.log'));
    })
   
    it('the native code cloud be exectuted',      	   
	  function(done) {	  
        this.timeout(0);
        var result = false; 
            
        var child = app_test.createChildProcess({
          execPath: process.execPath,
          appPath: path.join('app_tests', 'snapshot'),
          end: function(data, app) {
            done();
            app.kill();
            result = true;
          }
        });
        
        setTimeout(function(){
          if (!result) {
            done('the native code does not been executed');
            child.removeConnection();
            child.app.kill();
          }
        }, 3000);
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});
         	    
    })  
  

})
