var path = require('path');
var app_test = require('./nw_test_app');

describe('node-main', function() {
  describe('create http server in node-main', function() {
    it('nw should not close by itself after show devtool',      	   
	  function(done) {
        this.timeout(0);
        var result = false; 
            
        var child = app_test.createChildProcess({
          execPath: process.execPath,
          appPath: path.join('app_tests', 'show_devtool_after_http_server_created_in_node_main'),
          end: function(data, app) {
            
            if (data.success) {
              done();    
            } else {
              done('erro');    
            }
            app.kill();
            result = true;
          }
        });
        
        setTimeout(function(){
          if (!result) {
            done('nw close by itself')
          }
        }, 3000);
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});
            	    
    })  
  })
  
  describe('call require() in app', function() {
    it('nw should can require modules', function(done){
      this.timeout(0);
      
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join('app_tests', 'call_require_with_node-main_set'),
        end: function(data, app) {
          
          if (data.ok) {
            done();    
          } else {
            done(data.error);    
          }
          app.kill();
        }
      });
    })
  })

})
