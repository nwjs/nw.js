var forEach = require('utils').forEach;
var nwNative = requireNative('nw_natives');
var messagingNatives = requireNative('messaging_natives');
var util = nw.require('util');
var EventEmitter = nw.require('events').EventEmitter;

var trayEvents = { objs: {}, clickEvent: {} };

trayEvents.clickEvent = bindingUtil.createCustomEvent("NWObjectTrayClick", false, false);
trayEvents.clickEvent.addListener(function(id) {
  var tray = trayEvents.objs[id];
  if (!tray)
    return;
  var args = Array.prototype.slice.call(arguments, 1);
  args.unshift('click');
  tray.emit.apply(tray, args);
});

function Tray(option) {
  if (!(this instanceof Tray)) {
    return new Tray(option);
  }
  EventEmitter.apply(this);

  if (typeof option != 'object')
    throw new TypeError('Invalid option.');

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
  
  var id = nw.Obj.allocateId();
  this.id = id;
  privates(this).option = option;

  // All properties must be set after initialization.
  if (!option.hasOwnProperty('icon'))
    option.shadowIcon = '';
  if (!option.hasOwnProperty('alticon'))
    option.shadowAlticon = '';
  if (!option.hasOwnProperty('tooltip'))
    option.tooltip = '';
  
  nw.Obj.create(id, 'Tray', option);
  messagingNatives.BindToGC(this, nw.Obj.destroy.bind(undefined, id));
  trayEvents.objs[this.id] = this;
}

util.inherits(Tray, EventEmitter);

Tray.prototype.handleGetter = function(name) {
  return privates(this).option[name];
};

Tray.prototype.handleSetter = function(name, setter, type, value) {
  value = type(value);
  privates(this).option[name] = value;
  nw.Obj.callObjectMethod(this.id, 'Tray', setter, [ value ]);
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
  nw.Obj.callObjectMethod(this.id, 'Tray', 'SetMenu', [ val.id ]);
});

Tray.prototype.remove = function() {
  nw.Obj.callObjectMethod(this.id, 'Tray', 'Remove', []);
  delete trayEvents.objs[this.id];
}

exports.binding = Tray;

