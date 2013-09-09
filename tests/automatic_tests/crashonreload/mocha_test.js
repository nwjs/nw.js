var app_test = require('./nw_test_app');
describe('crash on reload',function(){
  it('nw should not crash when reloading with devtool opened',function(done){
    this.timeout(0);
    var result = false;
    var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'crashonreload'),
        end: function(data, app) {
          app.kill();
          result = true;
          if (data == 'ok') {
            done();
          }
          else 
            done('the nw crashes');
        }
    });
    setTimeout(function() {
      if (!result) {
        child.close();
        done('nw crashes when reloading with devtool opened');
      }
    }, 4000);
  })
})