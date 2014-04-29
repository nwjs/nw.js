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

function Menu(option) {
  if (typeof option != 'object')
    option = { type: 'contextmenu' };

  if (option.type != 'contextmenu' && option.type != 'menubar')
    throw new String('Invalid menu type: ' + option.type);

  if (option.hasOwnProperty('update')) {
      if (typeof option.update != 'function')
        throw new String("'update' must be a valid Function");
      else
        this.update = option.update;
    }

  this.type = option.type;
  v8_util.setHiddenValue(this, 'items', []);
  nw.allocateObject(this, option);
}
require('util').inherits(Menu, exports.Base);

Menu.prototype.__defineGetter__('items', function() {
  return v8_util.getHiddenValue(this, 'items');
});

Menu.prototype.__defineSetter__('items', function(val) {
  throw new String('Menu.items is immutable');
});

Menu.prototype.append = function(menu_item) {
  if (v8_util.getConstructorName(menu_item) != 'MenuItem')
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
  nw.callObjectMethod(this, 'Remove', [ menu_item.id, pos_hint ]);
  this.items.splice(pos_hint, 1);
}

Menu.prototype.removeAt = function(i) {
  nw.callObjectMethod(this, 'Remove', [ this.items[i].id, i ]);
  this.items.splice(i, 1);
}

Menu.prototype.popup = function(x, y) {
  nw.callObjectMethod(this, 'Popup', [ x, y ]);
}

Menu.prototype.handleEvent = function(ev) {
  if (ev == 'update') {
    // Emit update handler
    if (typeof this.update == 'function')
      this.update();
  }

  // Emit generate event handler
  exports.Base.prototype.handleEvent.apply(this, arguments);
}

exports.Menu = Menu;
