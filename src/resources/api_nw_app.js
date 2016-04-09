var nw_binding = require('binding').Binding.create('nw.App');
var nwNatives = requireNative('nw_natives');
var sendRequest = require('sendRequest');

var fullArgv = null;
var dataPath;

var eventsMap = {
  'open':             'onOpen',
  'reopen':           'onReopen'
};

var filteredArgv = [
  /^--url=/,
  /^--remote-debugging-port=/,
  /^--renderer-cmd-prefix=/,
  /^--nwapp=/
];

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('crashRenderer', function() {
    nwNatives.crashRenderer();
  });
  bindingsAPI.compiledApi.__defineGetter__('argv', function() {
    var fullArgv = this.fullArgv;
    var argv = fullArgv.filter(function(arg) {
      return !filteredArgv.some(function(f) {
        return f.test(arg);
      });
    });

    return argv;
  });
  bindingsAPI.compiledApi.__defineGetter__('fullArgv', function() {
    if (fullArgv)
      return fullArgv;
    fullArgv = nw.App.getArgvSync();
    return fullArgv;
  });
  bindingsAPI.compiledApi.__defineGetter__('filteredArgv', function() {
    return filteredArgv;
  });
  bindingsAPI.compiledApi.__defineSetter__('filteredArgv', function(newFilteredArgv) {
    return filteredArgv = newFilteredArgv;
  });
  bindingsAPI.compiledApi.__defineGetter__('manifest', function() {
    var ret= chrome.runtime.getManifest();
    if (ret.hasOwnProperty('__nwjs_manifest'))
      return ret['__nwjs_manifest'];
    return ret;
  });
  apiFunctions.setHandleRequest('getArgvSync', function() {
    return sendRequest.sendRequestSync('nw.App.getArgvSync', [], this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('setProxyConfig', function() {
    sendRequest.sendRequestSync('nw.App.setProxyConfig', arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('clearCache', function() {
    sendRequest.sendRequestSync('nw.App.clearCache', arguments, this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('getProxyForURL', function() {
    return nwNatives.getProxyForURL.apply(this, arguments);
  });
  apiFunctions.setHandleRequest('addOriginAccessWhitelistEntry', function() {
    nwNatives.addOriginAccessWhitelistEntry.apply(this, arguments);
  });
  apiFunctions.setHandleRequest('removeOriginAccessWhitelistEntry', function() {
    nwNatives.removeOriginAccessWhitelistEntry.apply(this, arguments);
  });
  apiFunctions.setHandleRequest('once', function(event, listener) { //FIXME: unify with nw.Window
    if (typeof listener !== 'function')
      throw new TypeError('listener must be a function');
    var fired = false;
    var self = this;

    function g() {
      self.removeListener(event, g);
      if (!fired) {
        fired = true;
        listener.apply(self, arguments);
      }
    }
    this.on(event, g);
    return this;
  });
  apiFunctions.setHandleRequest('on', function(event, callback) {
      if (eventsMap.hasOwnProperty(event)) {
        nw.App[eventsMap[event]].addListener(callback);
      }
  });
  apiFunctions.setHandleRequest('removeListener', function(event, callback) {
      if (eventsMap.hasOwnProperty(event)) {
        nw.App[eventsMap[event]].removeListener(callback);
      }
  });
  apiFunctions.setHandleRequest('removeAllListeners', function(event) {
    if (eventsMap.hasOwnProperty(event)) {
      for (let l of
           nw.App[eventsMap[event]].getListeners()) {
        nw.App[eventsMap[event]].removeListener(l.callback);
      }
    }
  });
  apiFunctions.setHandleRequest('getDataPath', function() {
    return sendRequest.sendRequestSync('nw.App.getDataPath', [], this.definition.parameters, {})[0];
  });
  bindingsAPI.compiledApi.__defineGetter__('dataPath', function() {
    if (!dataPath)
      dataPath = nw.App.getDataPath();
    return dataPath;
  });
  bindingsAPI.compiledApi.registerGlobalHotKey = function() {
    return nw.Shortcut.registerGlobalHotKey.apply(nw.Shortcut, arguments);
  };
  bindingsAPI.compiledApi.unregisterGlobalHotKey = function() {
    return nw.Shortcut.unregisterGlobalHotKey.apply(nw.Shortcut, arguments);
  };

});

exports.binding = nw_binding.generate();

