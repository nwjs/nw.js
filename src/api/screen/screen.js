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

function DesktopCaptureMonitor() {
    nw.allocateObject(this, {});
    this.sources = new Array();
    this.started = false;
}
require('util').inherits(DesktopCaptureMonitor, exports.Base);


DesktopCaptureMonitor.prototype.start = function (screens, windows) {
    if (this.started)
        return false;
    this.started = true;
    nw.callObjectMethodSync(this, 'start', [screens, windows]);
    return true;
}

DesktopCaptureMonitor.prototype.stop = function () {
    if (!this.started)
        return false;
    nw.callObjectMethodSync(this, 'stop', []);
    this.started = false;
    this.sources = new Array();
    return true;
}

DesktopCaptureMonitor.prototype.on('__nw_desktop_capture_monitor_listner_added', function (id, name, order, type) {
	if(this.sources.indexOf(id)!=-1)
	{
		//TODO: Find out what this event comes twice on some platforms
		return;
	}
    this.sources.splice(order, 0, id);
    this.emit("added", id, name, order, type);
    for (var i = order + 1; i <= this.sources.length - 1; i++) {
        this.emit("orderchanged", this.sources[i], i, i - 1);
    }
});


DesktopCaptureMonitor.prototype.on('__nw_desktop_capture_monitor_listner_removed', function (index) {
    var id = this.sources[index];
    if (index != -1) {
        this.sources.splice(index, 1);
        this.emit("removed", id);
        for (var i = index; i <= this.sources.length - 1; i++) {
            this.emit("orderchanged", this.sources[i], i, i + 1);
        }
    }
});

DesktopCaptureMonitor.prototype.on('__nw_desktop_capture_monitor_listner_moved', function (id, new_index, old_index) {
    var temp = this.sources[old_index];
    this.sources.splice(old_index, 1);
    this.sources.splice(new_index, 0, temp);
    this.emit("orderchanged", temp, new_index, old_index);
    for (var i = new_index; i < old_index; i++)
        this.emit("orderchanged", this.sources[i + 1], i + 1, i);
});

DesktopCaptureMonitor.prototype.on('__nw_desktop_capture_monitor_listner_namechanged', function (id, name) {
    this.emit("namechanged", id, name);
});

DesktopCaptureMonitor.prototype.on('__nw_desktop_capture_monitor_listner_thumbnailchanged', function (id, thumbnail) {
    this.emit("thumbnailchanged", id, thumbnail);
});

var listenerCount=0;
DesktopCaptureMonitor.prototype.on = DesktopCaptureMonitor.prototype.addListener = function (ev, callback) {
    //throw except if unsupported event
    if (ev != "added" && ev != "removed" && ev != "orderchanged" && ev != "namechanged" && ev != "thumbnailchanged")
        throw new String("only following events can be listened: added, removed, moved, namechanged, thumbnailchanged");

    process.EventEmitter.prototype.addListener.apply(this, arguments);
}


exports.DesktopCaptureMonitor=DesktopCaptureMonitor;

var screenInstance = null;

function Screen() {
  nw.allocateObject(this, {});
  this._numListener = 0;
  this.DesktopCaptureMonitor = new DesktopCaptureMonitor();
}
require('util').inherits(Screen, exports.Base);

// Override the addListener method.
Screen.prototype.on = Screen.prototype.addListener = function(ev, callback) {
  if ( ev != "displayBoundsChanged" && ev != "displayAdded" && ev != "displayRemoved" && ev != "chooseDesktopMedia")
    throw new TypeError('only following event can be listened: displayBoundsChanged, displayAdded, displayRemoved');
  
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
    if (nw.callStaticMethodSync('Screen', 'AddScreenChangeCallback', [ this.id ])[0] == false ) {
      throw new Error('nw.callStaticMethodSync(Screen, AddScreenChangeCallback) fails');
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
  if (ev != "chooseDesktopMedia")
    arguments[1] = JSON.parse(arguments[1]);
  // Call parent.
  this.emit.apply(this, arguments);
}

Screen.prototype.__defineGetter__('screens', function() {
  return JSON.parse(nw.callStaticMethodSync('Screen', 'GetScreens', [ ]));
});

Screen.prototype.chooseDesktopMedia = function(array, callback) {
  if(nw.callStaticMethodSync('Screen', 'ChooseDesktopMedia', [ this.id, array ])[0]) {
    this.once('chooseDesktopMedia', callback);
    return true;
  }
  return false;
}

Screen.prototype.cancelChooseDesktopMedia = function() {
  return nw.callStaticMethodSync('Screen', 'CancelChooseDesktopMedia', [ this.id ])[0];
}
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

