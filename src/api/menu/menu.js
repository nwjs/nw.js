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
var EventEmitter = process.EventEmitter;


function Menu(option) {
  if (typeof option != 'object')
    option = { type: 'contextmenu' };

  if (option.type != 'contextmenu' && option.type != 'menubar')
    throw new String('Invalid menu type: ' + option.type);

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

if (require('os').platform() === 'darwin'){
  Menu.prototype.on = Menu.prototype.addListener = function(ev, callback) {
    if (ev == 'show') {
        nw.callObjectMethod(this, 'EnableShowEvent', [ true ]);
    }
    // Call parent.
    EventEmitter.prototype.addListener.apply(this, arguments);
  }

  Menu.prototype.removeListener = function(ev, callback) {
    // Call parent.
    EventEmitter.prototype.removeListener.apply(this, arguments);
    if (ev == 'show' && EventEmitter.listenerCount(this, 'show') === 0) {
        nw.callObjectMethod(this, 'EnableShowEvent', [ false ]);
    }
  }

  Menu.prototype.createMacBuiltin = function (app_name, options) {
    var appleMenu = new Menu(),
        options = options || {};

    appleMenu.append(new exports.MenuItem({
        label: nw.getNSStringFWithFixup("IDS_ABOUT_MAC", app_name),
        selector: "orderFrontStandardAboutPanel:"
    }));
    appleMenu.append(new exports.MenuItem({
        type: "separator"
    }));
    appleMenu.append(new exports.MenuItem({
        label: nw.getNSStringFWithFixup("IDS_HIDE_APP_MAC", app_name),
        selector: "hide:",
        key: "h"
    }));
    appleMenu.append(new exports.MenuItem({
        label: nw.getNSStringWithFixup("IDS_HIDE_OTHERS_MAC"),
        selector: "hideOtherApplications:",
        key: "h",
        modifiers: "cmd-alt"
    }));
    appleMenu.append(new exports.MenuItem({
        label: nw.getNSStringWithFixup("IDS_SHOW_ALL_MAC"),
        selector: "unhideAllApplications:",
    }));
    appleMenu.append(new exports.MenuItem({
        type: "separator"
    }));
    appleMenu.append(new exports.MenuItem({
        label: nw.getNSStringFWithFixup("IDS_EXIT_MAC", app_name),
        selector: "closeAllWindowsQuit:",
        key: "q"
    }));
    this.append(new exports.MenuItem({ label:'', submenu: appleMenu}));

    if (!options.hideEdit) {
      var editMenu = new Menu();
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_EDIT_UNDO_MAC"),
          selector: "undo:",
      key: "z"
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_EDIT_REDO_MAC"),
          selector: "redo:",
      key: "z",
      modifiers: "cmd-shift"
      }));
      editMenu.append(new exports.MenuItem({
          type: "separator"
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_CUT_MAC"),
          selector: "cut:",
      key: "x"
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_COPY_MAC"),
          selector: "copy:",
      key: "c"
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_PASTE_MAC"),
          selector: "paste:",
      key: "v"
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_EDIT_DELETE_MAC"),
          selector: "delete:",
      key: ""
      }));
      editMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_EDIT_SELECT_ALL_MAC"),
          selector: "selectAll:",
      key: "a"
      }));
      this.append(new exports.MenuItem({ label: nw.getNSStringWithFixup("IDS_EDIT_MENU_MAC"), 
                                         submenu: editMenu}));
    }

    if (!options.hideWindow) {
      var winMenu = new Menu();
      winMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_MINIMIZE_WINDOW_MAC"),
          selector: "performMiniaturize:",
      key: "m"
      }));
      winMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_CLOSE_WINDOW_MAC"),
          selector: "performClose:",
      key: "w"
      }));
      winMenu.append(new exports.MenuItem({
          type: "separator"
      }));
      winMenu.append(new exports.MenuItem({
          label: nw.getNSStringWithFixup("IDS_ALL_WINDOWS_FRONT_MAC"),
          selector: "arrangeInFront:",
      }));
      this.append(new exports.MenuItem({ label: nw.getNSStringWithFixup("IDS_WINDOW_MENU_MAC"), 
                                         submenu: winMenu}));
    }
  }
}
exports.Menu = Menu;
