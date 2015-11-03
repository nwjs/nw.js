var Binding = require('binding').Binding;
var forEach = require('utils').forEach;
var nw_binding = require('binding').Binding.create('nw.Tray');
var nwNative = requireNative('nw_natives');
var sendRequest = require('sendRequest');
var contextMenuNatives = requireNative('context_menus');
var messagingNatives = requireNative('messaging_natives');
var Event = require('event_bindings').Event;

var trayEvents = { objs: {}, clickEvent: {} };

function Tray(id, option) {
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
    option.icon = nwNative.getAbsolutePath(option.icon);
  }

  if (option.hasOwnProperty('alticon')) {
    option.shadowAlticon = String(option.alticon);
    option.alticon = nwNative.getAbsolutePath(option.alticon);
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
    if (option.menu.constructor.name != 'Menu')
      throw new TypeError("'menu' must be a valid Menu");

    // Transfer only object id
    privates(this).menu = option.menu;
    option.menu = option.menu.id;
  }
  
  this.id = id;
  privates(this).option = option;

  // All properties must be set after initialization.
  if (!option.hasOwnProperty('icon'))
    option.shadowIcon = '';
  if (!option.hasOwnProperty('alticon'))
    option.shadowAlticon = '';
  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';
}

Tray.prototype.handleGetter = function(name) {
  return privates(this).option[name];
};

Tray.prototype.handleSetter = function(name, setter, type, value) {
  value = type(value);
  privates(this).option[name] = value;
  nw.Object.callObjectMethod(this.id, 'Tray', setter, [ value ]);
};

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
  privates(this).option.shadowIcon = String(val);
  var real_path = val == '' ? '' : nwNative.getAbsolutePath(val);
  this.handleSetter('icon', 'SetIcon', String, real_path);
});

Tray.prototype.__defineSetter__('alticon', function(val) {
  privates(this).option.shadowAlticon = String(val);
  var real_path = val == '' ? '' : nwNative.getAbsolutePath(val);
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
  return privates(this).menu;
});

Tray.prototype.__defineSetter__('menu', function(val) {
  if (val.constructor.name != 'Menu')
    throw new TypeError("'menu' property requries a valid Menu");

  privates(this).menu = val;
  nw.Object.callObjectMethod(this.id, 'Tray', 'SetMenu', [ val.id ]);
});

Tray.prototype.remove = function() {
  if (trayEvents.objs[this.id])
    this.removeListener('click');
  nw.Object.callObjectMethod(this.id, 'Tray', 'Remove', []);
}

Tray.prototype.on = function (event, callback) {
  if (event == 'click') {
    trayEvents.objs[this.id] = this;
    this._onclick = callback;
  }
}

Tray.prototype.removeListener = function (event) {
  if (event == 'click') {
    delete trayEvents.objs[this.id];
    delete this._onclick;
  }
}

nw_binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  trayEvents.clickEvent = new Event("NWObjectTrayClick");
  trayEvents.clickEvent.addListener(function(id) {
    if (!trayEvents.objs[id])
      return;
    trayEvents.objs[id]._onclick();
  });
  apiFunctions.setHandleRequest('destroy', function(id) {
    sendRequest.sendRequestSync('nw.Object.destroy', [id], this.definition.parameters, {});
  });
  apiFunctions.setHandleRequest('create', function(option) {
    var id = contextMenuNatives.GetNextContextMenuId();
    if (typeof option != 'object' || !option)
      option = { };

    option.generatedId = id;
    var ret = new Tray(id, option);
    sendRequest.sendRequestSync('nw.Object.create', [id, 'Tray', option], this.definition.parameters, {});
    messagingNatives.BindToGC(ret, nw.Tray.destroy.bind(undefined, id), -1);
    return ret;
  });
});

exports.binding = nw_binding.generate();

