function mytest(a) {
  document.write(a + 42);
    
  var gui = require('nw.gui');
  var result = { ok: true };

    var socket = require('net').connect({port: 13013});
    socket.setEncoding('utf8');
    socket.end(JSON.stringify(result));
    
    
}
