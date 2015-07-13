var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Object');
var sendRequest = require('sendRequest');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('create', function() {
    return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('destroy', function() {
    return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('callObjectMethod', function() {
    return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('callObjectMethodSync', function() {
    return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
  });
});

exports.binding = nw_binding.generate();

