function Window(routing_id) {
  // Get and set id.
  var id = global.__nwObjectsRegistry.allocateId();
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
  BindToShell(this.routing_id, this.id);
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
    }

    console.warn('Remove zombie callback for window id ' + window_id);
    this.removeListener(ev, listeners_copy[i]);
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
  if (typeof flag == 'undefined' || Boolean(flag))
    this.window.focus();
  else
    this.blur();
}

Window.prototype.blur = function() {
  this.window.blur();
}

Window.prototype.show = function(flag) {
  if (typeof flag == 'undefined' || Boolean(flag))
    CallObjectMethod(this, 'Show', []);
  else
    this.hide();
}

Window.prototype.hide = function() {
  CallObjectMethod(this, 'Hide', []);
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

Window.prototype.showDevTools = function() {
  CallObjectMethod(this, 'ShowDevTools', []);
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
  flash = Boolean(flash);
  CallObjectMethod(this, 'RequestAttention', [ flash ]);
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

}  // function Window.init
