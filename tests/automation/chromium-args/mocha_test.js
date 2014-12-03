var assert = require('assert');
var cp = require('child_process');
var fs = require('fs');
var fs_extra = require('fs-extra');
var curDir = fs.realpathSync('.');
var exec_path = process.execPath;
var app_path = path.join(curDir, 'internal');

describe('chromium-args', function() {


    describe('--disable-javascript', function() {

        var child;

    	before(function(done) {
          this.timeout(0);
          fs_extra.copy(path.join(app_path, 'package1.json'), path.join(app_path, 'package.json'), function (err) {
            if (err) {
              throw err;
            }
            child = spawnChildProcess(path.join(curDir, 'internal'));
          });

          setTimeout(function(){done();}, 2500);
    	});

    	after(function() {
          if (child)
            child.kill();
    	  fs.unlink(path.join(app_path, 'hi'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	  fs.unlink(path.join(app_path, 'package.json'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	});
	
    	it('javascript should not be executed', function() {
    	    var result = fs.existsSync(path.join(app_path, 'hi'));
    	    assert.equal(result, false);
    	});

    });


    describe('--app=url', function() {
	  var result = false, result2 = false;

      var child, server;

      before(function(done) {
        this.timeout(0);
        fs_extra.copy(path.join(app_path, 'package2.json'), path.join(app_path, 'package.json'), function (err) {
          if (err) {
            throw err;
          }

          server = createTCPServer(13013);
          server.on('connection', function(socket) {
            socket.setEncoding('utf8');
            socket.on('data', function(data) {
              result2 = data;
              result = true;
              done();
            });
          });

          child = spawnChildProcess(path.join(curDir, 'internal'));

          setTimeout(function() {
            if (!result) {
              child.kill();
              done('timeout');
            }
          }, 4500);

        });
      });

    	after(function(done) {
          this.timeout(0);
          child.kill();
          server.close();
          fs.unlink(path.join(app_path, 'package.json'), function(err) {if(err && err.code !== 'ENOENT') throw err});
          fs.unlink(path.join(app_path, 'hi'), function(err) {if(err && err.code !== 'ENOENT') throw err});
          done();
    	});
	
    	it('website should be the url', function() {
    	    assert.equal(result2, true);
    	});
    

    });

});
