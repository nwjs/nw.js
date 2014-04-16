var os = require('os');
var path = require('path');
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
describe('nw in memory after quiting',function(){
  it('nw.exe should not be in memory after gui.App.quit is called on windows',function(done){
    this.timeout(0);
    if(os.platform() == "win32") {
      var app = spawn(process.execPath, [path.join(global.tests_dir,'nw-in-mem/package')])
      app.on('close',function(){
        var nwname = "nw.exe";
        var nwcount = 0;
        setTimeout(function() {
          exec("tasklist", function(err, stdout, stderr) {
            if(err){ return console.log(err); }
            stdout.split('\n').filter(function(line){
              var p=line.trim().split(/\s+/),pname=p[0];
              if(pname.toLowerCase().indexOf(nwname)>=0)
                  nwcount++;
            });

            if(nwcount == 2)
              done();
            else if(nwcount > 2)
              done('nw.exe is still in memory after the quit of app');
          })
        }, 500);
      });
    }
    else 
      done();
  })
})