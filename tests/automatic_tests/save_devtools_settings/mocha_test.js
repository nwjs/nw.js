var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var original;
var changed;
var ok = false;

function read_changed(done) {
    if (!ok)
	setTimeout(read_changed, 2000, done);
    else {
	var child = app_test.createChildProcess({
	    execPath: process.execPath,
	    appPath: path.join(global.tests_dir, 'save_devtools_settings'),
	    args: [0],
	    end: function(data, app) {
		changed = data;
		app.kill();
		done();
		console.log("secnond");
	    }
	});
    }
}

describe('save_devtools_settings', function() {
    
    before(function(done) {
	this.timeout(0);
	var child = app_test.createChildProcess({
	    execPath: process.execPath,
	    appPath: path.join(global.tests_dir, 'save_devtools_settings'),
	    args: [1],
	    end: function(data, app) {
		original = data;
		app.kill();
		ok = true;
		console.log("first");
	    }
	});
	setTimeout(read_changed, 2000, done);
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
