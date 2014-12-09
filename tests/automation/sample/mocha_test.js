var assert = require('assert');

describe('sample-test', function() {

  beforeEach(function() {
    console.log('before each');
  });

  before(function() {
    console.log('before');
    //assert.ok(false);
  });

  describe('#indexOf()', function() {                                // unit test group 1
    it('should return -1 when value is not presenttt', function() {  // unit test 1.1
      assert.equal(-1, [1, 2, 3].indexOf(5));
      assert.equal(-1, [1, 2, 3].indexOf(6));
      assert.equal(1, [1, 2, 3].indexOf(2));
    });
  });

  describe('#length', function() {       // unit test group 2
    it('should return 3', function() {  // unit test 2.1
      assert.equal(3, [1,2,3].length);
    });
    it('should return 0', function() {  // unit test 2.2
      assert.equal(0, ''.length);
    });

  });
 
});
