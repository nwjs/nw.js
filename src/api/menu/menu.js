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

function Menu(option) {
  if (typeof option != 'object')
    throw new String('Invalid option.');

  if (!option.hasOwnProperty('title'))
    throw new String("'title' field is required");
  else
    option.title = String(option.title);

  this.setHiddenValue('option', option);
  this.setHiddenValue('items', []);
  nw.allocateObject(this, option);
}
nw.inherits(Menu, exports.Base);

Menu.prototype.__defineGetter__('title', function() {
  return this.handleGetter('title');
});

Menu.prototype.__defineSetter__('title', function(val) {
  this.handleSetter('title', 'SetTitle', String, val);
});

Menu.prototype.__defineGetter__('items', function() {
  return this.getHiddenValue('items');
});

Menu.prototype.__defineSetter__('items', function(val) {
  throw new String('Menu.items is immutable');
});

Menu.prototype.append = function(menu_item) {
  if (this.getConstructorName.call(menu_item) != 'MenuItem')
    throw new String("Menu.append() requires a valid MenuItem");
    
  this.items.push(menu_item);
  nw.callObjectMethod(this, 'Append', [ menu_item.id ]);
};

Menu.prototype.insert = function(menu_item, i) {
  this.items.splice(i, 0, menu_item);
  nw.callObjectMethod(this, 'Insert', [ menu_item.id, i ]);
}

Menu.prototype.remove = function(menu_item) {
  var pos_hint = this.items.indexOf(menu_item);
  this.items.splice(pos_hint, 1);
  nw.callObjectMethod(this, 'Remove', [ menu_item.id, pos_hint ]);
}

Menu.prototype.removeAt = function(i) {
  this.items.splice(i, 1);
  nw.callObjectMethod(this, 'RemoveAt', [ i ]);
}

Menu.prototype.popup = function(x, y) {
  nw.callObjectMethod(this, 'Popup', [ x, y ]);
}

exports.Menu = Menu;
