var Binding = require('binding').Binding;
var nw_binding = require('binding').Binding.create('nw.Window');
var nwNatives = requireNative('nw_natives');
var forEach = require('utils').forEach;
var Event = require('event_bindings').Event;
var sendRequest = require('sendRequest');

var currentNWWindow = null;
var currentNWWindowInternal = null;

var nw_internal = require('binding').Binding.create('nw.currentWindowInternal');

nw_internal.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('getZoom', function() {
    return sendRequest.sendRequestSync('nw.currentWindowInternal.getZoom', arguments, this.definition.parameters, {})[0];
  });
  apiFunctions.setHandleRequest('setZoom', function() {
    return sendRequest.sendRequestSync('nw.currentWindowInternal.setZoom', arguments, this.definition.parameters, {});
  });
});

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('get', function() {
    if (currentNWWindow)
      return currentNWWindow;

    currentNWWindowInternal = nw_internal.generate();
    var NWWindow = function() {
      this.appWindow = chrome.app.window.current();
      privates(this).menu = null;
    };
    forEach(currentNWWindowInternal, function(key, value) {
      NWWindow.prototype[key] = value;
    });

    NWWindow.prototype.onNewWinPolicy      = new Event();
    NWWindow.prototype.onNavigation        = new Event();
    NWWindow.prototype.LoadingStateChanged = new Event();
    NWWindow.prototype.onDocumentStart     = new Event();
    NWWindow.prototype.onDocumentEnd       = new Event();
    NWWindow.prototype.onZoom              = new Event();

    NWWindow.prototype.on = function (event, callback) {
      switch (event) {
      case 'focus':
        this.appWindow.contentWindow.onfocus = callback;
        break;
      case 'blur':
        this.appWindow.contentWindow.onblur = callback;
        break;
      case 'minimize':
        this.appWindow.onMinimized.addListener(callback);
        break;
      case 'maximize':
        this.appWindow.onMaximized.addListener(callback);
        break;
      case 'restore':
        this.appWindow.onRestored.addListener(callback);
        break;
      case 'resize':
        this.appWindow.onResized.addListener(callback);
        break;
      case 'move':
        this.appWindow.onMoved.addListener(callback);
        break;
      case 'enter-fullscreen':
        this.appWindow.onFullscreened.addListener(callback);
        break;
      case 'zoom':
        this.onZoom.addListener(callback);
        break;
      case 'closed':
        this.appWindow.onClosed.addListener(callback);
        break;
      case 'loaded':
        this.LoadingStateChanged.addListener(function(status) { if (status == 'loaded') callback(); });
        break;
      case 'new-win-policy':
        this.onNewWinPolicy.addListener(function(frame, url, policy) {
          policy.ignore         =  function () { this.val = 'ignore'; };
          policy.forceCurrent   =  function () { this.val = 'current'; };
          policy.forceDownload  =  function () { this.val = 'download'; };
          policy.forceNewWindow =  function () { this.val = 'new-window'; };
          policy.forceNewPopup  =  function () { this.val = 'new-popup'; };
          policy.setNewWindowManifest = function (m) { this.manifest = JSON.stringify(m); };
          callback(frame, url, policy);
        });
        break;
      case 'navigation':
        this.onNavigation.addListener(function(frame, url, policy, context) {
          policy.ignore         =  function () { this.val = 'ignore'; };
          callback(frame, url, policy, context);
        });
        break;
      case 'document-start':
        this.onDocumentStart.addListener(callback);
        break;
      case 'document-end':
        this.onDocumentEnd.addListener(callback);
        break;
      }
    };
    NWWindow.prototype.capturePage = function (callback, options) {
      var cb = callback;
      if (!options)
        options = {'format':'jpeg', 'datatype':'datauri'};
      if (typeof options == 'string')
        options = {'format':options, 'datatype':'datauri'};
      if (options.datatype != 'datauri') {
        cb = function (format, datauri) {
          var raw = datauri.replace(/^data:[^;]*;base64,/, '');
          switch(format){
          case 'buffer' :
            callback(new nw.Buffer(raw, "base64"));
            break;
          case 'raw' :
            callback(raw);
            break;
          }
        };
        cb = cb.bind(undefined, options.datatype);
      }
      currentNWWindowInternal.capturePageInternal(options, cb);
    };
    NWWindow.prototype.reload = function () {
      this.appWindow.contentWindow.location.reload();
    };
    NWWindow.prototype.reloadIgnoringCache = function () {
      currentNWWindowInternal.reloadIgnoringCache();
    };
    NWWindow.prototype.eval = function (frame, script) {
      nwNatives.evalScript(frame, script);
    };
    NWWindow.prototype.evalNWBin = function (frame, path) {
      nwNatives.evalNWBin(frame, path);
    };
    NWWindow.prototype.show = function () {
      this.appWindow.show();
    };
    NWWindow.prototype.hide = function () {
      this.appWindow.hide();
    };
    NWWindow.prototype.close = function () {
      this.appWindow.close();
    };
    NWWindow.prototype.focus = function () {
      this.appWindow.focus();
    };
    NWWindow.prototype.blur = function () {
      this.appWindow.contentWindow.blur();
    };
    NWWindow.prototype.minimize = function () {
      this.appWindow.minimize();
    };
    NWWindow.prototype.maximize = function () {
      this.appWindow.maximize();
    };
    NWWindow.prototype.unmaximize = NWWindow.prototype.restore = function () {
      this.appWindow.restore();
    };
    NWWindow.prototype.enterFullscreen = function () {
      this.appWindow.fullscreen();
    };
    NWWindow.prototype.leaveFullscreen = function () {
      if (this.appWindow.isFullscreen())
        this.appWindow.restore();
    };
    NWWindow.prototype.toggleFullscreen = function () {
      if (this.appWindow.isFullscreen())
        this.appWindow.restore();
      else
        this.appWindow.fullscreen();
    };
    NWWindow.prototype.setMaximumSize = function (width, height) {
      this.appWindow.outerBounds.maxWidth = width;
      this.appWindow.outerBounds.maxHeight = height;
    };
    NWWindow.prototype.setMinimumSize = function (width, height) {
      this.appWindow.outerBounds.minWidth = width;
      this.appWindow.outerBounds.minHeight = height;
    };
    NWWindow.prototype.setResizable = function (resizable) {
      this.appWindow.setResizable(resizable);
    };
    NWWindow.prototype.cookies = chrome.cookies;

    Object.defineProperty(NWWindow.prototype, 'x', {
      get: function() {
        return this.appWindow.outerBounds.left;
      },
      set: function(x) {
        this.appWindow.outerBounds.left = x;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'y', {
      get: function() {
        return this.appWindow.outerBounds.top;
      },
      set: function(y) {
        this.appWindow.outerBounds.top = y;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'width', {
      get: function() {
        return this.appWindow.innerBounds.width;
      },
      set: function(val) {
        this.appWindow.innerBounds.width = val;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'height', {
      get: function() {
        return this.appWindow.innerBounds.height;
      },
      set: function(val) {
        this.appWindow.innerBounds.height = val;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'title', {
      get: function() {
        return this.appWindow.contentWindow.document.title;
      },
      set: function(val) {
        this.appWindow.contentWindow.document.title = val;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'zoomLevel', {
      get: function() {
        return currentNWWindowInternal.getZoom();
      },
      set: function(val) {
        currentNWWindowInternal.setZoom(val);
      }
    });
    Object.defineProperty(NWWindow.prototype, 'menu', {
      get: function() {
        var ret = privates(this).menu || {};
        return ret;
      },
      set: function(menu) {
        if(!menu) {
          currentNWWindowInternal.clearMenu();
          return;
        }
        if (menu.type != 'menubar')
          throw new TypeError('Only menu of type "menubar" can be used as this.window menu');

        privates(this).menu =  menu;
        currentNWWindowInternal.setMenu(menu.id);
      }
    });
    Object.defineProperty(NWWindow.prototype, 'window', {
      get: function() {
        return this.appWindow.contentWindow;
      }
    });
    currentNWWindow = new NWWindow;
    return currentNWWindow;
  });

  apiFunctions.setHandleRequest('open', function(url, params, callback) {
    var options = {};
    //FIXME: unify this conversion code with nwjs/default.js
    options.innerBounds = {};
    options.outerBounds = {};
    if (params) {
      if (params.frame === false)
        options.frame = 'none';
      if (params.resizable === false)
        options.resizable = false;
      if (params.x)
        options.outerBounds.left = params.x;
      if (params.y)
        options.outerBounds.top = params.y;
      if (params.height)
        options.innerBounds.height = params.height;
      if (params.width)
        options.innerBounds.width = params.width;
      if (params.min_width)
        options.innerBounds.minWidth = params.min_width;
      if (params.max_width)
        options.innerBounds.maxWidth = params.max_width;
      if (params.min_height)
        options.innerBounds.minHeight = params.min_height;
      if (params.max_height)
        options.innerBounds.maxHeight = params.max_height;
      if (params.fullscreen === true)
        options.state = 'fullscreen';
      if (params.show === false)
        options.hidden = true;
      if (params['always_on_top'] === true)
        options.alwaysOnTop = true;
      if (params['visible_on_all_workspaces'] === true)
        options.visibleOnAllWorkspaces = true;
    }
    chrome.app.window.create(url, options, function(appWin) {
      if (callback) {
        callback(appWin.contentWindow.nw.Window.get());
      }
    });
  });

});

function dispatchEventIfExists(target, name, varargs) {
  // Sometimes apps like to put their own properties on the window which
  // break our assumptions.
  var event = target[name];
  if (event && (typeof event.dispatch == 'function'))
    $Function.apply(event.dispatch, event, varargs);
  else
    console.warn('Could not dispatch ' + name + ', event has been clobbered');
}

function onNewWinPolicy(frame, url, policy) {
  //console.log("onNewWinPolicy called: " + url + ", " + policy);
  if (!currentNWWindow)
    return;
  dispatchEventIfExists(currentNWWindow, "onNewWinPolicy", [frame, url, policy]);
}

function onNavigation(frame, url, policy, context) {
  //console.log("onNavigation called: " + url + ", " + context);
  if (!currentNWWindow)
    return;
  dispatchEventIfExists(currentNWWindow, "onNavigation", [frame, url, policy, context]);
}

function onLoadingStateChanged(status) {
  //console.log("onLoadingStateChanged: " + status);
  if (!currentNWWindow)
    return;
  dispatchEventIfExists(currentNWWindow, "LoadingStateChanged", [status]);
}

function onDocumentStartEnd(start, frame) {
  if (!currentNWWindow)
    return;
  if (start)
    dispatchEventIfExists(currentNWWindow, "onDocumentStart", [frame]);
  else
    dispatchEventIfExists(currentNWWindow, "onDocumentEnd", [frame]);
}

function updateAppWindowZoom(old_level, new_level) {
  if (!currentNWWindow)
    return;
  dispatchEventIfExists(currentNWWindow, "onZoom", [new_level]);
}

exports.binding = nw_binding.generate();
exports.onNewWinPolicy = onNewWinPolicy;
exports.onNavigation = onNavigation;
exports.LoadingStateChanged = onLoadingStateChanged;
exports.onDocumentStartEnd = onDocumentStartEnd;
exports.updateAppWindowZoom = updateAppWindowZoom;
