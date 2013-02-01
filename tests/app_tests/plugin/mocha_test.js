var path = require('path');
var app_test = require('./nw_test_app');

describe('Plugin', function() {
  describe('flash', function(){
    it('should not crash', function(done) {
      var result = false;
      
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join('app_tests', 'plugin', 'flash'),
        end: function(data, app) {        
          done();
          result = true;
          app.kill(); 
        }
      });
      
      setTimeout(function(){ 
        if (!result) {
          done('nw crash.');
        }
      }, 3000);
        
      
    })
  })
})