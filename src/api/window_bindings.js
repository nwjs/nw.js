function Window(routing_id, nobind, predefined_id) {
    // Get and set id.
    native function CallObjectMethod();
    native function CallObjectMethodSync();
    native function AllocateId();

    var id;
    if (predefined_id)
        id = predefined_id;
    else
        id = AllocateId();

    Object.defineProperty(this, 'id', {
        value: id,
        writable: false
    });

    // Store routing id (need for IPC since we are in node's context).
    this.routing_id = routing_id;

    // Store myself in node's context.
    global.__nwWindowsStore[id] = this;
    global.__nwObjectsRegistry.set(id, this);

    // Tell Shell I'm the js delegate of it.
    native function BindToShell();
    if (!nobind)
        BindToShell(this.routing_id, this.id);

    var that = this;
    this.cookies = {
        req_id : 0,
        get : function(details, cb) {
            this.req_id++;
            if (typeof cb == 'function') {
                that.once('__nw_gotcookie' + this.req_id, function(cookie) {
                    if (cookie.length > 0)
                        cb(cookie[0]);
                    else
                        cb(null);
                });
            }
            CallObjectMethod(that, 'CookieGet', [ this.req_id, details ]);
        },
        getAll : function(details, cb) {
            this.req_id++;
            if (typeof cb == 'function') {
                that.once('__nw_gotcookie' + this.req_id, function(cookie) {
                    cb(cookie);
                });
            }
            CallObjectMethod(that, 'CookieGetAll', [ this.req_id, details ]);
        },
        remove : function(details, cb) {
            this.req_id++;
            if (typeof cb == 'function') {
                that.once('__nw_gotcookie' + this.req_id, function(details) {
                    cb(details);
                });
            }
            CallObjectMethod(that, 'CookieRemove', [ this.req_id, details ]);
        },
        set : function(details, cb) {
            this.req_id++;
            if (typeof cb == 'function') {
                that.once('__nw_gotcookie' + this.req_id, function(cookie) {
                    cb(cookie);
                });
            }
            CallObjectMethod(that, 'CookieSet', [ this.req_id, details ]);
        },
        onChanged : {
            addListener : function(cb) {
                that.on('__nw_cookie_changed', cb);
            },
            removeListener : function(cb) {
                that.removeListener('__nw_cookie_changed', cb);
            }
        }
    }
}

// Window will inherit EventEmitter in "third_party/node/src/node.js", do
// not inherit here becuase this file is loaded before everything else.

// And init everything after the inheritance.
Window.init = function() {

var v8_util = process.binding('v8_util');
var EventEmitter = process.EventEmitter;

native function CallObjectMethod();
native function CallObjectMethodSync();

// Override the addListener method.
Window.prototype.on = Window.prototype.addListener = function(ev, callback) {
  // Save window id of where the callback is created.
  var closure = v8_util.getCreationContext(callback);
  if (v8_util.getConstructorName(closure) == 'Window' &&
      closure.hasOwnProperty('nwDispatcher')) {
    v8_util.setHiddenValue(callback, '__nwWindowId',
        closure.nwDispatcher.requireNwGui().Window.get().id);
  }

  // Call parent.
  EventEmitter.prototype.addListener.apply(this, arguments);
}

// Route events.
Window.prototype.handleEvent = function(ev) {
  // Filter invalid callbacks.
  var listeners_copy = this.listeners(ev).slice(0);
  for (var i = 0; i < listeners_copy.length; ++i) {
    var original_closure = v8_util.getCreationContext(listeners_copy[i]);

    // Skip for node context.
    if (v8_util.getConstructorName(original_closure) != 'Window')
      continue;


    var window_id = v8_util.getHiddenValue(listeners_copy[i], '__nwWindowId');

    // Remove callback if original window is closed (not in __nwWindowsStore).
    if (global.__nwWindowsStore.hasOwnProperty(window_id)) {
      // Check hashes and see if the window context of Shell has been changed,
      // this happens when we refresh the Shell or change it's locations.
      var original_hash = v8_util.getObjectHash(original_closure);
      var current_hash = v8_util.getObjectHash(
          global.__nwWindowsStore[window_id].window);

      // Do nothing if nothing is changed.
      if (original_hash == current_hash)
        continue;
      if (original_closure.window.top === global.__nwWindowsStore[window_id].window) //iframe case
        continue;
    }

    console.warn('Remove zombie callback for window id ' + window_id + " ev: " + ev);
    this.removeListener(ev, listeners_copy[i]);
  }

    if (ev == 'new-win-policy' && arguments.length > 3) {
        var policy = arguments[3];
        policy.ignore         =  function () { this.val = 'ignore'; };
        policy.forceCurrent   =  function () { this.val = 'current'; };
        policy.forceDownload  =  function () { this.val = 'download'; };
        policy.forceNewWindow =  function () { this.val = 'new-window'; };
        policy.forceNewPopup  =  function () { this.val = 'new-popup'; };
    }
  // Route events to EventEmitter.
  this.emit.apply(this, arguments);

  // If no one is listening to 'close' then close directly
  if (ev == 'close' && this.listeners(ev).length == 0) {
    this.close(true);
    return;
  } else if (ev == 'closed') { // Clear me.
    delete global.__nwWindowsStore[this.id];
    return;
  }
}

// Return current window object of Shell's DOM.
Window.prototype.__defineGetter__('window', function() {
  native function GetWindowObject();
  return GetWindowObject(this.routing_id);
});

Window.prototype.__defineSetter__('x', function(x) {
  this.moveTo(x, this.y);
});

Window.prototype.__defineGetter__('x', function() {
  return CallObjectMethodSync(this, 'GetPosition', [])[0];
});

Window.prototype.__defineSetter__('y', function(y) {
  this.moveTo(this.x, y);
});

Window.prototype.__defineGetter__('y', function() {
  return CallObjectMethodSync(this, 'GetPosition', [])[1];
});

Window.prototype.__defineSetter__('width', function(width) {
  this.resizeTo(width, this.height);
});

Window.prototype.__defineGetter__('width', function() {
  return CallObjectMethodSync(this, 'GetSize', [])[0];
});

Window.prototype.__defineSetter__('height', function(height) {
  this.resizeTo(this.width, height);
});

Window.prototype.__defineGetter__('height', function() {
  return CallObjectMethodSync(this, 'GetSize', [])[1];
});

Window.prototype.__defineSetter__('title', function(title) {
  this.window.document.title = title;
});

Window.prototype.__defineGetter__('title', function() {
  return this.window.document.title;
});

Window.prototype.__defineSetter__('zoomLevel', function(level) {
  CallObjectMethodSync(this, 'SetZoomLevel', level);
});

Window.prototype.__defineGetter__('zoomLevel', function() {
  return CallObjectMethodSync(this, 'GetZoomLevel', [])[0];
});

Window.prototype.__defineSetter__('menu', function(menu) {
  if(!menu) {
    CallObjectMethod(this, "ClearMenu", []);
    return;
  }
  if (v8_util.getConstructorName(menu) != 'Menu')
    throw new String("'menu' property requries a valid Menu");

  if (menu.type != 'menubar')
    throw new String('Only menu of type "menubar" can be used as this.window menu');

  v8_util.setHiddenValue(this, 'menu', menu);
  CallObjectMethod(this, 'SetMenu', [ menu.id ]);
});

Window.prototype.__defineGetter__('menu', function() {
  return v8_util.getHiddenValue(this, 'menu');
});

Window.prototype.__defineSetter__('isFullscreen', function(flag) {
  if (flag)
    this.enterFullscreen();
  else
    this.leaveFullscreen();
});

Window.prototype.isDevToolsOpen = function () {
    var result = CallObjectMethodSync(this, 'IsDevToolsOpen', []);
    return Boolean(result[0]);
}

Window.prototype.__defineGetter__('isFullscreen', function() {
  var result = CallObjectMethodSync(this, 'IsFullscreen', []);
  return Boolean(result[0]);
});

Window.prototype.__defineSetter__('isKioskMode', function(flag) {
  if (flag)
    this.enterKioskMode();
  else
    this.leaveKioskMode();
});

Window.prototype.__defineGetter__('isKioskMode', function() {
  var result = CallObjectMethodSync(this, 'IsKioskMode', []);
  return Boolean(result[0]);
});

Window.prototype.moveTo = function(x, y) {
  CallObjectMethod(this, 'MoveTo', [ Number(x), Number(y) ]);
}

Window.prototype.moveBy = function(x, y) {
  var position = CallObjectMethodSync(this, 'GetPosition', []);
  this.moveTo(position[0] + x, position[1] + y);
}

Window.prototype.resizeTo = function(width, height) {
  CallObjectMethod(this, 'ResizeTo', [ Number(width), Number(height) ]);
}

Window.prototype.resizeBy = function(width, height) {
  var size = CallObjectMethodSync(this, 'GetSize', []);
  this.resizeTo(size[0] + width, size[1] + height);
}

Window.prototype.focus = function(flag) {
  if (typeof flag == 'undefined' || Boolean(flag)) {
      if (this.__nw_is_devtools)
          CallObjectMethod(this, 'Focus', []);
      else
          this.window.focus();
  } else
    this.blur();
};

Window.prototype.blur = function() {
    if (this.__nw_is_devtools)
        CallObjectMethod(this, 'Blur', []);
    else
        this.window.blur();
};

Window.prototype.show = function(flag) {
  if (typeof flag == 'undefined' || Boolean(flag))
    CallObjectMethod(this, 'Show', []);
  else
    this.hide();
}

Window.prototype.hide = function() {
  CallObjectMethod(this, 'Hide', []);
}

Window.prototype.close = function(force) {
  CallObjectMethod(this, 'Close', [ Boolean(force) ]);
}

Window.prototype.maximize = function() {
  CallObjectMethod(this, 'Maximize', []);
}

Window.prototype.unmaximize = function() {
  CallObjectMethod(this, 'Unmaximize', []);
}

Window.prototype.minimize = function() {
  CallObjectMethod(this, 'Minimize', []);
}

Window.prototype.restore = function() {
  CallObjectMethod(this, 'Restore', []);
}

Window.prototype.enterFullscreen = function() {
  CallObjectMethod(this, 'EnterFullscreen', []);
}

Window.prototype.leaveFullscreen = function() {
  CallObjectMethod(this, 'LeaveFullscreen', []);
}

Window.prototype.toggleFullscreen = function() {
  CallObjectMethod(this, 'ToggleFullscreen', []);
}

Window.prototype.enterKioskMode = function() {
  CallObjectMethod(this, 'EnterKioskMode', []);
}

Window.prototype.leaveKioskMode = function() {
  CallObjectMethod(this, 'LeaveKioskMode', []);
}

Window.prototype.toggleKioskMode = function() {
  CallObjectMethod(this, 'ToggleKioskMode', []);
}

Window.prototype.closeDevTools = function() {
  CallObjectMethod(this, 'CloseDevTools', []);
}

Window.prototype.showDevTools = function(frm, headless) {
    var id = '';
    if (typeof frm === 'string') {
        id = frm;
        this._pending_devtools_jail = null;
    }else{
        this._pending_devtools_jail = frm;
    }
    var win_id = CallObjectMethodSync(this, 'ShowDevTools', [id, Boolean(headless)])[0];
    var ret;
    if (typeof win_id == 'number' && win_id > 0) {
        ret = global.__nwWindowsStore[win_id];
        if (!ret) {
            ret = new global.Window(this.window.nwDispatcher.getRoutingIDForCurrentContext(), true, win_id);
            ret.__nw_is_devtools = true;
        }
        return ret;
    }
}

Window.prototype.__setDevToolsJail = function(id) {
    var frm = null;
    if (id)
        frm = this.window.document.getElementById(id);
    else
        frm = this._pending_devtools_jail || null;
    CallObjectMethod(this, 'setDevToolsJail', frm);
}

Window.prototype.setMinimumSize = function(width, height) {
  CallObjectMethod(this, 'SetMinimumSize', [ width, height ]);
}

Window.prototype.setMaximumSize = function(width, height) {
  CallObjectMethod(this, 'SetMaximumSize', [ width, height ]);
}

Window.prototype.setResizable = function(resizable) {
  resizable = Boolean(resizable);
  CallObjectMethod(this, 'SetResizable', [ resizable ]);
}

Window.prototype.setAlwaysOnTop = function(flag) {
  CallObjectMethod(this, 'SetAlwaysOnTop', [ Boolean(flag) ]);
}

Window.prototype.setShowInTaskbar = function(flag) {
  flag = Boolean(flag);
  CallObjectMethod(this, 'SetShowInTaskbar', [ flag ]);
}

Window.prototype.requestAttention = function(flash) {
  if (typeof flash == 'boolean') {
    // boolean true is redirected as -1 value
    flash = flash ? -1 : 0;
  }
  CallObjectMethod(this, 'RequestAttention', [ flash ]);
}

Window.prototype.setBadgeLabel = function(label) {
  label = "" + label;
  CallObjectMethod(this, 'SetBadgeLabel', [ label ]);
}

Window.prototype.setPosition = function(position) {
  if (position != 'center' && position != 'mouse')
    throw new String('Invalid postion');
  CallObjectMethod(this, 'SetPosition', [ position ]);
}

Window.prototype.reload = function(type) {
  // type is default to 0 (RELOAD).
  if (!(typeof type == 'number' && 0 <= type && type <= 3))
    type = 0;

  CallObjectMethod(this, 'Reload', [ type ]);
}

Window.prototype.reloadIgnoringCache = function() {
  this.reload(1);
}

Window.prototype.reloadOriginalRequestURL = function() {
  this.reload(2);
}

Window.prototype.reloadDev = function() {
  this.reload(3);
}

var mime_types = {
  'jpeg' : 'image/jpeg',
  'png'  : 'image/png'
}

Window.prototype.capturePage = function(callback, image_format_options) {
  var options;

  // Be compatible with the old api capturePage(callback, [format string])
  if (typeof image_format_options == 'string' || image_format_options instanceof String) {
    options = {
      format : image_format_options
    };
  } else {
    options = image_format_options || {};
  }

  if (options.format != 'jpeg' && options.format != 'png') {
    options.format = 'jpeg';
  }

  if (typeof callback == 'function') {
    this.once('__nw_capturepagedone', function(imgdata) {
      switch(options.datatype){
        case 'buffer' :
          callback(new Buffer(imgdata, "base64"));
          break;
        case 'raw' :
          callback(imgdata);
        case 'datauri' :
        default :
          callback('data:' + mime_types[options.format] + ';base64,' + imgdata );
      }
    });
  }

  CallObjectMethod(this, 'CapturePage', [options.format]);
};

    Window.prototype.eval = function(frame, script) {
        return CallObjectMethod(this, 'EvaluateScript', frame, script);
    };

}  // function Window.init
