var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Clipboard');
var sendRequest = require('sendRequest');

var clipboard = null;

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('get', function() {
    if (clipboard)
      return clipboard;
    var NWClipboard = function() {};
    NWClipboard.prototype.get = function (type) {
      return nw.Clipboard.getSync(type);
    };
    NWClipboard.prototype.set = function (content, type) {
      return nw.Clipboard.setSync(content, type);
    };
    clipboard = new NWClipboard;
    return clipboard;
  });

  apiFunctions.setHandleRequest('getSync', function(type) {
      return sendRequest.sendRequestSync('nw.Clipboard.getSync', [type], this.definition.parameters, {})[0];
  });
});

exports.binding = nw_binding.generate();

