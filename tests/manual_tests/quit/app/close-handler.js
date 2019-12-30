exports.init = function(win, nwGui) {
  console.log('Setting up close handler');
  win.on('close', function(how) {
    console.log('Got CLOSE event in close-handler.js with "' + how + '"');
    win.hide();
    var http = require('http');
    http.get("http://localhost:9000/close");
    setTimeout(function() {
      console.log('Quitting from close handler');
      nwGui.App.quit();
    }, 5000);
  });
};
