var assert = require('assert');

describe('website', function() {
  describe('scores', function() {
    var gui = require('nw.gui');

    it('html5test.com should score high', function(done) {
      this.timeout(10000);
      var win = gui.Window.open('http://html5test.com', { show: false });
      win.on('loaded', function() {
        var score = win.window.document.getElementById('results').childNodes[0].childNodes[1].innerHTML;;
        win.close(true);
        assert.equal(score >= 445, true);
        done();
      });
    });
  });
});
