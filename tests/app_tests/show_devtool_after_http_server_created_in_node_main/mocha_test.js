var path = require('path');
var app_test = require('./app_test.js');

describe('AppTest - devtool', function() {
  describe('create http server in node-main', function() {
    it('nw should close by itself after show devtool',      	   
	  function(done) {
        this.timeout(0);
          /*
           * we should do the work here,
           * after get child process respond. 
           */
          function callback(socket, app) {
            socket.on('data', function(data) {
              if (data == 'ok') {
                done();    
              } else {
                done(data);    
              }
              app.kill();            
            });          
          }
      
        setTimeout(function(){ done('nw close by itself')}, 2000);
    
        app_test.createChildProcess(
            path.join('app_tests', 'show_devtool_after_http_server_created_in_node_main'),
            callback);
	    
    })
  

  })

})
