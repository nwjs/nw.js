var forEach = require('utils').forEach;
var nwNative = requireNative('nw_natives');
var messagingNatives = requireNative('messaging_natives');
var util = nw.require('util');
var EventEmitter = nw.require('events').EventEmitter;

var menuItems = { objs : {}, clickEvent: {} };
menuItems.clickEvent = bindingUtil.createCustomEvent("NWObjectclick", undefined, false, false);
menuItems.clickEvent.addListener(function(id) {
  var obj = menuItems.objs[id];
  if (!obj)
    return;
  try{obj.click && obj.click()}catch(e){console.error(e)}
  try{obj.emit('click')}catch(e){console.error(e)}
});

function MenuItem(option) {
  if (!(this instanceof MenuItem)) {
    return new MenuItem(option);
  }
  EventEmitter.apply(this);

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

  var id = option.id || nw.Obj.allocateId();
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

  if (!option.native)
    nw.Obj.create(id, 'MenuItem', option);
  messagingNatives.BindToGC(this, function() { nw.Obj.destroy(id); }, -1);

}

MenuItem.prototype._destroy = function () {
  menuItems.objs[this.id] = null;
};

util.inherits(MenuItem, EventEmitter);

MenuItem.prototype.handleGetter = function(name) {
  return privates(this).option[name];
};

MenuItem.prototype.handleSetter = function(name, setter, type, value) {
  value = type(value);
  privates(this).option[name] = value;
  nw.Obj.callObjectMethod(this.id, 'MenuItem', setter, [ value ]);
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

  return nw.Obj.callObjectMethodSync(this.id, 'MenuItem', 'GetChecked', []);
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
  nw.Obj.callObjectMethod(this.id, 'MenuItem', 'SetSubmenu', [ val.id ]);
});

MenuItem.prototype.__defineGetter__('native', function() {
  return this.handleGetter('native');
});

exports.binding = MenuItem;