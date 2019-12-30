var cp = require('child_process');
var os = require('os');
var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('proxy', function(){
  before(function(done){
    if (os.platform() == "win32"){
      this.timeout(0);
      var ex = cp.exec(path.join(curDir, 'export.bat'),
      function(error, stdout, stderr){
        console.log(stdout);
        done();
      });
    }
    else 
      done();
  })
  after(function(){
    this.timeout(0);
    if (os.platform() == "win32"){
      fs.unlink('c:/test.reg', function (err) {
        if (err) console.log(err);
      });
    }
  })
  it('the get proxyfor url should work fine', function(done) {
    if (os.platform() == "win32"){
      this.timeout(0);
      setTimeout(function(){
        var data = fs.readFileSync("c:/test.reg",'utf16le');
        var index = data.indexOf('ProxyServer');
        var right = data.substring(index+14);
        var array = right.split('"');
        var gui = require('nw.gui');
        var re = gui.App.getProxyForURL("https://www.google.com.hk");
        if (re == "PROXY "+array[0])
          done();
        else 
          done('the method getProxyForURL is not right');
      },1000);
    }
    else {
      done();
    }
 });

});

