var EventEmitter = nw.require('events').EventEmitter;

var OPTION_INVALID = 'Invalid option.';
var OPTION_KEY_REQUIRED = "Shortcut requires 'key' to specify key combinations.";
var OPTION_KEY_INVALID = "Invalid 'key' format.";
var OPTION_ACTIVE_INVALID = "'active' must be a valid function.";
var OPTION_FAILED_INVALID = "'failed' must be a valid function.";
var ARUGMENT_NOT_SHORTCUT = "'shortcut' argument is not an instance of nw.Shortcut";
var UNABLE_REGISTER_HOTKEY = "Unable to register the hotkey";
var UNABLE_UNREGISTER_HOTKEY = "Unable to unregister the hotkey";

// Build alias maps of all acceptable key code for nw.Shortcut API
// The list is polled from http://www.w3.org/TR/DOM-Level-3-Events-code/. And 
// it also contains easy to type aliases. For example, you can use either 
// "ctrl+`" or "ctrl+backquote" to register a shortcut.
// It is also backward compatible with NW12.
var ALIAS_MAP = (function() {
  var map = {
    '`' : 'Backquote',
    '\\': 'Backslash',
    '[' : 'BracketLeft',
    ']' : 'BracketRight',
    ',' : 'Comma',
    '=' : 'Equal',
    '-' : 'Minus',
    '.' : 'Period',
    '\'': 'Quote',
    ';' : 'Semicolon',
    '/' : 'Slash',
    '\n': 'Enter',
    '\t': 'Tab',
    // backward compatible with NW12
    'medianexttrack': 'MediaTrackNext',
    'mediaprevtrack': 'MediaTrackPrevious'
  };

  // a~z, KeyA~KeyZ
  var aCode = 'a'.charCodeAt(0);
  var ACode = 'A'.charCodeAt(0);
  for(var i = 0; i < 26; i++) {
    var alpha = 'Key' + String.fromCharCode(ACode + i);
    map[String.fromCharCode(aCode + i)] = alpha;
    map[alpha.toLowerCase()] = alpha;
  }

  // 0~9, Digit0~Digit9, Numpad0~Numpad9
  for(var i = 0; i <= 9; i++) {
    var d = i.toString(10);
    var digit = 'Digit' + d;
    var numpad = 'Numpad' + d;
    map[d] = digit;
    map[digit.toLowerCase()] = digit;
    map[numpad.toLowerCase()] = numpad;
  }

  // F1~F24
  for(var i = 1; i <= 24; i++) {
    var f = i.toString(10);
    var func = 'F' + f;
    map[func.toLowerCase()] = func;
  }

  // (Arrow)Down, (Arrow)Left, (Arrow)Right, (Arrow)Up
  'Down|Left|Right|Up'.split('|').forEach(function(a) {
    var arrow = 'Arrow' + a;
    map[a.toLowerCase()] = arrow;
    map[arrow.toLowerCase()] = arrow;
  });

  // 
  var keys =  'Backquote|Backslash|Backspace|BracketLeft|BracketRight|Comma|Equal|IntlBackslash|IntlHash|IntlRo|IntlYen|Minus|Period|Quote|Semicolon|Slash'
           + '|Enter|Space|Tab'
           + '|Delete|End|Help|Home|Insert|PageDown|PageUp'
           + '|NumLock|NumpadAdd|NumpadBackspace|NumpadClear|NumpadClearEntry|NumpadComma|NumpadDecimal|NumpadDivide|NumpadEnter|NumpadEqual|NumpadHash|NumpadMemoryAdd|NumpadMemoryClear|NumpadMemoryRecall|NumpadMemoryStore|NumpadMemorySubtract|NumpadMultiply|NumpadParenLeft|NumpadParenRight|NumpadStar|NumpadSubtract'
           + '|Escape|PrintScreen|Pause|ScrollLock'
           + '|BrowserBack|BrowserFavorites|BrowserForward|BrowserHome|BrowserRefresh|BrowserSearch|BrowserStop|BrowserStop|BrowserStop|LaunchApp2|LaunchMail|MediaPlayPause|MediaSelect|MediaStop|MediaTrackNext|MediaTrackPrevious|Power|Sleep|VolumeDown|VolumeMute|VolumeUp|WakeUp';
  keys.split('|').forEach(function(k) {
    map[k.toLowerCase()] = k;
  });

  return map;
}());

// modifiers
var MODIFIERS_REG = /^ctrl|alt|shift|command$/i;

var nwShortcutBinding;
// Hook Sync API calls
apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  nwShortcutBinding = bindingsAPI.compiledApi;
  ['registerAccelerator', 'unregisterAccelerator'].forEach(function(nwSyncAPIName) {
    apiFunctions.setHandleRequest(nwSyncAPIName, function() {
      return bindingUtil.sendRequestSync('nw.Shortcut.' + nwSyncAPIName, $Array.from(arguments), undefined, undefined)[0];
    });
  });
});

var handlers = {};

function keyToAccelerator(key) {
  key = key.toString();
  var parts = key.split('+');
  var maybeKey = parts.pop();
  var maybeModifiers = parts;

  var modifiers = {
    alt: false,
    command: false,
    ctrl: false,
    shift: false
  };

  maybeKey = ALIAS_MAP[maybeKey.toLowerCase()];
  if (!maybeKey) {
    throw new TypeError(OPTION_KEY_INVALID);
  }
  if (!maybeModifiers.every(function(m) {
    return modifiers[m.toLowerCase()] = MODIFIERS_REG.test(m);
  })) {
    throw new TypeError(OPTION_KEY_INVALID);
  }

  return {key: maybeKey, modifiers: modifiers};
}

function normalizeLocal(accelerator) {
  var modifiers = accelerator.modifiers;
  return [modifiers.alt, modifiers.command, modifiers.ctrl, modifiers.shift, accelerator.key].join('-');
}

function getRegistryLocal(accelerator) {
  var localKey = normalizeLocal(accelerator);
  return handlers[localKey];
}

function registerLocal(shortcut) {
  var localKey = normalizeLocal(shortcut._accelerator);
  handlers[localKey] = shortcut;
}

function unregisterLocal(shortcut) {
  var localKey = normalizeLocal(shortcut._accelerator);
  delete handlers[localKey];
}

function Shortcut(option) {
  if (!(this instanceof Shortcut)) {
    return new Shortcut(option);
  }

  EventEmitter.apply(this, arguments);

  if (typeof option != 'object')
    throw new TypeError(OPTION_INVALID);

  if (!option.key)
    throw new TypeError(OPTION_KEY_REQUIRED);

  if (option.hasOwnProperty('active')) {
    if (typeof option.active != 'function')
      throw new TypeError(OPTION_ACTIVE_INVALID);
  }

  if (option.hasOwnProperty('failed')) {
    if (typeof option.failed != 'function')
      throw new TypeError(OPTION_FAILED_INVALID);
  }

  var self = this;

  this.on('active', function() {
    if (!self.active) return;
    if (typeof self.active != 'function')
      throw new TypeError(OPTION_ACTIVE_INVALID);
    self.active.apply(self, arguments);
  });

  this.on('failed', function() {
    if (!self.failed) return;
    if (typeof self.failed != 'function')
      throw new TypeError(OPTION_FAILED_INVALID);
    self.failed.apply(self, arguments);
  });

  this.key = option.key;
  this.active = option.active;
  this.failed = option.failed;

  this._accelerator = keyToAccelerator(option.key);
}

nw.require('util').inherits(Shortcut, EventEmitter);

Shortcut.registerGlobalHotKey = function(shortcut) {
  if (!(shortcut instanceof Shortcut)) {
    throw new TypeError(ARUGMENT_NOT_SHORTCUT);
  }

  if(nwShortcutBinding.registerAccelerator(shortcut._accelerator)) {
    registerLocal(shortcut);
  } else {
    shortcut.emit('failed', new Error(UNABLE_REGISTER_HOTKEY));
  }
};

Shortcut.unregisterGlobalHotKey = function(shortcut) {
  if (!(shortcut instanceof Shortcut)) {
    throw new TypeError(ARUGMENT_NOT_SHORTCUT);
  }

  var handler = getRegistryLocal(shortcut._accelerator);
  if(handler && nwShortcutBinding.unregisterAccelerator(shortcut._accelerator)) {
    unregisterLocal(shortcut);
  } else {
    shortcut.emit('failed', new Error(UNABLE_UNREGISTER_HOTKEY));
  }
};

nwShortcutBinding.onKeyPressed.addListener(function(accelerator) {
  var handler = getRegistryLocal(accelerator);
  if (handler) {
    handler.emit('active');
  }
});

