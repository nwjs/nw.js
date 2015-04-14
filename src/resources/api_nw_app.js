var nw_binding = require('binding').Binding.create('nw.App');
var nwNatives = requireNative('nw_natives');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('crashRenderer', function() {
    nwNatives.crashRenderer();
  });
});

exports.binding = nw_binding.generate();

