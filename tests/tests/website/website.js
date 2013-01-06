var assert = require('assert');

describe('website', function() {
  describe('scores', function() {
    var gui = require('nw.gui');

    it('html5test.com should score high (long-to-run)', function(done) {
      this.timeout(0);
      var win = gui.Window.open('http://html5test.com', { show: false });
      win.on('loaded', function() {
        var results = win.window.document.getElementById('results');
        if (results == null){
          done('Can not connect to the web');          
          return;
        }
        var score = results.childNodes[0].childNodes[1].innerHTML;;
        if (score >= 445) {
          done();
        } else {
          done('have a low score');
        }
        
      });
      gui.Window.get().on('close', function() {
        win.close(true);
        this.close(true);
      });
    });
  });
});
