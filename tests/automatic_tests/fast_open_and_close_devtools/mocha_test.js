var path = require('path');
var assert = require('assert');
var spawn = require('child_process').spawn;
var result = false;
describe('Fast open&close devtools from #1391', function() {
        before(function(done) {
                this.timeout(0);
                var app = spawn(process.execPath, [path.join(global.tests_dir, 'fast_open_and_close_devtools')]);
                app.on('exit', function(code) {
                        if (code != null)
                           result = true;
                        done();
                });
        });

        it('should not crash', function() {
                assert.equal(result, true);
        });
});
