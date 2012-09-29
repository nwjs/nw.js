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

  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';

  if (!option.hasOwnProperty('icon'))
    option.icon = '';

  if (option.hasOwnProperty('menu') &&
      this.getConstructorName.call(option.menu) != 'Menu')
    throw new String("'menu' must be a valid Menu object");

  option.title = String(option.title);
  option.tooltip = String(option.tooltip);
  option.icon = String(option.icon);

  this.setHiddenValue('option', option);
  nw.allocateObject(this, option);
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
  this.handleSetter('icon', 'SetIcon', String, val);
});

Tray.prototype.__defineGetter__('tooltip', function() {
  return this.handleGetter('tooltip');
});

Tray.prototype.__defineSetter__('tooltip', function(val) {
  this.handleSetter('tooltip', 'SetTooltip', String, val);
});

Tray.prototype.__defineGetter__('menu', function() {
  return this.handleGetter('menu');
});

Tray.prototype.__defineSetter__('menu', function(val) {
  if (this.getConstructorName.call(val) != 'Menu')
    throw new String("'menu' property requries a valid Menu");

  this.getHiddenValue('option').menu = val;
  nw.callObjectMethod(this, 'SetMenu', [ val ]);
});

Tray.prototype.remove = function() {
  nw.callObjectMethod(this, 'Remove', []);
}

exports.Tray = Tray;
