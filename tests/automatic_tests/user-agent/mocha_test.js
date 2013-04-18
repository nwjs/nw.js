var assert = require('assert');
var fs = require('fs');
describe('user-agent', function() {
  var user_agent = navigator.userAgent.split('/');
  var package_info = JSON.parse(fs.readFileSync('package.json', 'utf8'));
  it('name should be same to the one in package.json',
    function() {
      assert.equal(user_agent[0], package_info.name);
    }
  )

  it('version should be same to the one in package.json',
    function() {
      assert.equal(user_agent[2], package_info.version);
    }
  ) 
})
