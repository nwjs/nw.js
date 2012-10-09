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

function Tray(option) {
  if (typeof option != 'object')
    throw new String('Invalid option');

  if (!option.hasOwnProperty('title') && !option.hasOwnProperty('icon'))
    throw new String("Must set 'title' or 'icon' field in option");

  if (!option.hasOwnProperty('title'))
    option.title = '';
  else
    option.title = String(option.title);

  if (option.hasOwnProperty('tooltip'))
    option.tooltip = String(option.tooltip);

  if (option.hasOwnProperty('menu')) {
    if (this.getConstructorName.call(option.menu) != 'Menu')
      throw new String("'menu' must be a valid Menu");

    // Transfer only object id
    this.setHiddenValue('menu', option.menu);
    option.menu = option.menu.id;
  }

  this.setHiddenValue('option', option);
  nw.allocateObject(this, option);

  if (option.hasOwnProperty('icon'))
    this.icon = option.icon;
  else
    option.icon = '';

  // All properties must be set after initialization.
  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';
}
nw.inherits(Tray, exports.Base);

Tray.prototype.__defineGetter__('title', function() {
  return this.handleGetter('title');
});

Tray.prototype.__defineSetter__('title', function(val) {
  this.handleSetter('title', 'SetTitle', String, val);
});

Tray.prototype.__defineGetter__('icon', function() {
  return this.handleGetter('icon');
});

Tray.prototype.__defineSetter__('icon', function(val) {
  this.getHiddenValue('option').icon = String(val);
  nw.callObjectMethod(this, 'SetIcon', [ nw.getAbsolutePath(val) ]);
});

Tray.prototype.__defineGetter__('tooltip', function() {
  return this.handleGetter('tooltip');
});

Tray.prototype.__defineSetter__('tooltip', function(val) {
  this.handleSetter('tooltip', 'SetTooltip', String, val);
});

Tray.prototype.__defineGetter__('menu', function() {
  return this.getHiddenValue('menu');
});

Tray.prototype.__defineSetter__('menu', function(val) {
  if (this.getConstructorName.call(val) != 'Menu')
    throw new String("'menu' property requries a valid Menu");

  this.setHiddenValue('menu', val);
  nw.callObjectMethod(this, 'SetMenu', [ val.id ]);
});

Tray.prototype.remove = function() {
  nw.callObjectMethod(this, 'Remove', []);
}

exports.Tray = Tray;
