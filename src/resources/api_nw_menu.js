var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Menu');
var nwNative = requireNative('nw_natives');
var sendRequest = require('sendRequest');
var contextMenuNatives = requireNative('context_menus');
var messagingNatives = requireNative('messaging_natives');
var Event = require('event_bindings').Event;

var menuItems = { objs : {}, clickEvent: {} };

var Menu = function(id, option) {
  if (option.type != 'contextmenu' && option.type != 'menubar')
    throw new TypeError('Invalid menu type: ' + option.type);

  this.id = id;
  this.type = option.type;
  privates(this).items = [];
  privates(this).option = option;
};

Menu.prototype.__defineGetter__('items', function() {
  return privates(this).items;
});

Menu.prototype.__defineSetter__('items', function(val) {
  throw new Error('Menu.items is immutable');
});

Menu.prototype.append = function(menu_item) {
  privates(this).items.push(menu_item);
  nw.Object.callObjectMethod(this.id, 'Menu', 'Append', [ menu_item.id ]);
};

Menu.prototype.insert = function(menu_item, i) {
  privates(this).items.splice(i, 0, menu_item);
  nw.Object.callObjectMethod(this.id, 'Menu', 'Insert', [ menu_item.id, i ]);
}

Menu.prototype.remove = function(menu_item) {
  var pos_hint = privates(this).items.indexOf(menu_item);
  nw.Object.callObjectMethod(this.id, 'Menu', 'Remove', [ menu_item.id, pos_hint ]);
  privates(this).items.splice(pos_hint, 1);
}

Menu.prototype.removeAt = function(i) {
  nw.Object.callObjectMethod(this.id, 'Menu', 'Remove', [ privates(this).items[i].id, i ]);
  privates(this).items.splice(i, 1);
}

Menu.prototype.popup = function(x, y) {
  nw.Object.callObjectMethod(this.id, 'Menu', 'Popup', [ x, y ]);
}

Menu.prototype.createMacBuiltin = function (app_name, options) {
  var appleMenu = nw.Menu.createMenu({type:'menubar'}),
  options = options || {};
  
  appleMenu.append(nw.Menu.createItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_ABOUT_MAC", app_name),
    selector: "orderFrontStandardAboutPanel:"
  }));
  appleMenu.append(nw.Menu.createItem({
    type: "separator"
  }));
  appleMenu.append(nw.Menu.createItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_HIDE_APP_MAC", app_name),
    selector: "hide:",
    modifiers: "cmd",
    key: "h"
  }));
  appleMenu.append(nw.Menu.createItem({
    label: nw.Menu.getNSStringWithFixup("IDS_HIDE_OTHERS_MAC"),
    selector: "hideOtherApplications:",
    key: "h",
    modifiers: "cmd-alt"
  }));
  appleMenu.append(nw.Menu.createItem({
    label: nw.Menu.getNSStringWithFixup("IDS_SHOW_ALL_MAC"),
    selector: "unhideAllApplications:",
  }));
  appleMenu.append(nw.Menu.createItem({
    type: "separator"
  }));
  appleMenu.append(nw.Menu.createItem({
    label: nw.Menu.getNSStringFWithFixup("IDS_EXIT_MAC", app_name),
    selector: "closeAllWindowsQuit:",
    modifiers: "cmd",
    key: "q"
  }));
  this.append(nw.Menu.createItem({ label:'', submenu: appleMenu}));
  
  if (!options.hideEdit) {
    var editMenu = nw.Menu.createMenu({type:'menubar'});
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_UNDO_MAC"),
      selector: "undo:",
      modifiers: "cmd",
      key: "z"
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_REDO_MAC"),
      selector: "redo:",
      key: "z",
      modifiers: "cmd-shift"
    }));
    editMenu.append(nw.Menu.createItem({
      type: "separator"
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_CUT_MAC"),
      selector: "cut:",
      modifiers: "cmd",
      key: "x"
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_COPY_MAC"),
      selector: "copy:",
      modifiers: "cmd",
      key: "c"
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_PASTE_MAC"),
      selector: "paste:",
      modifiers: "cmd",
      key: "v"
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_DELETE_MAC"),
      selector: "delete:",
      key: ""
    }));
    editMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_EDIT_SELECT_ALL_MAC"),
      selector: "selectAll:",
      modifiers: "cmd",
      key: "a"
    }));
    this.append(nw.Menu.createItem({ label: nw.Menu.getNSStringWithFixup("IDS_EDIT_MENU_MAC"),
      submenu: editMenu}));
  }
  
  if (!options.hideWindow) {
    var winMenu = nw.Menu.createMenu({type:'menubar'});
    winMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_MINIMIZE_WINDOW_MAC"),
      selector: "performMiniaturize:",
      modifiers: "cmd",
      key: "m"
    }));
    winMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_CLOSE_WINDOW_MAC"),
      selector: "performClose:",
      modifiers: "cmd",
      key: "w"
    }));
    winMenu.append(nw.Menu.createItem({
      type: "separator"
    }));
    winMenu.append(nw.Menu.createItem({
      label: nw.Menu.getNSStringWithFixup("IDS_ALL_WINDOWS_FRONT_MAC"),
      selector: "arrangeInFront:",
    }));
    this.append(nw.Menu.createItem({ label: nw.Menu.getNSStringWithFixup("IDS_WINDOW_MENU_MAC"),
      submenu: winMenu}));
  }
}


function MenuItem(id, option) {
  if (typeof option != 'object')
    throw new TypeError('Invalid option.');

  if (!option.hasOwnProperty('type'))
    option.type = 'normal';

  if (option.type != 'normal' &&
      option.type != 'checkbox' &&
      option.type != 'separator')
    throw new TypeError('Invalid MenuItem type: ' + option.type);

  if (option.type == 'normal' || option.type == 'checkbox') {
    if (option.type == 'checkbox')
      option.checked = Boolean(option.checked);

    if (!option.hasOwnProperty('label'))
      throw new TypeError('A normal MenuItem must have a label');
    else
      option.label = String(option.label);

    if (option.hasOwnProperty('icon')) {
      option.shadowIcon = String(option.icon);
      option.icon = nwNative.getAbsolutePath(option.icon);
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
      // Transfer only object id
      privates(this).submenu = option.submenu;
      option.submenu = option.submenu.id;
    }

    if (option.hasOwnProperty('click')) {
      if (typeof option.click != 'function')
        throw new TypeError("'click' must be a valid Function");
      else
        this.click = option.click;
    }
  } else if (option.type == 'separator') {
    option = {
      type: 'separator'
    };
  }

  this.id = id;
  privates(this).option = option;

  menuItems.objs[id] = this;
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

MenuItem.prototype.handleGetter = function(name) {
  return privates(this).option[name];
};

MenuItem.prototype.handleSetter = function(name, setter, type, value) {
  value = type(value);
  privates(this).option[name] = value;
  nw.Object.callObjectMethod(this.id, 'MenuItem', setter, [ value ]);
};

MenuItem.prototype.__defineGetter__('type', function() {
  return this.handleGetter('type');
});

MenuItem.prototype.__defineSetter__('type', function() {
  throw new Error("'type' is immutable at runtime");
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
  privates(this).option.shadowIcon = String(val);
  var real_path = val == '' ? '' : nwNative.getAbsolutePath(val); //FIXME
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
    throw new TypeError("'checked' property is only available for checkbox");

  this.handleSetter('checked', 'SetChecked', Boolean, val);
});

MenuItem.prototype.__defineGetter__('enabled', function() {
  return this.handleGetter('enabled');
});

MenuItem.prototype.__defineSetter__('enabled', function(val) {
  this.handleSetter('enabled', 'SetEnabled', Boolean, val);
});

MenuItem.prototype.__defineGetter__('submenu', function() {
  return privates(this).submenu;
});

MenuItem.prototype.__defineSetter__('submenu', function(val) {
  privates(this).submenu = val;
  nw.Object.callObjectMethod(this.id, 'MenuItem', 'SetSubmenu', [ val.id ]);
});

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  menuItems.clickEvent = new Event("NWObjectclick");
  menuItems.clickEvent.addListener(function(id) {
    if (!menuItems.objs[id])
      return;
    menuItems.objs[id].click();
  });
  apiFunctions.setHandleRequest('createItem', function(option) {
    var id = contextMenuNatives.GetNextContextMenuId();
    option.generatedId = id;
    var ret = new MenuItem(id, option);
    sendRequest.sendRequestSync('nw.Menu.createItem', [option], this.definition.parameters, {});
    messagingNatives.BindToGC(ret, nw.Menu.destroy.bind(undefined, id), -1);
    return ret;
  });
  apiFunctions.setHandleRequest('destroy', function(id) {
    sendRequest.sendRequestSync('nw.Object.destroy', [id], this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('createMenu', function(option) {
    var id = contextMenuNatives.GetNextContextMenuId();
    if (typeof option != 'object' || !option)
      option = { type: 'contextmenu' };

    option.generatedId = id;
    var ret = new Menu(id, option);
    sendRequest.sendRequestSync('nw.Object.create', [id, 'Menu', option], this.definition.parameters, {});
    messagingNatives.BindToGC(ret, nw.Menu.destroy.bind(undefined, id), -1);
    return ret;
  });
  apiFunctions.setHandleRequest('getNSStringWithFixup', function(msg) {
    return sendRequest.sendRequestSync('nw.Menu.getNSStringWithFixup', [msg], this.definition.parameters, {})[0];
  });
  apiFunctions.setHandleRequest('getNSStringFWithFixup', function(msg, appName) {
    return sendRequest.sendRequestSync('nw.Menu.getNSStringFWithFixup', [msg, appName], this.definition.parameters, {})[0];
  });
});

exports.binding = nw_binding.generate();

