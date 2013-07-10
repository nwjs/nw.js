var path = require('path');
var app_test = require('./nw_test_app');
var fs = require('fs-extra');
describe('data-path', function() {
	before(function(done){
		this.timeout(0);
	    fs.mkdirsSync('./data-cash');
	    fs.mkdirsSync('./dataPath');
	    fs.mkdirsSync('./dataPath/data-cash');
	    fs.mkdirsSync('./data-cash/dataPath');
	    done();
	  })

	  after(function() {
	    setTimeout(function() {
	      fs.remove('./data-cash');
	      fs.remove('./dataPath');
	      fs.remove('./dataPath/data-cash');
	      fs.remove('./data-cash/dataPath');
	    }, 1000);
	  })

  it('setting datapath as ./data-cash/ should pass',
    function(done) {
      this.timeout(0);
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath/datacash'),
        end: function(data, app) {
            app.kill();
            done();
        }
      });
  })
  it('setting datapath as ./dataPath/ should pass',
    function(done) {
      this.timeout(0);
      
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath/datapath'),
        end: function(data, app) {
            app.kill();
            done();
        }
      });
  })
  it('setting datapath as ./dataPath/data-cash/ should pass',
    function(done) {
      this.timeout(0);
      
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath/datapath-cash'),
        end: function(data, app) {
            app.kill();
            done();
        }
      });
  })
  it('setting datapath as ./data-cash/dataPath/ should pass',
    function(done) {
      this.timeout(0);
      
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath/datacash-path'),
        end: function(data, app) {
            app.kill();
            done();
        }
      });
  })
})
