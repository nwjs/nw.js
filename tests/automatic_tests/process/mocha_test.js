var path = require('path');
var app_test = require('./nw_test_app');
var fs = require('fs-extra');


describe('process', function() {
  describe('exit', function() {
    it('event process.exit should be fired after calling win.close(true)', function(done) {
      var crontab = undefined;
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'process', 'exit'),
        no_connect: true,
      });

      child.app.on('exit',function(){
        fs.exists(path.join(global.tests_dir, 'process', 'exit', 'a.txt'), function(exists) {
            if (crontab){
              clearTimeout(crontab);
            }
            if (exists) {
              fs.unlink(path.join(global.tests_dir, 'process', 'exit', 'a.txt'), done);
            } else {
              done('the event `exit` does not been called.');
            }
          });
      });

      crontab = setTimeout(function() {
        console.log("CHILD PROCESS FAIL TO EXIT");
        done('the event `exit` does not been called.');
      }, 5000);

    })
  })
})