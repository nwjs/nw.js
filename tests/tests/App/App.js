var gui = require('nw.gui');
var assert = require('assert');

describe('gui.App', function() {
  describe('manifest', function() {
    it('`gui.App.manifest` should equle to value of package.json', function() {
      assert.equal(gui.App.manifest.name, 'nw-tests');       
    })

    it('have window', function() {
      assert.equal(typeof gui.App.manifest.window, 'object');       
    })

    it('have main', function() {
      assert.equal(typeof gui.App.manifest.main, 'string');
      assert.equal(gui.App.manifest.main, 'index.html');
    })

    it('have dependencies', function() {
      assert.equal(typeof gui.App.manifest.dependencies, 'object');       
    })
  
  })
})
