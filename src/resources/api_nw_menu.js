var forEach = require('utils').forEach;
var nwNative = requireNative('nw_natives');
var messagingNatives = requireNative('messaging_natives');

function Menu (option) {
  if (!(this instanceof Menu)) {
    return new Menu(option);
  }

  if (typeof option != 'object' || !option)
    option = { type: 'contextmenu' };
  if (option.type != 'contextmenu' && option.type != 'menubar')
    throw new TypeError('Invalid menu type: ' + option.type);

  var id = nw.Obj.allocateId();
  option.generatedId = id;

  this.id = id;
  this.type = option.type;
  privates(this).items = [];
  privates(this).option = option;

  var items = privates(this).items;
  nw.Obj.create(id, 'Menu', option);
  messagingNatives.BindToGC(this, function() { items.forEach(function(element) { element._destroy(); }); nw.Obj.destroy(id); }, -1);
};

Menu.prototype.__defineGetter__('items', function() {
  return privates(this).items;
});

Menu.prototype.__defineSetter__('items', function(val) {
  throw new Error('Menu.items is immutable');
});

Menu.prototype.append = function(menu_item) {
  const isSeparator = menu_item.type === 'separator'
  const items = privates(this).items
  
  // https://github.com/nwjs/chromium.src/blob/nw45-log/ui/base/models/simple_menu_model.h#L117
  if (isSeparator && (items.length === 0 || items[items.length - 1].type === 'separator')) {
    return menu_item._destroy()
  }

  items.push(menu_item);
  nw.MenuItem.eventRegistry.add(menu_item);
  if (!menu_item.native)
    nw.Obj.callObjectMethod(this.id, 'Menu', 'Append', [ menu_item.id ]);
};

Menu.prototype.insert = function(menu_item, i) {
  privates(this).items.splice(i, 0, menu_item);
  nw.MenuItem.eventRegistry.add(menu_item);
  if (!menu_item.native)
    nw.Obj.callObjectMethod(this.id, 'Menu', 'Insert', [ menu_item.id, i ]);
}

Menu.prototype.remove = function(menu_item) {
  var pos_hint = privates(this).items.indexOf(menu_item);
  nw.Obj.callObjectMethod(this.id, 'Menu', 'Remove', [ menu_item.id, pos_hint ]);
  privates(this).items.splice(pos_hint, 1);
  nw.MenuItem.eventRegistry.remove(menu_item);
}

Menu.prototype.removeAt = function(i) {
  const items = privates(this).items;
  nw.Obj.callObjectMethod(this.id, 'Menu', 'Remove', [ items[i].id, i ]);
  nw.MenuItem.eventRegistry.remove(items[i]);
  items.splice(i, 1);
}

Menu.prototype.popup = function(x, y) {
  nw.Obj.callObjectMethodAsync(this.id, 'Menu', 'Popup', [ x, y ]);
}

Menu.prototype.createMacBuiltin = function (app_name, options) {
  var appleMenu = new nw.Menu({type:'menubar'}),
  options = options || {};

  appleMenu.append(new nw.MenuItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_ABOUT_MAC", app_name),
    selector: "orderFrontStandardAboutPanel:"
  }));
  appleMenu.append(new nw.MenuItem({
    type: "separator"
  }));
  appleMenu.append(new nw.MenuItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_HIDE_APP_MAC", app_name),
    selector: "hide:",
    modifiers: "cmd",
    key: "h"
  }));
  appleMenu.append(new nw.MenuItem({
    label: nw.Menu.getNSStringWithFixup("IDS_HIDE_OTHERS_MAC"),
    selector: "hideOtherApplications:",
    key: "h",
    modifiers: "cmd-alt"
  }));
  appleMenu.append(new nw.MenuItem({
    label: nw.Menu.getNSStringWithFixup("IDS_SHOW_ALL_MAC"),
    selector: "unhideAllApplications:",
  }));
  appleMenu.append(new nw.MenuItem({
    type: "separator"
  }));
  appleMenu.append(new nw.MenuItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_EXIT_MAC", app_name),
    selector: "closeAllWindowsQuit:",
    modifiers: "cmd",
    key: "q"
  }));
  this.append(new nw.MenuItem({ label:'', submenu: appleMenu}));

  if (!options.hideEdit) {
    var editMenu = new nw.Menu({type:'menubar'});
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_UNDO_MAC"),
      selector: "undo:",
      modifiers: "cmd",
      key: "z"
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_REDO_MAC"),
      selector: "redo:",
      key: "z",
      modifiers: "cmd-shift"
    }));
    editMenu.append(new nw.MenuItem({
      type: "separator"
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_CUT_MAC"),
      selector: "cut:",
      modifiers: "cmd",
      key: "x"
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_COPY_MAC"),
      selector: "copy:",
      modifiers: "cmd",
      key: "c"
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_PASTE_MAC"),
      selector: "paste:",
      modifiers: "cmd",
      key: "v"
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_DELETE_MAC"),
      selector: "delete:",
      key: ""
    }));
    editMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_SELECT_ALL_MAC"),
      selector: "selectAll:",
      modifiers: "cmd",
      key: "a"
    }));
    this.append(new nw.MenuItem({ label: nw.Menu.getNSStringWithFixup("IDS_EDIT_MENU_MAC"),
      submenu: editMenu}));
  }

  if (!options.hideWindow) {
    var winMenu = new nw.Menu({type:'menubar'});
    winMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_MINIMIZE_WINDOW_MAC"),
      selector: "performMiniaturize:",
      modifiers: "cmd",
      key: "m"
    }));
    winMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_CLOSE_WINDOW_MAC"),
      selector: "performClose:",
      modifiers: "cmd",
      key: "w"
    }));
    winMenu.append(new nw.MenuItem({
      type: "separator"
    }));
    winMenu.append(new nw.MenuItem({
      label: nw.Menu.getNSStringWithFixup("IDS_ALL_WINDOWS_FRONT_MAC"),
      selector: "arrangeInFront:",
    }));
    this.append(new nw.MenuItem({ label: nw.Menu.getNSStringWithFixup("IDS_WINDOW_MENU_MAC"),
      submenu: winMenu}));
  }
}

var nwMenuBinding;
apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  nwMenuBinding = bindingsAPI.compiledApi;
  apiFunctions.setHandleRequest('getNSStringWithFixup', function(msg) {
    return bindingUtil.sendRequestSync('nw.Menu.getNSStringWithFixup', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('getNSStringFWithFixup', function(msg, appName) {
    return bindingUtil.sendRequestSync('nw.Menu.getNSStringFWithFixup', $Array.from(arguments), undefined, undefined)[0];
  });
});

Menu.getNSStringWithFixup = nwMenuBinding.getNSStringWithFixup;
Menu.getNSStringFWithFixup = nwMenuBinding.getNSStringFWithFixup;

exports.binding = Menu;
