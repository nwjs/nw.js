var spawn = require('child_process').spawn;
var path = require('path');
var app_test = require('./nw_test_app');
describe('crash on reload',function(){
  it('nw should not crash when reloading with devtool opened',function(done){
    this.timeout(0);
    var result = false;
    var exec_argv = [path.join(global.tests_dir, 'crashonreload')];

    var app = spawn(process.execPath, exec_argv);
      app.on('exit', function (code){
      if (code != 0) return done('nw crashes');
        result = true;
        done();
      });
    setTimeout(function(){
        if (!result) {
          app.kill();
          done();
        }
    }, 7500);
  })
})