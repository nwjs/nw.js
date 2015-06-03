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

var v8_util = process.binding('v8_util');

function Tray(option) {
  if (typeof option != 'object')
    throw new TypeError('Invalid option');

  if (!option.hasOwnProperty('title') && !option.hasOwnProperty('icon'))
    throw new TypeError("Must set 'title' or 'icon' field in option");

  if (!option.hasOwnProperty('title'))
    option.title = '';
  else
    option.title = String(option.title);

  if (option.hasOwnProperty('icon')) {
    option.shadowIcon = String(option.icon);
    option.icon = nw.getAbsolutePath(option.icon);
  }

  if (option.hasOwnProperty('alticon')) {
    option.shadowAlticon = String(option.alticon);
    option.alticon = nw.getAbsolutePath(option.alticon);
  }

  if (option.hasOwnProperty('iconsAreTemplates'))
    option.iconsAreTemplates = Boolean(option.iconsAreTemplates);
  else
    option.iconsAreTemplates = true;

  if (option.hasOwnProperty('tooltip'))
    option.tooltip = String(option.tooltip);

  if (option.hasOwnProperty('click')) {
    if (typeof option.click != 'function') {
      throw new TypeError("'click' must be a valid Function");
    } else {
      this.click = option.click;
    }
   }

  if (option.hasOwnProperty('menu')) {
    if (v8_util.getConstructorName(option.menu) != 'Menu')
      throw new TypeError("'menu' must be a valid Menu");

    // Transfer only object id
    v8_util.setHiddenValue(this, 'menu', option.menu);
    option.menu = option.menu.id;
  }

  v8_util.setHiddenValue(this, 'option', option);
  nw.allocateObject(this, option);

  // All properties must be set after initialization.
  if (!option.hasOwnProperty('icon'))
    option.shadowIcon = '';
  if (!option.hasOwnProperty('alticon'))
    option.shadowAlticon = '';
  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';
}
require('util').inherits(Tray, exports.Base);

Tray.prototype.__defineGetter__('title', function() {
  return this.handleGetter('title');
});

Tray.prototype.__defineSetter__('title', function(val) {
  this.handleSetter('title', 'SetTitle', String, val);
});

Tray.prototype.__defineGetter__('icon', function() {
  return this.handleGetter('shadowIcon');
});

Tray.prototype.__defineGetter__('alticon', function() {
  return this.handleGetter('shadowAlticon');
});

Tray.prototype.__defineSetter__('icon', function(val) {
  v8_util.getHiddenValue(this, 'option').shadowIcon = String(val);
  var real_path = val == '' ? '' : nw.getAbsolutePath(val);
  this.handleSetter('icon', 'SetIcon', String, real_path);
});

Tray.prototype.__defineSetter__('alticon', function(val) {
  v8_util.getHiddenValue(this, 'option').shadowAlticon = String(val);
  var real_path = val == '' ? '' : nw.getAbsolutePath(val);
  this.handleSetter('alticon', 'SetAltIcon', String, real_path);
});

Tray.prototype.__defineGetter__('iconsAreTemplates', function() {
  return this.handleGetter('iconsAreTemplates');
});

Tray.prototype.__defineSetter__('iconsAreTemplates', function(val) {
  this.handleSetter('iconsAreTemplates', 'SetIconsAreTemplates', Boolean, val);
});

Tray.prototype.__defineGetter__('tooltip', function() {
  return this.handleGetter('tooltip');
});

Tray.prototype.__defineSetter__('tooltip', function(val) {
  this.handleSetter('tooltip', 'SetTooltip', String, val);
});

Tray.prototype.__defineGetter__('menu', function() {
  return v8_util.getHiddenValue(this, 'menu');
});

Tray.prototype.__defineSetter__('menu', function(val) {
  if (v8_util.getConstructorName(val) != 'Menu')
    throw new TypeError("'menu' property requries a valid Menu");

  v8_util.setHiddenValue(this, 'menu', val);
  nw.callObjectMethod(this, 'SetMenu', [ val.id ]);
});

Tray.prototype.remove = function() {
  nw.callObjectMethod(this, 'Remove', []);
}

Tray.prototype.handleEvent = function(ev) {
 if (ev == 'click') {
   // Emit click handler
   if (typeof this.click == 'function'){
     this.click();
   }
 }
 // Emit generate event handler
 exports.Base.prototype.handleEvent.apply(this, arguments);
}
exports.Tray = Tray;
