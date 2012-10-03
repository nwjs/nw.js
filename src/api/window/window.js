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

function Window() {
  nw.allocateObject(this, {});
}
nw.inherits(Window, exports.Base);

Window.prototype.__defineSetter__('x', function(x) {
  window.moveTo(x, window.screenY);
});

Window.prototype.__defineGetter__('x', function() {
  return window.screenX;
});

Window.prototype.__defineSetter__('y', function(y) {
  window.moveTo(window.screenX, y);
});

Window.prototype.__defineGetter__('y', function() {
  return window.screenY;
});

Window.prototype.__defineSetter__('width', function(width) {
  window.resizeTo(width, window.innerHeight);
});

Window.prototype.__defineGetter__('width', function() {
  return window.innerWidth;
});

Window.prototype.__defineSetter__('height', function(height) {
  window.resizeTo(window.innerWidth, height);
});

Window.prototype.__defineGetter__('height', function() {
  return window.innerHeight;
});

Window.prototype.moveTo = function(x, y) {
  window.moveTo(x, y);
}

Window.prototype.moveBy = function(x, y) {
  window.moveBy(x, y);
}

Window.prototype.resizeTo = function(width, height) {
  window.resizeTo(width, height);
}

Window.prototype.resizeBy = function(width, height) {
  window.resizeBy(width, height);
}

Window.prototype.focus = function(flag) {
  if (typeof flag == 'undefined' || Boolean(flag))
    window.focus();
  else
    this.blur();
}

Window.prototype.blur = function() {
  window.blur();
}

Window.prototype.show = function(flag) {
  if (typeof flag == 'undefined' || Boolean(flag))
    nw.callObjectMethod(this, 'Show', []);
  else
    this.hide();
}

Window.prototype.hide = function() {
  nw.callObjectMethod(this, 'Hide', []);
}

Window.prototype.close = function() {
  window.close();
}

Window.prototype.maximize = function() {
  nw.callObjectMethod(this, 'Maximize', []);
}

Window.prototype.unmaximize = function() {
  nw.callObjectMethod(this, 'Unmaximize', []);
}

Window.prototype.minimize = function() {
  nw.callObjectMethod(this, 'Minimize', []);
}

Window.prototype.restore = function() {
  nw.callObjectMethod(this, 'Restore', []);
}

exports.Window = {
  get: function() {
    if (nwDispatcher.windowInstance == null) {
      nwDispatcher.windowInstance = new Window();
    }

    return nwDispatcher.windowInstance;
  },

  from: function(win) {
    return win.require('nw.gui').Window.get();
  }
};
