// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//

var argv, fullArgv, dataPath, manifest;
var v8_util = process.binding('v8_util');

function App() {
}
require('util').inherits(App, exports.Base);

App.filteredArgv = [
  /^--no-toolbar$/,
  /^--url=/,
  /^--remote-debugging-port=/,
  /^--renderer-cmd-prefix/,
];

App.prototype.quit = function() {
  nw.callStaticMethod('App', 'Quit', [ ]);
}

App.prototype.closeAllWindows = function() {
  nw.callStaticMethod('App', 'CloseAllWindows', [ ]);
}

App.prototype.crashBrowser = function() {
  nw.callStaticMethod('App', 'CrashBrowser', [ ]);
}

App.prototype.crashRenderer = function() {
  nw.crashRenderer();
}

App.prototype.setCrashDumpDir = function(dir) {
  nw.setCrashDumpDir(dir); // for windows renderer process
  return nw.callStaticMethodSync('App', 'SetCrashDumpDir', [ dir ]);
}

App.prototype.createShortcut = function(dir) {
  return nw.callStaticMethodSync('App', 'CreateShortcut', [ dir ]);
}

App.prototype.clearCache = function() {
  nw.callStaticMethodSync('App', 'ClearCache', [ ]);
}

App.prototype.doneMenuShow = function() {
  nw.callStaticMethodSync('App', 'DoneMenuShow', [ ]);
}

App.prototype.getProxyForURL = function (url) {
  return nw.callStaticMethodSync('App', 'getProxyForURL', [ url ]);
}

App.prototype.setProxyConfig = function (proxy_config) {
  return nw.callStaticMethodSync('App', 'SetProxyConfig', [ proxy_config ]);
}

App.prototype.addOriginAccessWhitelistEntry = function(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains) {
    return nw.callStaticMethodSync('App', 'AddOriginAccessWhitelistEntry', sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains);
}

App.prototype.removeOriginAccessWhitelistEntry = function(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains) {
    return nw.callStaticMethodSync('App', 'RemoveOriginAccessWhitelistEntry', sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains);
}

App.prototype.registerGlobalHotKey = function(shortcut) {
  if (v8_util.getConstructorName(shortcut) != "Shortcut")
    throw new TypeError("Invaild parameter, need Shortcut object.");

  return nw.callStaticMethodSync('App',
                                 'RegisterGlobalHotKey',
                                 [ shortcut.id ])[0];
}

App.prototype.unregisterGlobalHotKey = function(shortcut) {
  if (v8_util.getConstructorName(shortcut) != "Shortcut")
    throw new TypeError("Invaild parameter, need Shortcut object.");

  nw.callStaticMethodSync('App', 'UnregisterGlobalHotKey', [ shortcut.id ]);
}

App.prototype.__defineGetter__('argv', function() {
  if (!argv) {
    var fullArgv = this.fullArgv;
    argv = [];
    for (var i = 0; i < fullArgv.length; ++i) {
      var matched = false;
      for (var j = 0; j < App.filteredArgv.length; ++j) {
        if (App.filteredArgv[j].test(fullArgv[i])) {
          matched = true;
          break;;
        }
      }
      if (matched)
        continue;

      argv.push(fullArgv[i]);
    }
  }

  return argv;
});

App.prototype.__defineGetter__('fullArgv', function() {
  if (!fullArgv)
    fullArgv = nw.callStaticMethodSync('App', 'GetArgv', [ ]);

  return fullArgv;
});

App.prototype.__defineGetter__('dataPath', function() {
  if (!dataPath)
    dataPath = nw.callStaticMethodSync('App', 'GetDataPath', [ ])[0];

  return dataPath;
});

App.prototype.__defineGetter__('manifest', function() {
  if (!manifest) {
    manifest = JSON.parse(
        nw.callStaticMethodSync('App', 'GetPackage', [ ])[0]);
  }
  return manifest;
});

// Store App object in node's context.
if (process['_nw_app']) {
  exports.App = process['_nw_app'];
} else {
  exports.App = process['_nw_app'] = new App();
}
