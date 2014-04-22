var assert = require('assert');

describe('website', function() {
  describe('scores', function() {
    var gui = require('nw.gui');

    it('html5test.com should score high (long-to-run)', function(done) {
      this.timeout(0);
      var win = gui.Window.open('http://html5test.com', { show: true });
      win.on('loaded', function() {
        //do not read data immediately, it is a RESTful site.
        //wait 4 seconds
        setTimeout(function(){
          var results = win.window.document.querySelectorAll('div.pointsPanel strong');
          if (results.length === 0){
            done('Can not connect to the web');
            return;
          }

          var score = parseInt(results[0].innerHTML);
          if (score >= 445) {
            done();
          } else {
            done('have a low score');
          }
          win.close();
        },4000);
      });
    });
    
    
    it('should support WebGL at get.webgl.org', function(done) {
      this.timeout(0);
      var win = gui.Window.open('http://get.webgl.org', { show: true });
      win.on('loaded', function() {
        var results = win.window.document.getElementById('webgl-yes');
        if (results.classList.contains('webgl-hidden')) {
          done('do not support WebGL');
        } else {
          done();
        }
        win.close();
      });
    })
    
    
  });
});
