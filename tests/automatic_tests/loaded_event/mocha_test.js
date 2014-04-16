
var path = require('path');
var app_test = require('./nw_test_app');

describe('loaded event', function() {

    it('loaded event can been fired',
	  function(done) {
        this.timeout(0);
        var result = false;

        var child = app_test.createChildProcess({
          execPath: process.execPath,
          appPath: path.join(global.tests_dir, 'loaded_event'),
          end: function(data, app) {
            result = true;
            done();
            app.kill();
          }
        });


        setTimeout(function(){
          if (!result) {
            child.close();
            //child.app.kill();
            //child.removeConnection();
            done('loaded evenet does not been fired');
          }
        }, 3000);
        //child.app.stderr.on('data', function(d){ console.log ('app' + d);});

    })


})

