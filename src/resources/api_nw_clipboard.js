var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Clipboard');
var sendRequest = require('sendRequest');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;

  ['getSync', 'setSync', 'clearSync'].forEach(function(nwSyncAPIName) {
    apiFunctions.setHandleRequest(nwSyncAPIName, function() {
      return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {})[0];
    });
  });

  ['readAvailableTypes'].forEach(function(nwSyncAPIName) {
    apiFunctions.setHandleRequest(nwSyncAPIName, function() {
      return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
    });
  });
});

var nwClipboardBinding = nw_binding.generate();

function NWClipboard() {

}

var clipboard = null;

NWClipboard.get = function() {
  if (clipboard)
    return clipboard;
  clipboard = new NWClipboard();
  return clipboard;
};

NWClipboard.prototype.get = function (type, raw) {
  return nwClipboardBinding.getSync(type || 'text', !!raw);
};
NWClipboard.prototype.set = function (content, type, raw) {
  return nwClipboardBinding.setSync(content, type || 'text', !!raw);
};
NWClipboard.prototype.clear = function () {
  return nwClipboardBinding.clearSync();
};
NWClipboard.prototype.readAvailableTypes = function() {
  return nwClipboardBinding.readAvailableTypes();
};

exports.binding = NWClipboard;

