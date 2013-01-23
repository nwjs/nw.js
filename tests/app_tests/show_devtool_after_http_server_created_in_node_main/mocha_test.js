var path = require('path');
var app_test = require('./nw_test_app');

describe('AppTest - devtool', function() {
  describe('create http server in node-main', function() {
    it('nw should not close by itself after show devtool',      	   
	  function(done) {
        this.timeout(0);
               
        setTimeout(function(){ done('nw close by itself')}, 3000);
    
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
          }
        });
        
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});
            
	    
    })
  

  })

})
