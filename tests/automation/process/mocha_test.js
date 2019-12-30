var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('process', function() {
  describe('exit', function(){
    it('event process.exit should be fired after calling win.close(true)', function(done) {


      var child = spawnChildProcess(path.join(curDir, 'exit'));

      setTimeout(function() {
        var tmpFilePath = path.join(curDir, 'exit', 'a.txt');
        fs.exists(tmpFilePath, function (exists) {
          if (exists) {
            fs.unlink(tmpFilePath);
            done();
          } else {
            done('the event `exit` does not been called.');
          }
        });

      }, 1500);


    });

  });

});

