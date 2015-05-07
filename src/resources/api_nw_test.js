var nw_binding = require('binding').Binding.create('nw.test');
var nwNatives = requireNative('nw_natives');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('crashRenderer', function() {
    nwNatives.crashRenderer();
  });
  apiFunctions.setHandleRequest('done', function() {
    nw.process.exit(0);
  });
  apiFunctions.setHandleRequest('fail', function() {
    nw.process.exit(1);
  });
  apiFunctions.setHandleRequest('assert', function(value, message) {
    if (!value) { console.log(message); nw.process.exit(1); }
  });
});

exports.binding = nw_binding.generate();

