// Copyright (c) 2016 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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

var AutoUpdater = null;
var EventEmitter = nw.require('events').EventEmitter;

if (process.platform === 'win32') {

  const fs = nw.require('fs')
  const path = nw.require('path')
  const spawn = nw.require('child_process').spawn

  // i.e. my-app/app-0.1.13/
  const appFolder = path.dirname(process.execPath)

  // i.e. my-app/Update.exe
  const updateExe = path.resolve(appFolder, '..', 'Update.exe')
  const exeName = path.basename(process.execPath)
  var spawnedArgs = []
  var spawnedProcess

  var isSameArgs = function (args) {
    return (args.length === spawnedArgs.length) && args.every(function (e, i) {
      return e === spawnedArgs[i]
    })
  }

  // Spawn a command and invoke the callback when it completes with an error
  // and the output from standard out.
  var spawnUpdate = function (args, detached, callback) {
    var error, errorEmitted, stderr, stdout

    try {
      // Ensure we don't spawn multiple squirrel processes
      // Process spawned, same args:        Attach events to alread running process
      // Process spawned, different args:   Return with error
      // No process spawned:                Spawn new process
      if (spawnedProcess && !isSameArgs(args)) {
        return callback('AutoUpdater process with arguments ' + args + ' is already running')
      } else if (!spawnedProcess) {
        spawnedProcess = spawn(updateExe, args, {
          detached: detached
        })
        spawnedArgs = args || []
      }
    } catch (error1) {
      error = error1

      // Shouldn't happen, but still guard it.
      process.nextTick(function () {
        return callback(error)
      })
      return
    }
    stdout = ''
    stderr = ''
    spawnedProcess.stdout.on('data', function (data) {
      stdout += data
    })
    spawnedProcess.stderr.on('data', function (data) {
      stderr += data
    })
    errorEmitted = false
    spawnedProcess.on('error', function (error) {
      errorEmitted = true
      callback(error)
    })
    return spawnedProcess.on('exit', function (code, signal) {
      spawnedProcess = undefined
      spawnedArgs = []

      // We may have already emitted an error.
      if (errorEmitted) {
        return
      }

      // Process terminated with error.
      if (code !== 0) {
        return callback('Command failed: ' + (signal != null ? signal : code) + '\n' + stderr)
      }

      // Success.
      callback(null, stdout)
    })
  }

  function Squirrel() {
  }
  // Start an instance of the installed app.
  Squirrel.prototype.processStart = function () {
    return spawnUpdate(['--processStartAndWait', exeName], true, function () {})
  }

  // Download the releases specified by the URL and write new results to stdout.
  Squirrel.prototype.download = function (updateURL, callback) {
    return spawnUpdate(['--download', updateURL], false, function (error, stdout) {
      var json, ref, ref1, update
      if (error != null) {
        return callback(error)
      }
      try {
        // Last line of output is the JSON details about the releases
        json = stdout.trim().split('\n').pop()
        update = (ref = JSON.parse(json)) != null ? (ref1 = ref.releasesToApply) != null ? typeof ref1.pop === 'function' ? ref1.pop() : void 0 : void 0 : void 0
      } catch (jsonError) {
        return callback('Invalid result:\n' + stdout)
      }
      return callback(null, update)
    })
  }

  // Update the application to the latest remote version specified by URL.
  Squirrel.prototype.update = function (updateURL, callback) {
    return spawnUpdate(['--update', updateURL], false, callback)
  }

  // Is the Update.exe installed with the current application?
  Squirrel.prototype.supported = function () {
    try {
      fs.accessSync(updateExe, fs.R_OK)
      return true
    } catch (error) {
      return false
    }
  }
  const squirrelUpdate = new Squirrel();

  // === AutoUpdaterWin ===

  var AutoUpdaterWin = new EventEmitter();

  AutoUpdaterWin.GetFeedURL = function() {
    return this.updateURL;
  }

  AutoUpdaterWin.SetFeedURL = function(url) {
    this.updateURL = url;
  }

  AutoUpdaterWin.emitError = function(message) {
    this.emit('error', message);
  }

  AutoUpdaterWin.QuitAndInstall = function() {
    if (!this.updateAvailable) {
      return this.emitError('No update available, can\'t quit and install');
    }
    squirrelUpdate.processStart();
    nw.App.quit();
  }

  AutoUpdaterWin.CheckForUpdates = function() {
    if (!this.updateURL) {
      return this.emitError('Update URL is not set')
    }
    if (!squirrelUpdate.supported()) {
      return this.emitError('Can not find Squirrel')
    }
    this.emit('checking-for-update')
    squirrelUpdate.download(this.updateURL, function(error, update) {
      if (error != null) {
        return AutoUpdaterWin.emitError(error)
      }
      if (update == null) {
        return AutoUpdaterWin.emit('update-not-available')
      }
      AutoUpdaterWin.updateAvailable = true
      AutoUpdaterWin.emit('update-available')
      squirrelUpdate.update(AutoUpdaterWin.updateURL, function(error) {
        if (error != null) {
          return AutoUpdaterWin.emitError(error)
        }
        const releaseNotes = update
        // Date is not available on Windows, so fake it.
        const date = new Date()
        AutoUpdaterWin.emit('update-downloaded', releaseNotes, date, AutoUpdaterWin.updateURL)
      })
	})
  }

  AutoUpdater = AutoUpdaterWin;

} else {
  var nw_binding = require('binding').Binding.create('nw.AutoUpdater');
  var sendRequest = require('sendRequest');

  var events = {
    onError: 'error',
    onCheckingForUpdate: 'checking-for-update',
    onUpdateAvailable: 'update-available',
    onUpdateNotAvailable: 'update-not-available',
    onUpdateDownloaded: 'update-downloaded'
  };

  // Hook Sync API calls
  nw_binding.registerCustomHook(function(bindingsAPI) {
    var apiFunctions = bindingsAPI.apiFunctions;
    ['NativeCallSync'].forEach(function(nwSyncAPIName) {
      apiFunctions.setHandleRequest(nwSyncAPIName, function() {
        return sendRequest.sendRequestSync(this.name, arguments, this.definition.parameters, {});
      });
    });
  });

  var nwAutoUpdaterBinding = nw_binding.generate();

  var AutoUpdaterNative = new EventEmitter();

  AutoUpdaterNative.SetFeedURL = function (url) {
    nwAutoUpdaterBinding.NativeCallSync("SetFeedURL", url);
  }

  AutoUpdaterNative.GetFeedURL = function () {
    return nwAutoUpdaterBinding.NativeCallSync("GetFeedURL", "")[0];
  }

  AutoUpdaterNative.CheckForUpdates = function () {
    nwAutoUpdaterBinding.NativeCallSync("CheckForUpdates", "");
  }

  AutoUpdaterNative.QuitAndInstall = function () {
    nwAutoUpdaterBinding.NativeCallSync("QuitAndInstall", "");
  }

  Object.keys(events).forEach(function(eventName) {
    nwAutoUpdaterBinding[eventName].addListener(function() {
      var args = Array.prototype.concat.apply([events[eventName]], arguments);
      if(eventName == 'onUpdateDownloaded') {
        args[3] = Date(args[3]);
      }
      AutoUpdaterNative.emit.apply(AutoUpdaterNative, args);
    });
  });

  AutoUpdater = AutoUpdaterNative;
}

AutoUpdater.Init = function() {
  //do nothing, compatibility with old api
}

exports.binding = AutoUpdater;
