var app_test = require('./nw_test_app');
var path = require('path');

describe('file input',function(){
  describe('set value',function(){
    it('the value should be set without exception',function(done){
      this.timeout(0);
      var child = app_test.createChildProcess({
        execPath : process.execPath,
        appPath : path.join(global.tests_dir, 'filevalue'),
        end: function(data, app){
          app.kill();
          if (data == 'mytestfile')
            done();
          else 
            done('the value of the file input is not set correctly');
        }
      })
    })
  })
})