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

var try_hidden = function (view) {
  if (view.chrome.windows)
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

var nwWinEventsMap = {
  'minimize':         'onMinimized',
  'maximize':         'onMaximized',
  'restore':          'onRestore',
  'enter-fullscreen': 'onFullscreen',
  'zoom':             'onZoom',
  'resize':           'onResized'
};

var nwWrapEventsMap = {
  'loaded':           'LoadingStateChanged',
  'new-win-policy':   'onNewWinPolicy',
  'navigation':       'onNavigation'
};

var wrapEventsMapNewWin = {
  'move':    'onMove',
  'focus':   'onFocusChanged',
  'blur':    'onFocusChanged',
  'closed':  'onRemoved',
  'close':   'onRemoving'
};

function NWWindow(cWindow) {
  var self = this;
  if (cWindow)
    this.cWindow = cWindow;
  else {
    this.cWindow = currentNWWindowInternal.getCurrent(-2, {'populate': true});
    if (!this.cWindow)
          console.error('The JavaScript context calling ' +
                        'nw.Window.get() has no associated Browser window.');
  }

  function updateWindowAttributes(w) {
    if (w.id !== self.cWindow.id)
      return;
    var oldState = self.cWindow.state;
    var oldWidth = self.cWindow.width;
    var oldHeight = self.cWindow.height;

    self.cWindow.state = w.state;
    self.cWindow.width = w.width;
    self.cWindow.height = w.height;

    if (oldState != 'minimized' && w.state == 'minimized') {
      dispatchEventIfExists(self, 'onMinimized');
    } else if (oldState != 'maximized' && w.state == 'maximized') {
      dispatchEventIfExists(self, 'onMaximized');
    } else if (oldState != 'fullscreen' && w.state == 'fullscreen') {
      dispatchEventIfExists(self, 'onFullscreen');
    } else if (oldState != 'normal' && w.state == 'normal') {
      dispatchEventIfExists(self, 'onRestore');
    } else if (oldWidth != w.width || oldHeight != w.height) {
      dispatchEventIfExists(self, 'onResized', [w.width, w.height]);
    }
  }
  privates(this).menu = null;
  chrome.windows.onWindowChanged.addListener(updateWindowAttributes);
}

NWWindow.prototype.onNewWinPolicy      = bindingUtil.createCustomEvent("nw.Window.onNewWinPolicy", false, false);
NWWindow.prototype.onNavigation        = bindingUtil.createCustomEvent("nw.Window.onNavigation",   false, false);
NWWindow.prototype.LoadingStateChanged = bindingUtil.createCustomEvent("nw.Window.LoadingStateChanged", false, false);
NWWindow.prototype.onDocumentStart     = bindingUtil.createCustomEvent("nw.Window.onDocumentStart",     false, false);
NWWindow.prototype.onDocumentEnd       = bindingUtil.createCustomEvent("nw.Window.onDocumentEnd",       false, false);
NWWindow.prototype.onZoom              = bindingUtil.createCustomEvent("nw.Window.onZoom",              false, false);
NWWindow.prototype.onClose             = bindingUtil.createCustomEvent("nw.Window.onClose", true, false);
NWWindow.prototype.onMinimized         = bindingUtil.createCustomEvent("nw.Window.onMinimized", false, false);
NWWindow.prototype.onMaximized         = bindingUtil.createCustomEvent("nw.Window.onMaximized", false, false);
NWWindow.prototype.onFullscreen        = bindingUtil.createCustomEvent("nw.Window.onFullscreen", false, false);
NWWindow.prototype.onResized           = bindingUtil.createCustomEvent("nw.Window.onResized", false, false);
NWWindow.prototype.onRestore           = bindingUtil.createCustomEvent("nw.Window.onRestore", false, false);

NWWindow.prototype.close = function (force) {
  currentNWWindowInternal.close(force, this.cWindow.id);
}

NWWindow.prototype.once = function (event, listener, record) {
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
  this.on(event, g, false);
  return this;
};

NWWindow.prototype.on = function (event, callback, record) {
  var self = this;

  // Wrap callback to bind to `self`.
  // If `cb` is given, use `cb` instead of original `callback`.
  function wrap(cb) {
    var fn = (cb || callback).bind(self);
    fn.listener = callback;
    callback.__nw_cb = fn;
    return fn;
  }

  if (event === 'close') {
    var cbc = wrap(function(windowId, flag) {
      if (self.cWindow.id !== windowId)
        return;
      callback.call(self, flag);
    });
    chrome.windows.onRemoving.addListener(cbc, {instanceId: self.cWindow.id});
    return this;
  }
  switch (event) {
  case 'focus':
    var cbf = wrap(function(windowId) {
      if (self.cWindow.id !== windowId)
        return;
      callback.call(self);
    });
    chrome.windows.onFocusChanged.addListener(cbf);
    break;
  case 'blur':
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    var cbf = wrap(function(windowId) {
      if (self.cWindow.id === windowId)
        return;
      if (!self.cWindow.focused)
        return;
      callback.call(self);
    });
    chrome.windows.onFocusChanged.addListener(cbf);
    break;
  case 'closed':
    var cbr = wrap(function(windowId) {
      if (self.cWindow.id !== windowId)
        return;
      callback.call(self);
    });
    chrome.windows.onRemoved.addListener(cbr);
    break;
  case 'loaded':
    var g = wrap(function(tabId, changeInfo, tab) {
      if (tab.windowId !== self.cWindow.id)
        return;
      if ('status' in changeInfo && changeInfo.status == 'complete')
        callback.call(self);
    });
    chrome.tabs.onUpdated.addListener(g);
    break;
  case 'document-start':
    var cb1 = wrap(function(frame, top_routing_id) {
      if (top_routing_id !== self.cWindow.tabs[0].mainFrameId)
        return;
      callback.call(self, frame);
    });
    this.onDocumentStart.addListener(cb1);
    break;
  case 'document-end':
    var cb0 = wrap(function(frame, top_routing_id) {
      //console.log("document-end: cWindow: " + self.cWindow.id + "; top routing id: " + top_routing_id + "; main frame id: " + self.cWindow.tabs[0].mainFrameId);
      if (top_routing_id !== self.cWindow.tabs[0].mainFrameId)
        return;
      callback.call(self, frame);
    });
    this.onDocumentEnd.addListener(cb0);
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
    var k = wrap(function(w) {
      if (w.id != self.cWindow.id)
        return;
      callback.call(self, w.left, w.top);
    });
    chrome.windows.onMove.addListener(k);
    return this; //return early
    break;
  }
  if (nwWinEventsMap.hasOwnProperty(event)) {
    this[nwWinEventsMap[event]].addListener(wrap());
    return this;
  }
  return this;
};
NWWindow.prototype.removeListener = function (event, callback) {
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
  if (wrapEventsMapNewWin.hasOwnProperty(event)) {
    for (let l of chrome.windows[wrapEventsMapNewWin[event]].getListeners()) {
      if (l.listener && l.listener === callback) {
        chrome.windows[wrapEventsMapNewWin[event]].removeListener(l);
        return this;
      }
    }
  }
  return this;
};

NWWindow.prototype.removeAllListeners = function (event) {
  if (arguments.length === 0) {
    var obj = Object.assign({}, nwWinEventsMap, nwWrapEventsMap);
    var keys = Object.keys(obj);
    for (var i = 0, key; i < keys.length; ++i) {
      key = keys[i];
      this.removeAllListeners(key);
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
  if (wrapEventsMapNewWin.hasOwnProperty(event)) {
    for (let l of chrome.windows[wrapEventsMapNewWin[event]].getListeners()) {
      chrome.windows[wrapEventsMapNewWin[event]].removeListener(l);
    }
    return this;
  }
  return this;
};

NWWindow.prototype.setShadow = function(shadow) {
  currentNWWindowInternal.setShadow(shadow);
};

NWWindow.prototype.showDevTools = function(frm, callback) {
  var id = '';
  if (typeof frm === 'string')
    id = frm;
  var f = null;
  if (id)
    f = this.window.getElementById(id);
  else
    f = frm || null;
  nwNatives.setDevToolsJail(f);
  currentNWWindowInternal.showDevTools2Internal(this.cWindow.id, callback);
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
  chrome.tabs.reload(this.cWindow.tabs[0].id);
};
NWWindow.prototype.reloadIgnoringCache = function () {
  chrome.tabs.reload(this.cWindow.tabs[0].id, {'bypassCache': true});
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
  chrome.windows.update(this.cWindow.id, {'state':'normal'});
};
NWWindow.prototype.hide = function () {
  chrome.windows.update(this.cWindow.id, {'state':'hidden'});
};
NWWindow.prototype.focus = function () {
  chrome.windows.update(this.cWindow.id, {'focused':true});
};
NWWindow.prototype.blur = function () {
  chrome.windows.update(this.cWindow.id, {'focused':false});
};
NWWindow.prototype.minimize = function () {
  chrome.windows.update(this.cWindow.id, {'state':'minimized'});
};
NWWindow.prototype.maximize = function () {
  chrome.windows.update(this.cWindow.id, {'state':"maximized"});
};
NWWindow.prototype.unmaximize = NWWindow.prototype.restore = function () {
  chrome.windows.update(this.cWindow.id, {'state':"normal"});
};
NWWindow.prototype.enterFullscreen = function () {
  chrome.windows.update(this.cWindow.id, {'state':"fullscreen"});
};
NWWindow.prototype.leaveFullscreen = function () {
  var self = this;
  chrome.windows.get(this.cWindow.id, {}, function(w) {
    self.cWindow = w;
    if (w.state === 'fullscreen')
      chrome.windows.update(w.id, {'state':"normal"});
  });
};

NWWindow.prototype.toggleFullscreen = function () {
  var self = this;
  chrome.windows.get(this.cWindow.id, {}, function(w) {
    self.cWindow = w;
    if (w.state === 'fullscreen')
      chrome.windows.update(w.id, {'state':"normal"});
    else
      self.enterFullscreen();
  });
};

NWWindow.prototype.setAlwaysOnTop = function (top) {
  chrome.windows.update(this.cWindow.id, {'alwaysOnTop': top});
};
NWWindow.prototype.setPosition = function (pos) {
  this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
  if (pos == "center") {
    var screenWidth = screen.availWidth;
    var screenHeight = screen.availHeight;
    var width  = this.cWindow.width;
    var height = this.cWindow.height;
    chrome.windows.update(this.cWindow.id, {'left': Math.round((screenWidth-width)/2),
                                            'top': Math.round((screenHeight-height)/2)});
  }
};
NWWindow.prototype.setVisibleOnAllWorkspaces = function(all_visible) {
  chrome.windows.update(this.cWindow.id, {'allVisible': all_visible});
};
NWWindow.prototype.canSetVisibleOnAllWorkspaces = function() {
  return canSetVisibleOnAllWorkspaces;
};
NWWindow.prototype.setMaximumSize = function (width, height) {
  chrome.windows.update(this.cWindow.id, {'maxWidth': width, 'maxHeight': height});
};
NWWindow.prototype.setMinimumSize = function (width, height) {
  //TODO: cover glass frame case in windows
  chrome.windows.update(this.cWindow.id, {'minWidth': width, 'minHeight': height});
};
NWWindow.prototype.resizeTo = function (width, height) {
  chrome.windows.update(this.cWindow.id, {'width': width, 'height': height});
};
NWWindow.prototype.resizeBy = function (width, height) {
  this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
  chrome.windows.update(this.cWindow.id,
                        {'width': this.cWindow.width + width,
                         'height': this.cWindow.height + height});
};
NWWindow.prototype.moveTo = function (x, y) {
  chrome.windows.update(this.cWindow.id, {'left': x, 'top': y});
};
NWWindow.prototype.moveBy = function (x, y) {
  this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
  chrome.windows.update(this.cWindow.id,
                        {'left': this.cWindow.left + x,
                         'top': this.cWindow.top + y});
};
NWWindow.prototype.setResizable = function (resizable) {
  chrome.windows.update(this.cWindow.id, {'resizable': resizable});
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
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.left;
  },
  set: function(x) {
    chrome.windows.update(this.cWindow.id, {'left': x});
  }
});
Object.defineProperty(NWWindow.prototype, 'y', {
  get: function() {
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.top;
  },
  set: function(y) {
    chrome.windows.update(this.cWindow.id, {'top': y});
  }
});
Object.defineProperty(NWWindow.prototype, 'width', {
  get: function() {
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.width;
  },
  set: function(val) {
    chrome.windows.update(this.cWindow.id, {'width': val});
  }
});
Object.defineProperty(NWWindow.prototype, 'height', {
  get: function() {
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.height;
  },
  set: function(val) {
    chrome.windows.update(this.cWindow.id, {'height': val});
  }
});
Object.defineProperty(NWWindow.prototype, 'title', {
  get: function() {
    return currentNWWindowInternal.getTitleInternal(this.cWindow.id);
  },
  set: function(val) {
    currentNWWindowInternal.setTitleInternal(val, this.cWindow.id);
  }
});
Object.defineProperty(NWWindow.prototype, 'zoomLevel', {
  get: function() {
    return currentNWWindowInternal.getZoom(this.cWindow.id);
  },
  set: function(val) {
    currentNWWindowInternal.setZoom(val, this.cWindow.id);
  }
});
Object.defineProperty(NWWindow.prototype, 'isTransparent', {
  get: function() {
    return this.appWindow.alphaEnabled();
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
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.state === 'fullscreen';
  }
});
Object.defineProperty(NWWindow.prototype, 'isAlwaysOnTop', {
  get: function() {
    this.cWindow = currentNWWindowInternal.getCurrent(this.cWindow.id, {'populate': true});
    return this.cWindow.alwaysOnTop;
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
      currentNWWindowInternal.clearMenu(this.cWindow.id);
      return;
    }
    if (menu.type != 'menubar')
      throw new TypeError('Only menu of type "menubar" can be used as this.window menu');

    privates(this).menu =  menu;
    var menuPatch = currentNWWindowInternal.setMenu(menu.id, this.cWindow.id);
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
    return appWindowNatives.GetFrame(this.cWindow.tabs[0].mainFrameId, false);
  }
});
Object.defineProperty(NWWindow.prototype, 'frameId', {
  get: function() {
    return currentRoutingID;
  }
});

apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  currentNWWindowInternal = getInternalApi('nw.currentWindowInternal');
  forEach(currentNWWindowInternal, function(key, value) {
    if (!key.endsWith('Internal'))
      NWWindow.prototype[key] = value;
  });
  apiFunctions.setHandleRequest('get', function(domWindow) {
    if (domWindow)
      return try_nw(domWindow.top).nw.Window.get();
    if (currentNWWindow)
      return currentNWWindow;

    currentNWWindow = new NWWindow;
    return currentNWWindow;
  });

  apiFunctions.setHandleRequest('open', function(url, params, callback) {
    var options = {'url': url, 'setSelfAsOpener': true, 'type': 'popup'};
    //FIXME: unify this conversion code with nwjs/default.js
    if (params) {
      if (params.frame === false)
        options.frameless = true;
      if (params.resizable === false)
        options.resizable = false;
      if (params.focus === false)
        options.focused = false;
      if (params.x)
        options.left = params.x;
      if (params.y)
        options.top = params.y;
      if (params.height)
        options.height = params.height;
      if (params.width)
        options.width = params.width;
      if (params.min_width)
        options.minWidth = params.min_width;
      if (params.max_width)
        options.maxWidth = params.max_width;
      if (params.min_height)
        options.minHeight = params.min_height;
      if (params.max_height)
        options.maxHeight = params.max_height;
      if (params.fullscreen === true)
        options.state = 'fullscreen';
      if (params.show === false)
        options.hidden = true;
      if (params.show_in_taskbar === false)
        options.showInTaskbar = false;
      if (params['always_on_top'] === true)
        options.alwaysOnTop = true;
      if (params['visible_on_all_workspaces'] === true)
        options.allVisible = true;
      if (typeof params['inject_js_start'] == 'string')
         options.inject_js_start = params['inject_js_start'];
      if (typeof params['inject_js_end'] == 'string')
         options.inject_js_end = params['inject_js_end'];
      if (params.transparent)
         options.alphaEnabled = true;
      // if (params.kiosk === true)
      //   options.kiosk = true;
      if (params.new_instance === true) {
        options.new_instance = true;
        options.setSelfAsOpener = false;
      }
      if (params.position)
        options.position = params.position;
      if (params.title)
        options.title = params.title;
      if (params.icon)
        options.icon = params.icon;
      //if (params.id)
      //  options.tabId = params.id;
    }
    try_hidden(window).chrome.windows.create(options, function(cWin) {
      if (callback) {
        if (cWin)
          callback(new NWWindow(cWin));
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

function onDocumentStartEnd(start, frame, top_routing_id) {
  if (start) {
    dispatchEventIfExists(currentNWWindow, "onDocumentStart", [frame, top_routing_id]);
  }
  else
    dispatchEventIfExists(currentNWWindow, "onDocumentEnd", [frame, top_routing_id]);
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
  appWindowNatives.FixGamePadAPI();
  var nw0 = try_nw(window).nw;
  if (nw0)
    nw0.Window.get();
}

//if (bgPage !== window) {
//  renderFrameObserverNatives.OnDocumentElementCreated(currentRoutingID, get_nw);
//}

exports.onNewWinPolicy = onNewWinPolicy;
exports.onNavigation = onNavigation;
exports.LoadingStateChanged = onLoadingStateChanged;
exports.onDocumentStartEnd = onDocumentStartEnd;
exports.onClose = onClose;
exports.updateAppWindowZoom = updateAppWindowZoom;
