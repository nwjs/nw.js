var nw_binding = require('binding').Binding.create('nw.Shell');
var sendRequest = require('sendRequest');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('openExternal', function() {
    sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('openItem', function() {
    sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('showItemInFolder', function() {
    sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
});

exports.binding = nw_binding.generate();

