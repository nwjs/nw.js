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

native function CallObjectMethod();
native function CallObjectMethodSync();

// Route events.
Window.prototype.handleEvent = function(ev) {
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
  this.window.moveTo(x, this.window.screenY);
});

Window.prototype.__defineGetter__('x', function() {
  return this.window.screenX;
});

Window.prototype.__defineSetter__('y', function(y) {
  this.window.moveTo(this.window.screenX, y);
});

Window.prototype.__defineGetter__('y', function() {
  return this.window.screenY;
});

Window.prototype.__defineSetter__('width', function(width) {
  this.window.resizeTo(width, this.window.outerHeight);
});

Window.prototype.__defineGetter__('width', function() {
  return this.window.outerWidth;
});

Window.prototype.__defineSetter__('height', function(height) {
  this.window.resizeTo(this.window.outerWidth, height);
});

Window.prototype.__defineGetter__('height', function() {
  return this.window.outerHeight;
});

Window.prototype.__defineSetter__('title', function(title) {
  this.window.document.title = title;
});

Window.prototype.__defineGetter__('title', function() {
  return this.window.document.title;
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
  this.window.moveTo(x, y);
}

Window.prototype.moveBy = function(x, y) {
  this.window.moveBy(x, y);
}

Window.prototype.resizeTo = function(width, height) {
  this.window.resizeTo(width, height);
}

Window.prototype.resizeBy = function(width, height) {
  this.window.resizeBy(width, height);
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

Window.prototype.requestAttention = function(flash) {
  flash = Boolean(flash);
  CallObjectMethod(this, 'RequestAttention', [ flash ]);
}

Window.prototype.setPosition = function(position) {
  if (position != 'center' && position != 'mouse')
    throw new String('Invalid postion');
  CallObjectMethod(this, 'SetPosition', [ position ]);
}

}  // function Window.init
