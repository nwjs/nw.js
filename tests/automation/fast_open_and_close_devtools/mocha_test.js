var path = require('path');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

var result = false;
describe('Fast open&close devtools from #1391', function() {
        before(function(done) {
                this.timeout(0);
                var app = spawnChildProcess(path.join(curDir, 'internal'));
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
