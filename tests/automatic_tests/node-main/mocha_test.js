var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');

describe('node-main', function() {
  describe('create http server in node-main', function() {
    it('nw should not close by itself after show devtool',
	  function(done) {
        this.timeout(0);
        var result = false;

        var child = app_test.createChildProcess({
          execPath: process.execPath,
          appPath: path.join(global.tests_dir,
              'show_devtool_after_http_server_created_in_node_main'),
          end: function(data, app) {
            app.kill();
            if (data.success) {
              done();
            } else {
              done('erro');
            }     
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
  });

  describe('call require() in app', function() {
    it('nw should can require modules', function(done){
      this.timeout(0);

      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'call_require_with_node-main_set'),
        end: function(data, app) {
          app.kill();
          if (data.ok) {
            done();
          } else {
            done(data.error);
          }  
        }
      });
    })
  });

  describe('reference node-main module',function(){
    it('nw should be able to reference node-main module',function(done){
      this.timeout(0);

      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'reference-node-main'),
        end: function(data, app) {
          app.kill();
          assert.equal(data,true);
          done();
        }
      });
    });
  });

})
