var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


/// 1
describe('require', function() {
  describe('type', function() {
    it('is function', function() {
      assert.equal(typeof global.require, 'function');
    });
  });

  describe('members', function() {
    it('have resolve', function() {
      assert.equal(typeof global.require.resolve, 'function');
    });

    it('have main', function() {
      assert.equal(typeof global.require.main, 'object');
    });

    it('have extensions', function() {
      assert.equal(typeof global.require.extensions, 'object');
    });

    it('have registerExtension', function() {
      assert.equal(typeof global.require.registerExtension, 'function');
    });

    it('have cache', function() {
      assert.equal(typeof global.require.cache, 'object');
    });
  });
});

////////////////// 2
/*
describe('node', function() {

  var mocha_callback = null;
  before(function() {
    mocha_callback = process.listeners('uncaughtException')[1];
    process.removeListener('uncaughtException', mocha_callback);
  });
  after(function(){
    process.on('uncaughtException',mocha_callback);
  })

  describe('process', function() {
    it('uncaughtException should have a default listener', function() {
      assert.equal(process.listeners('uncaughtException').length, 1);
    });

    it('throwing uncaughtException should not show error page when having listener', function(done) {
      var empty = function(e) {
        process.removeListener('uncaughtException', empty);
        done();
      };
      process.on('uncaughtException', empty);
      process.nextTick(function() {
        throw new String('If you see this then uncaughtException test failed');
      });
    });
  });
});
*/
////////////// 3

describe('module', function() {
  describe('javascript modules', function() {
    var path = require('path');

    it('require by path', function() {
      var module1_path = path.join(curDir, 'module1.js');
      assert.equal(require(module1_path).test(), 'module1');
    });

    it('simple file module', function() {
      var module3_path = path.join(curDir, 'node_modules', 'module3');
      assert.equal(require(module3_path).test(), 'module3');
    });

    it('space in path', function() {
      var module4_path = path.join(curDir, 'node_modules', 'module4 space');
      assert.equal(require(module4_path).test(), 'module4');
    });
  });

  describe('built-in modules', function() {
    var fs = require('fs');
    var http = require('http');
    var path = require('path');
    var exec = require('child_process').exec;

    it('execute child process', function(done) {
      var child = exec('echo echo back', function (error, stdout, stderr) {
        assert.equal(error, null);
        assert.equal(stdout, process.platform == 'win32' ? 'echo back\r\n' : 'echo back\n');
        assert.equal(stderr, '');
        done();
      });
    });

    it('Buffer', function() {
      assert.equal(String(Buffer('test')), 'test');
    });
/*
    it('http server and client', function(done) {
      var server = http.createServer(function (req, res) {
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end('hello');
      }).listen(10086);
      process.nextTick(function() {
        var count = 0;
        for (var i = 0; i < 5; ++i) {
          http.get("http://127.0.0.1:10086", function(res) {
            var data = '';
            res.on('end', function(e) {
              if (data == 'hello')
                ++count;
            }).on('data', function(chunk) {
              data += chunk;
            });
          }).on('error', function() {
            assert.equal(true, false);
          });
        }
        setTimeout(function() {
          assert.equal(count, 5);
          server.close();
          done();
        }, 50);
      });
    });
*/
  });

/*
  describe('native modules (long-to-run)', function() {
    it('bignum should work on posix environment', function() {
      if (process.platform != 'win32') {
        var bignum = require('./node_modules/bignum');
        assert.equal(bignum('782910138827292261791972728324982').sub('182373273283402171237474774728373').div(8), '75067108192986261319312244199576');
      }
    });

    it('native modules without handle scope', function() {
	require('./node_modules/nw_test_loop_without_handle');
    });
     
    it('native modules should work', function() {
	var nativeModules = new Array("dtrace-provider", "ref", "lame");
	for (var i = 0; i < nativeModules.length; i++)
	    assert.equal((typeof require(nativeModules[i])),  "object");
    });
  });
*/

});

////////////////////// 4

describe('performance', function() {
  describe('timers', function() {
    it('setTimeout in webkit and node.js should be accurate', function(done) {
      var flag1, flag2;
      global.setTimeout(function() { flag1 = true; }, 10);
      window.setTimeout(function() { flag2 = true; }, 10);

      setTimeout(function() {
        assert.equal(flag1, true);
        assert.equal(flag2, true);
        done();
      }, 50);
    });

    it('setInterval int webkit and node.js should be accurate', function(done) {
      var count1 = 0;
      var count2 = 0;;
      global.setInterval(function() { ++count1; }, 10);
      window.setInterval(function() { ++count2; }, 10);

      setTimeout(function() {
        assert.equal(count1 > 3, true);
        assert.equal(count2 > 3, true);
        done();
      }, 50);
    });
  });


  describe('IO', function() {
    var fs = require('fs');
    var path = require('path');
    var exec = require('child_process').exec;
    var tar = require('tar');
    var result = false;

    it('file reading should be quick even when we have a child process running', function(done) {
      this.timeout(0);
      var mochaPath = path.join(curDir, 'mocha_test.js');
      fs.readFile(mochaPath, 'utf8', function(err, data) {
        result = true;
        done();
      });

      setTimeout(function() {
        if (!result) done('timeout');
      }, 1000);
    });

    it('untar a file should be quick', function(done) {
      this.timeout(100);
      var testPath = path.join(curDir, 'test.tar');
      fs.createReadStream(testPath)
      .pipe(tar.Extract({ path: 'tmp' }))
      .on('error', function (er) {
        assert.equal(false, true);
      })
      .on('end', function () {
        var content = fs.readFileSync('tmp/text', 'utf8');
        assert.equal(content, 'I love luyao\n');   // who is luyao ?
        fs.unlinkSync('tmp/text');
        fs.rmdirSync('tmp');
        done();
      });
    });

  });


});

///////////////////////////// 5

describe('browser', function() {
  describe('security', function() {
    it('cross domain call in file:// should succeed', function(done) {
      window.$.get('http://baidu.com', function(data, status) {
        assert.equal(status, 'success');
        assert.notEqual(data, null);
        done();
      });
    });
  });

  describe('render crashes', function() {
    it('MessageChannel should not crash', function(done) {
      var testPath = path.join(curDir, 'message_channel_test');
      var test = require(testPath);
      test.run(done); // force to run in node context.
    });
  });
});


