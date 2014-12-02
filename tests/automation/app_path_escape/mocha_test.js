var spawn = require('child_process').spawn;
var os = require('os');
var path = require('path');
var global = require('../globals');

describe('app_path_escaping',function(){
  it('app should start normally with directory named xdk',function(done){
    this.timeout(0);
    var opened = false;
    var app = spawn(process.execPath, [path.join(global.tests_dir,  'xdk')]);
    app.on('close',function(){
      opened = true;
      done();
    });

    setTimeout(function() {
      if(!opened){
        app.kill();
        done('the app is not executed');
      }
    }, 5000);

  });
});

