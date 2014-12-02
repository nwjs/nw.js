var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('source maps', function() {

  var server, child, results, ok = false;

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        ok = true;
        results = JSON.parse(data);
        child.kill();
        done();
      });
    });

    setTimeout(function() {
      if (!ok) {
        child.kill();
        done('timeout');
      }
    }, 4500);

  });
    
  after(function () {
        server.close();
  });

    it('should support Closure Compile', function() {
	assert.equal("script.closure.js", results[0]);
	assert.equal("script1.js", results[8]);
    });
    
    it('should support JSMin and Grunt', function() {
	assert.equal("script.jsmin-grunt.js", results[4]);
	assert.equal("script2.js", results[9]);
    });
    
    it('should support Uglifyjs', function() {
	assert.equal('script.uglify.js', results[7]);
	assert.equal('script.js', results[3]);
    });
    
    it('should support CoffeeScript', function() {
	assert.equal('script.coffee.min.js', results[2]);
	assert.equal('script.coffee.coffee', results[1]);
    });
    
    it('should support TypeScript', function() {
	assert.equal('script.typescript.min.js', results[5]);
	assert.equal('script.typescript.ts', results[6]);
    });


});


