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

function MenuItem(option) {
  if (typeof option != 'object')
    throw new String('Invalid option.');

  if (!option.type)
    option.type = 'normal';

  if (option.type != 'normal' && option.type != 'separator')
    throw new String('Invalid MenuItem type: ' + option.type);

  if (!option.hasOwnProperty('label'))
    option.label = '';

  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';

  if (!option.hasOwnProperty('enabled'))
    option.enabled = true;

  if (option.hasOwnProperty('submenu') &&
      this.getConstructorName.call(option.submenu) != 'Menu')
    throw new String("'submenu' must be a valid MenuItem");

  if (option.hasOwnProperty('click')) {
    if (typeof option.click != 'function')
      throw new String("'click' must be a valid Function");
    else
      this.click = option.click;
  }

  option.label = String(option.label);
  option.tooltip = String(option.tooltip);
  option.enabled = Boolean(option.enabled);
  option.checked = Boolean(option.checked);

  this.setHiddenValue('option', option);
  nw.allocateObject(this, option);
}
nw.inherits(MenuItem, exports.Base);

MenuItem.prototype.__defineGetter__('type', function() {
  return this.handleGetter('type');
});

MenuItem.prototype.__defineSetter__('type', function() {
  throw new String("'type' is immutable at runtime");
});

MenuItem.prototype.__defineGetter__('label', function() {
  return this.handleGetter('label');
});

MenuItem.prototype.__defineSetter__('label', function(val) {
  this.handleSetter('label', 'SetLabel', String, val);
});

MenuItem.prototype.__defineGetter__('icon', function() {
  return this.handleGetter('icon');
});

MenuItem.prototype.__defineSetter__('icon', function(val) {
  this.handleSetter('icon', 'SetIcon', String, val);
});

MenuItem.prototype.__defineGetter__('tooltip', function() {
  return this.handleGetter('tooltip');
});

MenuItem.prototype.__defineSetter__('tooltip', function(val) {
  this.handleSetter('tooltip', 'SetTooltip', String, val);
});

MenuItem.prototype.__defineGetter__('checked', function() {
  return this.handleGetter('checked');
});

MenuItem.prototype.__defineSetter__('checked', function(val) {
  this.handleSetter('checked', 'SetChecked', Boolean, val);
});

MenuItem.prototype.__defineGetter__('enabled', function() {
  return this.handleGetter('enabled');
});

MenuItem.prototype.__defineSetter__('enabled', function(val) {
  this.handleSetter('enabled', 'SetEnabled', Boolean, val);
});

MenuItem.prototype.__defineGetter__('submenu', function() {
  return this.handleGetter('submenu');
});

MenuItem.prototype.__defineSetter__('submenu', function(val) {
  if (this.getConstructorName.call(val) != 'Menu')
    throw new String("'submenu' property requries a valid Menu");

  this.getHiddenValue('option').submenu = val;
  nw.callObjectMethod(this, 'SetMenu', [ val ]);
});

MenuItem.prototype.handleResponse = function(ev) {
  if (ev == 'click' && typeof this.click == 'function')
    this.click();
  else if (typeof this.on == 'function')
    this.on.apply(this, arguments);
}

exports.MenuItem = MenuItem;
