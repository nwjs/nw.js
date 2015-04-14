var Binding = require('binding').Binding;
var nw_binding = require('binding').Binding.create('nw.Window');
var nwNatives = requireNative('nw_natives');
var forEach = require('utils').forEach;

var currentNWWindow = null;
var currentNWWindowInternal = null;

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('get', function() {
    if (currentNWWindow)
      return currentNWWindow;

    currentNWWindowInternal =
        Binding.create('nw.currentWindowInternal').generate();
    var NWWindow = function() {
      //this.appWindow = chrome.app.window.current();
    };
    forEach(currentNWWindowInternal, function(key, value) {
      NWWindow.prototype[key] = value;
    });
    currentNWWindow = new NWWindow;
    return currentNWWindow;
  });
});

exports.binding = nw_binding.generate();

