var gui = require('nw.gui');
var assert = require('assert');
require('should');

describe('Shortcut', function() {
  describe('.key', function() {
    it('should be undefined if no key specified.', function() {
      var shortcut;
      try {
        shortcut = new gui.Shortcut();
      } catch (err) {}
      assert.equal(shortcut, undefined);
    });

    it('should be an object if key specified.', function() {
      var shortcut;
      try {
        shortcut = new gui.Shortcut({"key": "Alt+Shift+S"});
      } catch (err) {}

      shortcut.should.be.a.Object;
      assert.equal(shortcut.key, "Alt+Shift+S");
    });
  });

  describe('.active', function() {
    it('should be undefined if active is not an function object.', function() {
      var shortcut;
      try {
        shortcut = new gui.Shortcut({key: 'Alt+Shift+S', active: "foo"});
      } catch(err) {}

      assert.equal(shortcut, undefined);
    });

    it('should be an object if "key" and "active" specified.', function() {
      var onActive = function() {};
      var shortcut = new gui.Shortcut({key: 'Alt+Shift+S', active: onActive});

      shortcut.should.be.a.Object;
      assert.equal(shortcut.active, onActive);
    });
  });

  describe('.failed', function() {
    it('should be undefined if "failed" is not a function object.', function() {
      var shortcut;
      try {
        shortcut = new gui.Shortcut({key: 'Alt+Shift+S', failed: "foo"});
      } catch(err) {}

      assert.equal(shortcut, undefined);
    });

    it('should be an object if "key" and "failed" specified.', function() {
      var onFailed = function() {};
      var shortcut = new gui.Shortcut({key: 'Alt+Shift+S', failed: onFailed});

      shortcut.should.be.a.Object;
      assert.equal(shortcut.failed, onFailed);
    });
  });
});

describe('App.registerGlobalHotKey', function() {
  it('should register failed if the parameter is not an Shortcut object.',
    function(done) {
      var object = new Object();
      try {
        gui.App.registerGlobalHotKey(object);
      } catch(err) {
        done();
      }
    }
  );

  it('should register failed if the same key has been registered.',
    function(done) {
      var shortcut = new gui.Shortcut({key: "Alt+Shift+S"});
      shortcut.failed = function(msg) {
        assert.equal(msg, "Register global desktop keyboard shortcut failed.");
        done();
      };

      gui.App.registerGlobalHotKey(shortcut);
      gui.App.registerGlobalHotKey(shortcut);
    }
  );

  it('should register failed for invalid key.', function(done) {
    var shortcut = new gui.Shortcut({key: "foo"});
    shortcut.failed = function(msg) {
      done();
    }
    gui.App.registerGlobalHotKey(shortcut);
  });
});

describe('App.unregisterGlobalHotKey', function() {
  it('should unregister failed if the parameter is not an Shortcut object.',
    function(done) {
      var object = new Object();
      try {
        gui.App.unregisterGlobalHotKey(object);
      } catch(err) {
        done();
      }
    }
  );
});
