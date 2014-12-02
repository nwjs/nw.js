var spawn = require('child_process').spawn;
var path = require('path');
var fs = require('fs');
var curDir = fs.realpathSync('.');

describe('crash on reload',function(){
  it('nw should not crash when reloading with devtool opened',function(done){
    this.timeout(0);
    var result = false;

    var child = spawnChildProcess(path.join(curDir, 'internal'));
    child.on('exit', function (code){
      result = true;
      if (code != 0) {
        done('nw crashes');
        child.kill();
      } else {
        done();
      }

    });

    setTimeout(function(){
        if (!result) {
          child.kill();
          done();
        }

    }, 4500);
  });
});
