var path = require('path');
var assert = require('assert');

describe('window', function() {
    this.timeout(0);
    before(function(done) {
	this.timeout(0);


    var checkDone = function() {
      if (test_done) {
        done();
      } else {
        setTimeout(function() {checkDone();}, 2000);;
      }
    };

    checkDone();

    });

    describe('focus()', function() {
	it('should focus on the window', function() {
	    for (var i = 0; i < 3; i++) {
          console.log('win_c['+i+'] = '+ win_c[i]);
          assert.notEqual(win_c[i], "0");
        }
	});
    });
});
