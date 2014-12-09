var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


var original;
var changed;
var ok = false;
var server, child;

var spawnChild = function(action) {
    return spawn(process.execPath, [path.join(curDir, 'internal'), action]);
};

function read_changed(done) {
    if (!ok) {
	  setTimeout(read_changed, 2000, done);
    } else {
      var tmpChild = spawnChild(0);
      server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        changed = data;
        tmpChild.kill();
        done();
        console.log("secnond");
      });
      });
      
    }
}

describe('save_devtools_settings', function() {

  before(function(done) {
    this.timeout(0);
    server = createTCPServer(13013);
    child = spawnChild(1);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = JSON.parse(data);
        original = data;
        ok = true;
        child.kill();
        done();
        console.log("first");
      });
    });

    setTimeout(read_changed, 2000, done);

  });
    
  after(function () {
        server.close();
  });

    
    it("should save devtools' settings", function() {
	var i = 0;
	// general
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.equal((original[i] + 1) % 4, changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;

	assert.equal((original[i] + 1) % 4, changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	if (!original[i - 1])
	    assert.equal((original[i] + 1) % 10, changed[i]);
	i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;

	// overrides
	assert.notEqual(original[i], changed[i]); i++;
	if (!original[i - 1])
	    // 0~21                                                                                               
	    assert.equal((original[i] + 1) % 22, changed[i]);
	i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	if (!original[i - 1]) {
	    assert.equal((original[i] + 1) % 10, changed[i]); i++;
	    assert.equal((original[i] + 1) % 10, changed[i]); i++;
	}
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	if (!original[i - 1]) {
	    assert.equal((original[i] + 1) % 10, changed[i]); i++;
	    assert.equal((original[i] + 1) % 10, changed[i]); i++;
	    assert.equal((original[i] + 1) % 10, changed[i]); i++;
	}
	assert.notEqual(original[i], changed[i]); i++;
	assert.notEqual(original[i], changed[i]); i++;
	if (!original[i - 1])
	    assert.equal((original[i] + 1) % 9, changed[i]);
	i++;

    });
    
});


