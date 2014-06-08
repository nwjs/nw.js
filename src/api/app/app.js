// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

var argv, fullArgv, dataPath, manifest;

function App() {
}
require('util').inherits(App, exports.Base);

App.filteredArgv = [
  /--no-toolbar/,
  /--url=.*/,
  /--remote-debugging-port=.*/,
  /--renderer-cmd-prefix.*/,
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

App.prototype.clearCache = function() {
  nw.callStaticMethodSync('App', 'ClearCache', [ ]);
}

App.prototype.getProxyForURL = function (url) {
  return nw.callStaticMethodSync('App', 'getProxyForURL', [ url ]);
}

App.prototype.addOriginAccessWhitelistEntry = function(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains) {
    return nw.callStaticMethodSync('App', 'AddOriginAccessWhitelistEntry', sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains);
}

App.prototype.removeOriginAccessWhitelistEntry = function(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains) {
    return nw.callStaticMethodSync('App', 'RemoveOriginAccessWhitelistEntry', sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains);
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
