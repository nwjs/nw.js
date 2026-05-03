var nwNatives = requireNative('nw_natives');

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

apiBridge.registerCustomHook(function(bindingsAPI) {
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
    return nw.App.getArgvSync();
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
    return bindingUtil.sendRequestSync('nw.App.getArgvSync', [], undefined, undefined);
  });
  apiFunctions.setHandleRequest('setProxyConfig', function() {
    bindingUtil.sendRequestSync('nw.App.setProxyConfig', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('clearCache', function() {
    bindingUtil.sendRequestSync('nw.App.clearCache', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('clearAppCache', function() {
    bindingUtil.sendRequestSync('nw.App.clearAppCache', $Array.from(arguments), undefined, undefined);
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

  // Event methods defined directly on compiledApi to bypass the API bridge's
  // argument parsing which wraps function callbacks in native bindings that
  // become inert after the first invocation.
  var compiledApi = bindingsAPI.compiledApi;

  compiledApi.once = function(event, listener) {
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
  };

  compiledApi.on = function(event, callback) {
      if (eventsMap.hasOwnProperty(event)) {
        compiledApi[eventsMap[event]].addListener(callback);
      }
  };

  compiledApi.removeListener = function(event, callback) {
      if (eventsMap.hasOwnProperty(event)) {
        compiledApi[eventsMap[event]].removeListener(callback);
      }
  };

  compiledApi.removeAllListeners = function(event) {
    if (eventsMap.hasOwnProperty(event)) {
      var listeners = compiledApi[eventsMap[event]].getListeners();
      for (var i = 0; i < listeners.length; i++) {
        compiledApi[eventsMap[event]].removeListener(listeners[i]);
      }
    }
  };
  apiFunctions.setHandleRequest('getDataPath', function() {
    return bindingUtil.sendRequestSync('nw.App.getDataPath', [], undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('getStartPath', function() {
    return nwNatives.getOldCwd();
  });
  bindingsAPI.compiledApi.__defineGetter__('dataPath', function() {
    if (!dataPath)
      dataPath = nw.App.getDataPath();
    return dataPath;
  });
  bindingsAPI.compiledApi.__defineGetter__('startPath', function() {
    return nw.App.getStartPath();
  });
  bindingsAPI.compiledApi.registerGlobalHotKey = function() {
    return nw.Shortcut.registerGlobalHotKey.apply(nw.Shortcut, arguments);
  };
  bindingsAPI.compiledApi.unregisterGlobalHotKey = function() {
    return nw.Shortcut.unregisterGlobalHotKey.apply(nw.Shortcut, arguments);
  };

});

//exports.binding = nw_binding.generate();

