var nwNatives = requireNative('nw_natives');
var forEach = require('utils').forEach;
var runtimeNatives = requireNative('runtime');
var renderFrameObserverNatives = requireNative('renderFrameObserverNatives');
var appWindowNatives = requireNative('app_window_natives');

var GetExtensionViews = runtimeNatives.GetExtensionViews;

var currentNWWindow = null;
var currentNWWindowInternal = null;
var currentRoutingID = nwNatives.getRoutingID();
var currentWidgetRoutingID = nwNatives.getWidgetRoutingID();

var bgPage = GetExtensionViews(-1, -1, 'BACKGROUND')[0];

if (typeof bgPage === 'undefined') //new instance window
  bgPage = window;

if (bgPage == window) {
  window.__nw_initwindow = function (routingId, self) {
    if (!bgPage.__nw_windows)
      bgPage.__nw_windows = {};
    if (routingId in bgPage.__nw_windows) {
      Object.setPrototypeOf(bgPage.__nw_windows[routingId][0], self);
      var eventCBs = bgPage.__nw_windows[routingId][1];
      self.outerEventCBs = eventCBs;
      for (var event in eventCBs) {
        for (var i in eventCBs[event]) {
          if (eventCBs[event][i].once)
            self.once(event, eventCBs[event][i].callback, false);
          else
            self.on(event, eventCBs[event][i].callback, false);
        }
      }
    } else {
      bgPage.__nw_windows[routingId] = [Object.create(self), {}];
      renderFrameObserverNatives.OnDocumentElementCreated(routingId, bgPage.__nw_ondocumentcreated, true);
      renderFrameObserverNatives.OnDestruct(routingId, bgPage.__nw_ondestruct);
    }
    self.outerWindow = bgPage.__nw_windows[routingId][0];
    self.outerEventCBs = bgPage.__nw_windows[routingId][1];
  };
  window.__nw_ondocumentcreated = function (succeed, routingId) {
    if (!succeed)
      return;
    // OnDocumentElementCreated is one-off
    renderFrameObserverNatives.OnDocumentElementCreated(routingId, bgPage.__nw_ondocumentcreated, true);
    if (routingId in window.__nw_windows) {
      var view = appWindowNatives.GetFrame(routingId, false);
      try_nw(view).nw.Window.get();
    }
  };
  window.__nw_ondestruct = function (routingId) {
    delete window.__nw_windows[routingId];
  };
  window.__nw_removeOuterEventCB = function (self, event, listener) {
    if (!(event in self.outerEventCBs))
      return;
    var index = -1;
    for (var i in self.outerEventCBs[event])
      if (self.outerEventCBs[event][i].callback === listener) {
        index = i;
        break;
      }
    if (index > -1)
      $Array.splice(self.outerEventCBs[event], index, 1);
  };
  window.__nw_record_event = function (self, event, listener, is_once) {
    if (!(event in self.outerEventCBs))
      self.outerEventCBs[event] = [];
    $Array.push(self.outerEventCBs[event], {callback: listener, once: is_once});
  };
  window.__nw_remove_all_listeners = function (self, event) {
    if (event in self.outerEventCBs) {
      delete self.outerEventCBs[event];
    }
  };
  window.__nw_windows = {};
}

var try_hidden = function (view) {
  if (view.chrome.app.window)
    return view;
  return privates(view);
};

var try_nw = function (view) {
  if (view.nw)
    return view;
  return privates(view);
};

function getPlatform() {
  var platforms = [
    [/CrOS Touch/, "chromeos touch"],
    [/CrOS/, "chromeos"],
    [/Linux/, "linux"],
    [/Mac/, "mac"],
    [/Win/, "win"],
  ];

  for (var i = 0; i < platforms.length; i++) {
    if ($RegExp.exec(platforms[i][0], navigator.appVersion)) {
      return platforms[i][1];
    }
  }
  return "unknown";
}

var canSetVisibleOnAllWorkspaces = /(mac|linux)/.exec(getPlatform());
var appWinEventsMap = {
  'minimize':         'onMinimized',
  'maximize':         'onMaximized',
  'restore':          'onRestored',
  'enter-fullscreen': 'onFullscreened',
  'closed':           'onClosed',
  'move':             'onMoved',
  'resize':           'onResized'
};

var nwWinEventsMap = {
  'zoom':             'onZoom',
  'close':            'onClose',
  'document-start':   'onDocumentStart',
  'document-end':     'onDocumentEnd'
};

var nwWrapEventsMap = {
  'loaded':           'LoadingStateChanged',
  'new-win-policy':   'onNewWinPolicy',
  'navigation':       'onNavigation'
};

apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('get', function(domWindow) {
    if (domWindow)
      return try_nw(domWindow.top).nw.Window.get();
    if (currentNWWindow)
      return currentNWWindow.outerWindow;

    currentNWWindowInternal = getInternalApi('nw.currentWindowInternal');
    var NWWindow = function() {
      this.appWindow = try_hidden(window).chrome.app.window.current();
      if (!this.appWindow) {
        var winParam = currentNWWindowInternal.getWinParamInternal();
        try_hidden(window).chrome.app.window.initializeAppWindow(winParam);
        this.appWindow = try_hidden(window).chrome.app.window.current();
        if (!this.appWindow)
          console.error('The JavaScript context calling ' +
                        'nw.Window.get() has no associated AppWindow.');
      }
      privates(this).menu = null;
      nwNatives.callInWindow(bgPage, "__nw_initwindow", currentRoutingID, this);
    };
    forEach(currentNWWindowInternal, function(key, value) {
      if (!key.endsWith('Internal'))
        NWWindow.prototype[key] = value;
    });

    NWWindow.prototype.onNewWinPolicy      = bindingUtil.createCustomEvent('nw.Window.onNewWinPolicy', false, false);
    NWWindow.prototype.onNavigation        = bindingUtil.createCustomEvent('nw.Window.onNavigation', false, false);
    NWWindow.prototype.LoadingStateChanged = bindingUtil.createCustomEvent('nw.Window.LoadingStateChanged', false, false);
    NWWindow.prototype.onDocumentStart     = bindingUtil.createCustomEvent('nw.Window.onDocumentStart', false, false);
    NWWindow.prototype.onDocumentEnd       = bindingUtil.createCustomEvent('nw.Window.onDocumentEnd', false, false);
    NWWindow.prototype.onZoom              = bindingUtil.createCustomEvent('nw.Window.onZoom', false, false);
    NWWindow.prototype.onClose             = bindingUtil.createCustomEvent("nw.Window.onClose", true, false);

    NWWindow.prototype.once = function (event, listener, record) {
      if (typeof listener !== 'function')
        throw new TypeError('listener must be a function');
      var fired = false;
      var self = this;

      if (typeof record === 'undefined') {
        nwNatives.callInWindow(bgPage, "__nw_record_event", this, event, listener, true);
      }

      function g() {
        nwNatives.callInWindow(bgPage, "__nw_removeOuterEventCB", self, event, listener);
        self.removeListener(event, g);
        if (!fired) {
          fired = true;
          listener.apply(self, arguments);
        }
      }
      this.on(event, g, false);
      return this;
    };

    NWWindow.prototype.on = function (event, callback, record) {
      var self = this;
      if (typeof record === 'undefined') {
        nwNatives.callInWindow(bgPage, "__nw_record_event", this, event, callback, false);
      }

      // Wrap callback to bind to `self`.
      // If `cb` is given, use `cb` instead of original `callback`.
      function wrap(cb) {
        var fn = (cb || callback).bind(self);
        fn.listener = callback;
        return fn;
      }

      if (event === 'close') {
        this.onClose.addListener(wrap(), {instanceId: currentWidgetRoutingID});
        return this;
      }
      switch (event) {
      case 'focus':
        this.appWindow.contentWindow.onfocus = wrap();
        break;
      case 'blur':
        this.appWindow.contentWindow.onblur = wrap();
        break;
      case 'loaded':
        var g = wrap(function(status) {
          if (status == 'loaded')
            callback.call(self);
        });
        this.LoadingStateChanged.addListener(g);
        break;
      case 'new-win-policy':
        var h = wrap(function(frame, url, policy) {
          policy.ignore         =  function () { this.val = 'ignore'; };
          policy.forceCurrent   =  function () { this.val = 'current'; };
          policy.forceDownload  =  function () { this.val = 'download'; };
          policy.forceNewWindow =  function () { this.val = 'new-window'; };
          policy.forceNewPopup  =  function () { this.val = 'new-popup'; };
          policy.setNewWindowManifest = function (m) { this.manifest = m; };
          callback.call(self, frame, url, policy);
        });
        this.onNewWinPolicy.addListener(h);
        break;
      case 'navigation':
        var j = wrap(function(frame, url, policy, context) {
          policy.ignore         =  function () { this.val = 'ignore'; };
          callback.call(self, frame, url, policy, context);
        });
        this.onNavigation.addListener(j);
        break;
      case 'move':
        var k = wrap(function() {
          callback.call(self, self.x, self.y);
        });
        this.appWindow.onMoved.addListener(k);
        return this; //return early
        break;
      case 'resize':
        var l = wrap(function() {
          callback.call(self, self.width, self.height);
        });
        this.appWindow.onResized.addListener(l);
        return this; //return early
        break;
      }
      if (appWinEventsMap.hasOwnProperty(event)) {
        this.appWindow[appWinEventsMap[event]].addListener(wrap());
        return this;
      }
      if (nwWinEventsMap.hasOwnProperty(event)) {
        this[nwWinEventsMap[event]].addListener(wrap());
        return this;
      }
      return this;
    };
    NWWindow.prototype.removeListener = function (event, callback) {
      nwNatives.callInWindow(bgPage, "__nw_removeOuterEventCB", this, event, callback);
      if (appWinEventsMap.hasOwnProperty(event)) {
        for (let l of this.appWindow[appWinEventsMap[event]].getListeners()) {
          if (l.listener && l.listener === callback) {
            this.appWindow[appWinEventsMap[event]].removeListener(l);
            return this;
          }
        }
      }
      if (nwWinEventsMap.hasOwnProperty(event)) {
        for (let l of this[nwWinEventsMap[event]].getListeners()) {
          if (l.listener && l.listener === callback) {
            this[nwWinEventsMap[event]].removeListener(l);
            return this;
          }
        }
      }
      if (nwWrapEventsMap.hasOwnProperty(event)) {
        for (let l of this[nwWrapEventsMap[event]].getListeners()) {
          if (l.listener && l.listener === callback) {
            this[nwWrapEventsMap[event]].removeListener(l);
            return this;
          }
        }
      }
      switch (event) {
      case 'focus':
        if (this.appWindow.contentWindow.onfocus && this.appWindow.contentWindow.onfocus.listener === callback)
          this.appWindow.contentWindow.onfocus = null;
        break;
      case 'blur':
        if (this.appWindow.contentWindow.onblur && this.appWindow.contentWindow.onblur.listener === callback)
          this.appWindow.contentWindow.onblur = null;
        break;
      }
      return this;
    };

    NWWindow.prototype.removeAllListeners = function (event) {
      if (arguments.length === 0) {
        var obj = Object.assign({}, appWinEventsMap, nwWinEventsMap, nwWrapEventsMap);
        var keys = Object.keys(obj);
        for (var i = 0, key; i < keys.length; ++i) {
          key = keys[i];
          this.removeAllListeners(key);
        }
        return this;
      }
      nwNatives.callInWindow(bgPage, "__nw_remove_all_listeners", this, event);
      if (appWinEventsMap.hasOwnProperty(event)) {
        for (let l of this.appWindow[appWinEventsMap[event]].getListeners()) {
          this.appWindow[appWinEventsMap[event]].removeListener(l);
        }
        return this;
      }
      if (nwWinEventsMap.hasOwnProperty(event)) {
        for (let l of this[nwWinEventsMap[event]].getListeners()) {
          this[nwWinEventsMap[event]].removeListener(l);
        }
        return this;
      }
      if (nwWrapEventsMap.hasOwnProperty(event)) {
        for (let l of this[nwWrapEventsMap[event]].getListeners()) {
          this[nwWrapEventsMap[event]].removeListener(l);
        }
        return this;
      }
      switch (event) {
      case 'focus':
        this.appWindow.contentWindow.onfocus = null;
        break;
      case 'blur':
        this.appWindow.contentWindow.onblur = null;
        break;
      }
      return this;
    };

    NWWindow.prototype.setShadow = function(shadow) {
      currentNWWindowInternal.setShadowInternal(shadow);
    };

    NWWindow.prototype.setShowInTaskbar = function(show) {
      currentNWWindowInternal.setShowInTaskbarInternal(show);
    };

    NWWindow.prototype.setProgressBar = function(progress) {
      currentNWWindowInternal.setProgressBarInternal(progress);
    };

    NWWindow.prototype.setBadgeLabel = function(label) {
      currentNWWindowInternal.setBadgeLabelInternal(label);
    };

    NWWindow.prototype.enterKioskMode = function() {
      currentNWWindowInternal.enterKioskModeInternal();
    };

    NWWindow.prototype.leaveKioskMode = function() {
      currentNWWindowInternal.leaveKioskModeInternal();
    };

    NWWindow.prototype.toggleKioskMode = function() {
      currentNWWindowInternal.toggleKioskModeInternal();
    };

    NWWindow.prototype.showDevTools = function(frm, callback) {
      var id = '';
      if (typeof frm === 'string')
        id = frm;
      var f = null;
      if (id)
        f = this.appWindow.contentWindow.getElementById(id);
      else
        f = frm || null;
      nwNatives.setDevToolsJail(f);
      currentNWWindowInternal.showDevToolsInternal(callback);
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
      currentNWWindowInternal.capturePageInternal(0, options, cb);
    };
    NWWindow.prototype.reload = function () {
      this.appWindow.contentWindow.location.reload();
    };
    NWWindow.prototype.reloadIgnoringCache = function () {
      currentNWWindowInternal.reloadIgnoringCache();
    };
    NWWindow.prototype.eval = function (frame, script) {
      return nwNatives.evalScript(frame, script);
    };
    NWWindow.prototype.evalNWBin = function (frame, path) {
      this.evalNWBinInternal(frame, path);
    };
    NWWindow.prototype.evalNWBinModule = function (frame, path, module_path) {
      this.evalNWBinInternal(frame, path, module_path);
    };
    NWWindow.prototype.evalNWBinInternal = function (frame, path, module_path) {
      var ab;
      if (Buffer.isBuffer(path)) {
        let buf = path;
        ab = new global.ArrayBuffer(path.length);
        path.copy(Buffer.from(ab));
      } else if ($Object.prototype.toString.apply(path) === '[object ArrayBuffer]') {
        ab = path;
      } else {
        let buf = global.require('fs').readFileSync(path);
        ab = new global.ArrayBuffer(buf.length);
        buf.copy(Buffer.from(ab));
      }
      if (module_path)
        return nwNatives.evalNWBin(frame, ab, module_path);
      return nwNatives.evalNWBin(frame, ab);
    };
    NWWindow.prototype.show = function () {
      this.appWindow.show();
    };
    NWWindow.prototype.hide = function () {
      this.appWindow.hide();
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
    NWWindow.prototype.setAlwaysOnTop = function (top) {
      this.appWindow.setAlwaysOnTop(top);
    };
    NWWindow.prototype.setPosition = function (pos) {
      if (pos == "center") {
        var screenWidth = screen.availWidth;
        var screenHeight = screen.availHeight;
        var width  = this.appWindow.outerBounds.width;
        var height = this.appWindow.outerBounds.height;
        this.appWindow.outerBounds.setPosition(Math.round((screenWidth-width)/2),
                                               Math.round((screenHeight-height)/2));
      }
    };
    NWWindow.prototype.setVisibleOnAllWorkspaces = function(all_visible) {
      this.appWindow.setVisibleOnAllWorkspaces(all_visible);
    };
    NWWindow.prototype.canSetVisibleOnAllWorkspaces = function() {
      return canSetVisibleOnAllWorkspaces;
    };
    NWWindow.prototype.setMaximumSize = function (width, height) {
      this.appWindow.outerBounds.maxWidth = width;
      this.appWindow.outerBounds.maxHeight = height;
    };
    NWWindow.prototype.setMinimumSize = function (width, height) {
      this.appWindow.outerBounds.minWidth = width;
      this.appWindow.outerBounds.minHeight = height;
    };
    NWWindow.prototype.resizeTo = function (width, height) {
      this.appWindow.outerBounds.width = width;
      this.appWindow.outerBounds.height = height;
    };
    NWWindow.prototype.resizeBy = function (width, height) {
      this.appWindow.outerBounds.width += width;
      this.appWindow.outerBounds.height += height;
    };
    NWWindow.prototype.moveTo = function (x, y) {
      this.appWindow.outerBounds.left = x;
      this.appWindow.outerBounds.top = y;
    };
    NWWindow.prototype.moveBy = function (x, y) {
      this.appWindow.outerBounds.left += x;
      this.appWindow.outerBounds.top += y;
    };
    NWWindow.prototype.setResizable = function (resizable) {
      this.appWindow.setResizable(resizable);
    };
    NWWindow.prototype.requestAttention = function (flash) {
      if (typeof flash == 'boolean')
        flash = flash ? -1 : 0;
      currentNWWindowInternal.requestAttentionInternal(flash);
    };
    NWWindow.prototype.cookies = chrome.cookies;

    NWWindow.prototype.print = function(option) {
      var _option = JSON.parse(JSON.stringify(option));
      if (!("autoprint" in _option))
        _option["autoprint"] = true;
      if (option.pdf_path)
        _option["printer"] = "Save as PDF";
      currentNWWindowInternal.setPrintSettingsInternal(_option);
      window.print();
      // autoprint will be set to false in print_preview_handler.cc after printing is done
      // window.print will return immediately for PDF window #5002
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
        return currentNWWindowInternal.getTitleInternal();
      },
      set: function(val) {
        currentNWWindowInternal.setTitleInternal(val);
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
    Object.defineProperty(NWWindow.prototype, 'isTransparent', {
      get: function() {
        return this.appWindow.alphaEnabled();
      }
    });
    Object.defineProperty(NWWindow.prototype, 'isAACActive', {
      get: function() {
        return currentNWWindowInternal.isAACActiveInternal();
      }
    });
    Object.defineProperty(NWWindow.prototype, 'isKioskMode', {
      get: function() {
        return currentNWWindowInternal.isKioskInternal();
      },
      set: function(val) {
        if (val)
          currentNWWindowInternal.enterKioskMode();
        else
          currentNWWindowInternal.leaveKioskMode();
      }
    });
    Object.defineProperty(NWWindow.prototype, 'isFullscreen', {
      get: function() {
        return this.appWindow.isFullscreen();
      }
    });
    Object.defineProperty(NWWindow.prototype, 'isAlwaysOnTop', {
      get: function() {
        return this.appWindow.isAlwaysOnTop();
      }
    });
    Object.defineProperty(NWWindow.prototype, 'menu', {
      get: function() {
        var ret = privates(this).menu || {};
        return ret;
      },
      set: function(menu) {
        if(!menu) {
          privates(this).menu = null;
          currentNWWindowInternal.clearMenu();
          return;
        }
        if (menu.type != 'menubar')
          throw new TypeError('Only menu of type "menubar" can be used as this.window menu');

        privates(this).menu =  menu;
        var menuPatch = currentNWWindowInternal.setMenu(menu.id);
        if (menuPatch.length) {
          menuPatch.forEach((patch)=>{
            let menuIndex = patch.menu;
            let itemIndex = patch.index;
            let menuToPatch = menu.items[menuIndex];
            if (menuToPatch && menuToPatch.submenu) {
              menuToPatch.submenu.insert(new nw.MenuItem(patch.option), itemIndex);
            }
          });
        }
      }
    });
    Object.defineProperty(NWWindow.prototype, 'window', {
      get: function() {
        return this.appWindow.contentWindow;
      }
    });
    Object.defineProperty(NWWindow.prototype, 'frameId', {
      get: function() {
        return currentRoutingID;
      }
    });
    currentNWWindow = new NWWindow;
    return currentNWWindow.outerWindow;
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
      if (params.focus === false)
        options.focused = false;
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
      if (params.show_in_taskbar === false)
        options.show_in_taskbar = false;
      if (params['always_on_top'] === true)
        options.alwaysOnTop = true;
      if (params['visible_on_all_workspaces'] === true)
        options.visibleOnAllWorkspaces = true;
      if (typeof params['inject_js_start'] == 'string')
        options.inject_js_start = params['inject_js_start'];
      if (typeof params['inject_js_end'] == 'string')
        options.inject_js_end = params['inject_js_end'];
      if (params.transparent)
        options.alphaEnabled = true;
      if (params.kiosk === true)
        options.kiosk = true;
      if (params.new_instance === true)
        options.new_instance = true;
      if (params.position)
        options.position = params.position;
      if (params.title)
        options.title = params.title;
      if (params.icon)
        options.icon = params.icon;
      if (params.id)
        options.id = params.id;
    }
    try_hidden(window).chrome.app.window.create(url, options, function(appWin) {
      if (callback) {
        if (appWin)
          callback(try_nw(appWin.contentWindow).nw.Window.get());
        else
          callback();
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

function onClose(user_force) {
  if (!currentNWWindow)
    return;
  currentNWWindow.onClose.dispatchNW({instanceId: currentWidgetRoutingID}, user_force);
}

function get_nw() {
  try {
    //appWindowNatives.FixGamePadAPI();
    var nw0 = try_nw(window).nw;
    if (nw0)
      nw0.Window.get();
  } catch (e) {
  }
}

if (bgPage !== window) {
  renderFrameObserverNatives.OnDocumentElementCreated(currentRoutingID, get_nw);
}

exports.onNewWinPolicy = onNewWinPolicy;
exports.onNavigation = onNavigation;
exports.LoadingStateChanged = onLoadingStateChanged;
exports.onDocumentStartEnd = onDocumentStartEnd;
exports.onClose = onClose;
exports.updateAppWindowZoom = updateAppWindowZoom;
