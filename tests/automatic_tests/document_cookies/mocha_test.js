var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var gui = require('nw.gui');
var results = new Array();
describe('document.cookies', function() {

    describe('http', function() {
	before(function(done) {
	    this.timeout(0);
	    var url = "http://127.0.0.1:8123/document_cookies.html";
	    var win = gui.Window.open(url);

	    setTimeout(function() {
		results.push(win.window.msg.textContent);
		done();
		win.window.close();
	    }, 1000);
	});
	it('should be set', function() {
	    assert.equal(results[0], '123');
	});
    });

    describe('file', function() {
	before(function(done) {
	    this.timeout(0);
	    var child = app_test.createChildProcess({
		execPath: process.execPath,
		appPath: path.join(global.tests_dir, 'document_cookies', 'file'),
		end: function(data, app) {
		    results.push(data);
                    app.kill();
                    done();
		}
	    });
	    setTimeout(done, 3000);
	});
	it ('should be set', function() {
	    assert.equal(results[1], '123');
	});
    });

    describe('app', function() {
	before(function(done) {
	    this.timeout(0);
	    var child = app_test.createChildProcess({
		execPath: process.execPath,
		appPath: path.join(global.tests_dir, 'document_cookies', 'app'),
		end: function(data, app) {
		    results.push(data);
                    app.kill();
                    done();
		}
	    });
	    setTimeout(done, 3000);
	});
	it ('should be set', function() {
	    assert.equal(results[2], '123');
	});
    });

});
