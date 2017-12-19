onconnect = function(e) {
  var port = e.ports[0];
  try{
    var fs = require('fs');
    port.postMessage('success');
  } catch (e) {
    port.postMessage(e.message);
  }
}
