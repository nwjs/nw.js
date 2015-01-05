// Copyright (c) 2012 Intel Corp
// Copyright (c) 2014 The Chromium Authors
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

var screenInstance = null;

function Screen() {
  nw.allocateObject(this, {});
  this._numListener = 0;
}
require('util').inherits(Screen, exports.Base);

// Override the addListener method.
Screen.prototype.on = Screen.prototype.addListener = function(ev, callback) {
  if ( ev != "displayBoundsChanged" && ev != "displayAdded" && ev != "displayRemoved" )
    throw new String('only following event can be listened: displayBoundsChanged, displayAdded, displayRemoved');
  
  var onRemoveListener = function (type, listener) {
    if (this._numListener > 0) {
      this._numListener--;
      if (this._numListener == 0) {
        process.EventEmitter.prototype.removeListener.apply(this, ["removeListener", onRemoveListener]);
        nw.callStaticMethodSync('Screen', 'RemoveScreenChangeCallback', [ this.id ]);
      }
    }
  };

  if(this._numListener == 0) {
    if (nw.callStaticMethodSync('Screen', 'AddScreenChangeCallback', [ this.id ]) == false ) {
      throw new String('nw.callStaticMethodSync(Screen, AddScreenChangeCallback) fails');
      return;
    }
    process.EventEmitter.prototype.addListener.apply(this, ["removeListener", onRemoveListener]);
  }
  
  // Call parent.
  process.EventEmitter.prototype.addListener.apply(this, arguments);
  this._numListener++;
}

// Route events.
Screen.prototype.handleEvent = function(ev) {
  arguments[1] = JSON.parse(arguments[1]);
  // Call parent.
  this.emit.apply(this, arguments);
}

Screen.prototype.__defineGetter__('screens', function() {
  return JSON.parse(nw.callStaticMethodSync('Screen', 'GetScreens', [ ]));
});


// ======== Screen functions End ========

// Store App object in node's context.
exports.Screen = {
Init: function() {
  if (screenInstance == null) {
    screenInstance = new Screen();
  }
  exports.Screen = screenInstance;
  return screenInstance;
}
};

