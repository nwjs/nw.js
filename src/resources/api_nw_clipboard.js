var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Clipboard');
var sendRequest = require('sendRequest');

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;

  ['clearSync', 'setListSync'].forEach(function(nwSyncAPIName) {
    apiFunctions.setHandleRequest(nwSyncAPIName, function() {
      return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {})[0];
    });
  });

  ['readAvailableTypes', 'getListSync'].forEach(function(nwSyncAPIName) {
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
  var toString = $Object.prototype.toString;
  // get(ClipboardData[])
  if (arguments.length === 1 && toString.apply(type) === '[object Array]') {
    return nwClipboardBinding.getListSync(type);
  }
  // get(DOMString, boolean)
  if (!type || toString.apply(type) === '[object String]') {
    return nwClipboardBinding.getListSync([{type: type || 'text', raw: !!raw}])[0].data;
  }
  // get(ClipboardData)
  if (arguments.length === 1) {
    return nwClipboardBinding.getListSync([type])[0].data;
  }
  throw new TypeException('Expecting get(string, optional boolean) or get(ClipboardData[]) or get(ClipboardData).');
};
NWClipboard.prototype.set = function (content, type, raw) {
  var toString = $Object.prototype.toString;
  // set(ClipboardData[])
  if (arguments.length === 1 && toString.apply(content) === '[object Array]') {
    return nwClipboardBinding.setListSync(content);
  }
  // set(DOMString, DOMString, boolean)
  if (toString.apply(content) === '[object String]') {
    return nwClipboardBinding.setListSync([{type: type || 'text', raw: !!raw, data: content}]);
  }
  // set(ClipboardData)
  if (arguments.length === 1) {
    return nwClipboardBinding.setListSync([content]);
  }
  throw new TypeException('Expecting set(string, optional string, optional boolean) or set(ClipboardData[]) or set(ClipboardData).');
};
NWClipboard.prototype.clear = function () {
  return nwClipboardBinding.clearSync();
};
NWClipboard.prototype.readAvailableTypes = function() {
  return nwClipboardBinding.readAvailableTypes();
};

exports.binding = NWClipboard;

