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

function MenuItem(option) {
  if (typeof option != 'object')
    throw new String('Invalid option.');

  if (!option.hasOwnProperty('type'))
    option.type = 'normal';

  if (option.type != 'normal' &&
      option.type != 'checkbox' &&
      option.type != 'separator')
    throw new String('Invalid MenuItem type: ' + option.type);

  if (option.type == 'normal' || option.type == 'checkbox') {
    if (option.type == 'checkbox')
      option.checked = Boolean(option.checked);

    if (!option.hasOwnProperty('label'))
      throw new String('A normal MenuItem must have a label');
    else
      option.label = String(option.label);

    if (option.hasOwnProperty('icon')) {
      option.shadowIcon = String(option.icon);
      option.icon = nw.getAbsolutePath(option.icon);
    }

    if (option.hasOwnProperty('iconIsTemplate'))
      option.iconIsTemplate = Boolean(option.iconIsTemplate);
    else
      option.iconIsTemplate = true;

    if (option.hasOwnProperty('tooltip'))
      option.tooltip = String(option.tooltip);

    if (option.hasOwnProperty('enabled'))
      option.enabled = Boolean(option.enabled);

    if (option.hasOwnProperty('submenu')) {
      if (v8_util.getConstructorName(option.submenu) != 'Menu')
        throw new String("'submenu' must be a valid Menu");

      // Transfer only object id
      v8_util.setHiddenValue(this, 'submenu', option.submenu);
      option.submenu = option.submenu.id;
    }

    if (option.hasOwnProperty('click')) {
      if (typeof option.click != 'function')
        throw new String("'click' must be a valid Function");
      else
        this.click = option.click;
    }
  } else if (option.type == 'separator') {
    option = {
      type: 'separator'
    };
  }

  v8_util.setHiddenValue(this, 'option', option);
  nw.allocateObject(this, option);

  // All properties must be set after initialization.
  if (!option.hasOwnProperty('icon'))
    option.shadowIcon = '';
  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';
  if (!option.hasOwnProperty('enabled'))
    option.enabled = true;
  if (!option.hasOwnProperty('key'))
    option.key = "";
  if (!option.hasOwnProperty('modifiers'))
    option.modifiers = "";
}
require('util').inherits(MenuItem, exports.Base);

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
  return this.handleGetter('shadowIcon');
});

MenuItem.prototype.__defineSetter__('icon', function(val) {
  v8_util.getHiddenValue(this, 'option').shadowIcon = String(val);
  var real_path = val == '' ? '' : nw.getAbsolutePath(val);
  this.handleSetter('icon', 'SetIcon', String, real_path);
});

MenuItem.prototype.__defineGetter__('iconIsTemplate', function() {
  return this.handleGetter('iconIsTemplate');
});

MenuItem.prototype.__defineSetter__('iconIsTemplate', function(val) {
  this.handleSetter('iconIsTemplate', 'SetIconIsTemplate', Boolean, val);
});

MenuItem.prototype.__defineGetter__('tooltip', function() {
  return this.handleGetter('tooltip');
});

MenuItem.prototype.__defineSetter__('tooltip', function(val) {
  this.handleSetter('tooltip', 'SetTooltip', String, val);
});

MenuItem.prototype.__defineGetter__('key', function() {
  return this.handleGetter('key');
});

MenuItem.prototype.__defineSetter__('key', function(val) {
  this.handleSetter('key', 'SetKey', String, val);
});

MenuItem.prototype.__defineGetter__('modifiers', function() {
  return this.handleGetter('modifiers');
});

MenuItem.prototype.__defineSetter__('modifiers', function(val) {
  this.handleSetter('modifiers', 'SetModifiers', String, val);
});

MenuItem.prototype.__defineGetter__('checked', function() {
  if (this.type != 'checkbox')
    return undefined;

  return this.handleGetter('checked');
});

MenuItem.prototype.__defineSetter__('checked', function(val) {
  if (this.type != 'checkbox')
    throw new String("'checked' property is only available for checkbox");

  this.handleSetter('checked', 'SetChecked', Boolean, val);
});

MenuItem.prototype.__defineGetter__('enabled', function() {
  return this.handleGetter('enabled');
});

MenuItem.prototype.__defineSetter__('enabled', function(val) {
  this.handleSetter('enabled', 'SetEnabled', Boolean, val);
});

MenuItem.prototype.__defineGetter__('submenu', function() {
  return v8_util.getHiddenValue(this, 'submenu');
});

MenuItem.prototype.__defineSetter__('submenu', function(val) {
  if (v8_util.getConstructorName(val) != 'Menu')
    throw new String("'submenu' property requries a valid Menu");

  v8_util.setHiddenValue(this, 'submenu', val);
  nw.callObjectMethod(this, 'SetSubmenu', [ val.id ]);
});

MenuItem.prototype.handleEvent = function(ev) {
  if (ev == 'click') {
    // Automatically flag the 'checked' property.
    if (this.type == 'checkbox') {
      var option = v8_util.getHiddenValue(this, 'option');
      option.checked = !option.checked;
    }

    // Emit click handler
    if (typeof this.click == 'function')
      this.click();
  }

  // Emit generate event handler
  exports.Base.prototype.handleEvent.apply(this, arguments);
}

exports.MenuItem = MenuItem;
