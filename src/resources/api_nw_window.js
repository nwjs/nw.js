var Binding = require('binding').Binding;
var nw_binding = require('binding').Binding.create('nw.Window');
var nwNatives = requireNative('nw_natives');
var forEach = require('utils').forEach;
var Event = require('event_bindings').Event;

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
      this.appWindow = chrome.app.window.current();
      privates(this).menu = null;
    };
    forEach(currentNWWindowInternal, function(key, value) {
      NWWindow.prototype[key] = value;
    });

    NWWindow.prototype.onNewWinPolicy      = new Event();
    NWWindow.prototype.onNavigation        = new Event();
    NWWindow.prototype.LoadingStateChanged = new Event();

    NWWindow.prototype.on = function (event, callback) {
      switch (event) {
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
    NWWindow.prototype.eval = function (frame, script) {
      nwNatives.evalScript(frame, script);
    };
    NWWindow.prototype.evalNWBin = function (frame, path) {
      nwNatives.evalScript(frame, path);
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
    };
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

exports.binding = nw_binding.generate();
exports.onNewWinPolicy = onNewWinPolicy;
exports.onNavigation = onNavigation;
exports.LoadingStateChanged = onLoadingStateChanged;
