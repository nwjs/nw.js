var assert = require('assert');
var fs = require('fs');
var path = require('path');
var nw = require('./nw');

describe('fd-limit', function() {

    describe('file descriptor limit', function() {

        before(function(done) {
            this.timeout(5000);
            fs.mkdir('testfiles', function(err) {
                if (err) throw err;
                var n = 0;
                (function next() {
                    fs.writeFile('testfiles/' + n + '.json', JSON.stringify({i: n}), function(err) {
                        if (err) throw err;

                        if (++n === 2000) {
                            done();
                        } else {
                            next();
                        }
                    });
                }());
            });
        });

        after(function(done) {
            this.timeout(5000);
            var n = 0;
            (function next() {
                fs.unlink('testfiles/' + n + '.json', function(err) {
                    if (err) throw err;

                    if (++n === 2000) {
                        fs.rmdir('testfiles', function(err){
                            if (err) throw err;
                            done();
                        });
                    } else {
                        next();
                    }
                });
            }());
        });

        it('fetches 2k files via XHR should succeed with --file-descriptor-limit=8192', function(done) {
            if (process.platform === 'win32') {
                assert.equal(true, true);
                done();
            }

            this.timeout(5000);

            nw.spawn(['test_app', '--file-descriptor-limit=8192'], function(err, data) {
                if (err) throw err;
                assert.equal(data.ok, true);
                done();
            });
            
        });

        it('fetches 2k files via XHR should fail with --file-descriptor-limit=100', function(done) {
            if (process.platform === 'win32') {
                assert.equal(true, true);
                done();
            }

            this.timeout(5000);

            nw.spawn(['test_app', '--file-descriptor-limit=100'], function(err, data) {
                if (err) throw err;
                assert.equal(data.ok, false);
                done();
            });
            
        });

    });

});
