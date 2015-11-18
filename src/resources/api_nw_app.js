var nw_binding = require('binding').Binding.create('nw.App');
var nwNatives = requireNative('nw_natives');
var sendRequest = require('sendRequest');

var argv = null;

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('crashRenderer', function() {
    nwNatives.crashRenderer();
  });
  apiFunctions.setHandleRequest('argv', function() {
    if (argv)
      return argv;
    argv = nw.App.getArgvSync();
    return argv;
  });
  apiFunctions.setHandleRequest('getArgvSync', function() {
    return sendRequest.sendRequestSync('nw.App.getArgvSync', [], this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('addOriginAccessWhitelistEntry', function() {
    nwNatives.addOriginAccessWhitelistEntry.apply(this, arguments);
  });
  apiFunctions.setHandleRequest('removeOriginAccessWhitelistEntry', function() {
    nwNatives.removeOriginAccessWhitelistEntry.apply(this, arguments);
  });
});

exports.binding = nw_binding.generate();

