var app_test = require('./nw_test_app');
var assert = require('assert');
var path = require('path');
var exec_path = process.execPath;

describe('fd-limit', function() {

    describe('file descriptor limit', function() {

        if (process.platform !== 'win32') {

        it('fetches 2k files via XHR should succeed with --file-descriptor-limit=8192', function(done) {
            this.timeout(5000);

            var child = app_test.createChildProcess({
                execPath: exec_path,
                appPath: global.tests_dir + '/fd-limit',
                args: '--file-descriptor-limit=8192',
                end: function(data, app) {
                    assert.equal(true, data.ok);
                    done();
                }
            });
            
        });

        it('fetches 2k files via XHR should fail with --file-descriptor-limit=100', function(done) {
            this.timeout(5000);

            var child = app_test.createChildProcess({
                execPath: exec_path,
                appPath: global.tests_dir + '/fd-limit',
                args: '--file-descriptor-limit=100',
                end: function(data, app) {
                    assert.equal(false, data.ok);
                    done();
                }
            });
            
        });

        }

    });

});
