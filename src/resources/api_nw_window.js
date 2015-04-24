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
      this.appWindow = chrome.app.window.current();
    };
    forEach(currentNWWindowInternal, function(key, value) {
      NWWindow.prototype[key] = value;
    });

    NWWindow.prototype.on = function (event, callback) {
      switch (event) {
      case 'closed':
        this.appWindow.onClosed.addListener(callback);
        break;
      }
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
    chrome.app.window.create(url, options, function(appWin) {
      callback(appWin.contentWindow.nw.Window.get());
    });
  });

});

exports.binding = nw_binding.generate();

