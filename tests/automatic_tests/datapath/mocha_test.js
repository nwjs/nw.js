var assert = require('assert');
var fs = require('fs');
var path = require('path');
var gui = require('nw.gui');
var app_test = require('./nw_test_app');
describe('data-path', function() {
  it('setting datapath as ./data-cash/ should pass',
    function(done) {
      this.timeout(0);
      var child = app_test.createChildProcess({
        execPath: process.execPath,
        appPath: path.join(global.tests_dir, 'datapath/datacash'),
        end: function(data, app) {
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
            done();
        }
      });
  })
})
