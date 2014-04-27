var path = require('path');
var app_test = require('./nw_test_app');
var assert = require('assert');
var gui = require('nw.gui');
var results = new Array();
describe('document.cookies', function() {
//this case could be very slow on OS X
//leave child process enough time to finish its job
	this.timeout(10000);
	describe('http', function() {
		before(function(done) {
			this.timeout(0);
			var url = "http://127.0.0.1:8123/document_cookies.html";
			var win = gui.Window.open(url);

			win.on("loaded", function() {
				results.push(win.window.msg.textContent);
				done();
				win.window.close();
			});
		});
		it('should be set', function() {
			assert.equal(results[0], '123');
		});
	});

	describe('file', function() {
		before(function(done) {
			this.timeout(0);
			var crontab = undefined;
			var child = app_test.createChildProcess({
				execPath: process.execPath,
				appPath: path.join(global.tests_dir, 'document_cookies', 'file'),
				end: function(data, app) {
					if (crontab !== undefined){
						clearTimeout(crontab);
					}
					results.push(data);
					app.kill();
					done();
				}
			});
			crontab = setTimeout(done, 10000);
		});
		it('should be set', function() {
			assert.equal(results[1], '123');
		});
	});

	describe('app', function() {
		before(function(done) {
			this.timeout(0);
			var crontab = undefined;
			var child = app_test.createChildProcess({
				execPath: process.execPath,
				appPath: path.join(global.tests_dir, 'document_cookies', 'app'),
				end: function(data, app) {
					if (crontab !== undefined){
						clearTimeout(crontab);
					}
					results.push(data);
					app.kill();
					done();
				}
			});
			crontab = setTimeout(done, 10000);
		});
		it('should be set', function() {
			assert.equal(results[2], '123');
		});
	});

});