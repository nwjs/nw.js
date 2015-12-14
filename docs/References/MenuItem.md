# MenuItem {: .doctitle}
---

!!! important "Available"
    Since 0.3.0

[TOC]

`MenuItem` represents an item in a menu. A `MenuItem` can be a separator or a normal item which has label and icon or a checkbox. It can response to mouse click or keyboard shortcut.

## Synopsis

```javascript
var item;

// Create a separator
item = new nw.MenuItem({ type: 'separator' });

// Create a normal item with label and icon
item = new nw.MenuItem({
  type: "normal", 
  label: "I'm a menu item",
  icon: "img/icon.png"
});

// Or you can omit the 'type' field for normal items
item = new nw.MenuItem({ label: 'Simple item' });

// Bind a callback to item
item = new nw.MenuItem({
  label: "Click me",
  click: function() {
    console.log("I'm clicked");
  },
  key: "s",
  modifiers: "ctrl+alt",
});

// You can have submenu!
var submenu = new nw.Menu();
submenu.append(new nw.MenuItem({ label: 'Item 1' }));
submenu.append(new nw.MenuItem({ label: 'Item 2' }));
submenu.append(new nw.MenuItem({ label: 'Item 3' }));
item.submenu = submenu;

// And everything can be changed at runtime
item.label = 'New label';
item.click = function() { console.log('New click callback'); };
```

## new MenuItem(option)

!!! important "Available"
    Since 0.3.0

* `option` `{Object}` an object contains initial settings for the `MenuItem`
    - `label` `{String}` _Optional_ label for normal item or checkbox
    - `icon` `{String}` _Optional_ icon for normal item or checkbox
    - `tooltip` `{String}` _Optional_ tooltip for normal item or checkbox
    - `type` `{String}` _Optional_ the type of the item. Three types are accepted: `normal`, `checkbox`, `separator`
    - `click` `{Function}` _Optional_ the callback function when item is triggered by mouse click or keyboard shortcut
    - `enabled` `{Boolean}` _Optional_ whether the item is enabled or disabled. It's set to `true` by default.
    - `checked` `{Boolean}` _Optional_ whether the checkbox is checked or not. It's set to `false` by default.
    - `submenu` `{Menu}` _Optional_ a submenu
    - `key` `{String}` _Optional_ the key of the shortcut
    - `modifiers` `{String}` _Optional_ the modifiers of the shortcut

Every field has its own property in the `MenuItem`, see documentation of each property for details.

`MenuItem` is inherited from `EventEmitter`. You can use `on` to listen to the events.

## item.type

!!! important "Available"
    Since 0.3.0

Get the `type` of a `MenuItem`, it can be `separator`, `checkbox` and `normal`.

!!! note
    The type can be set only when you create it. It cannot be changed at runtime.

## item.label

!!! important "Available"
    Since 0.3.0

Get or set the `label` of a `MenuItem`, it can only be plain text for now.

## item.icon

!!! important "Available"
    Since 0.3.0

Get or set the `icon` of a `MenuItem`, it must a path to your icon file. It can be a relative path which points to an icon in your app, or an absolute path pointing to a file in user's system.

It has no effect on setting `icon` of a `separator` item.

## item.iconIsTemplate (Mac)

!!! important "Available"
    Since 0.11.3

Get or set whether `icon` image is treated as "template" (`true` by default). When the property is set to `true` the image is treated as "template" and the system automatically ensures proper styling according to the various states of the status item (e.g. dark menu, light menu, etc.). Template images should consist only of black and clear colours and can use the alpha channel in the image to adjust the opacity of black content. It has no effects on Linux and Windows. For further information see [#2775](https://github.com/rogerwang/node-webkit/pull/2775).

## item.tooltip (Mac)

!!! important "Available"
    Since 0.3.0

Get or set the `tooltip` of a `MenuItem`, it can only be plain text. A `tooltip` is short string that describes the menu item, it will show when you hover your mouse on the item.

## item.checked

!!! important "Available"
    Since 0.3.0

Get or set whether the `MenuItem` is `checked`. Usually if a `MenuItem` is checked. There will be a mark on the left side of it. It's used mostly to indicate whether an option is on.

## item.enabled

!!! important "Available"
    Since 0.3.0

Get or set whether the `MenuItem` is `enabled`. An disabled `MenuItem` will be greyed out and you will not be able to click on it.

## item.submenu

!!! important "Available"
    Since 0.3.0

Get or set the `submenu` of a `MenuItem`, the `submenu` must be a `Menu` object. You should set `submenu` in the `option` when creating the `MenuItem`. Changing it at runtime is slow on some platforms.

## item.click

!!! important "Available"
    Since 0.3.0

Get or set the `click` callback of a `MenuItem`, the `click` must be a valid function. It will be called when users activate the item.

## item.key

!!! important "Available"
    Since 0.10.0

A single character string to specify the shortcut key for the menu item.

List of valid keys:

* Alphabet: `a`-`z`
* Digits: `0`-`9`
* Other keys on main area: `[` , `]` , `'` , `,` , `.` , `/` , `` ` `` , `-` , `=` , `\` , `'` , `;` , `Tab`
* `Esc`
* `Down` , `Up` , `Left` , `Right`

On Mac, you can use special keys as shortcut key:
```javascript
key = String.fromCharCode(28); // arrow left
key = String.fromCharCode(29); // arrow right
key = String.fromCharCode(30); // arrow up
key = String.fromCharCode(31); // arrow down
key = String.fromCharCode(27); // escape
key = String.fromCharCode(11); // Page up
key = String.fromCharCode(12); // Page down
```
For full list of special keys supported on Mac, see [NSMenuItem.keyEquivalent](https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSMenuItem_Class/#//apple_ref/occ/instp/NSMenuItem/keyEquivalent) and [NSEvent: Function-Key Unicodes](https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSEvent_Class/index.html#//apple_ref/doc/constant_group/Function_Key_Unicodes).

Since 0.13.0, you can use key names defined in [W3C DOM Level 3 KeyboardEvent Key Values](http://www.w3.org/TR/DOM-Level-3-Events-key/), such as `KeyA` (same as `A`), `Escape` (same as `Esc`), `F1`, `ArrowDown` (same as `Down`) etc.

## item.modifiers

!!! important "Available"
    Since 0.10.0

A string to specify the modifier keys for the shortcut of the menu item. It should be the concatenation of the following strings: `cmd` / `command` / `super`, `shift`, `ctrl`, `alt`. e.g. `"cmd+shift+alt"`.

!!! warning "Behavior Changed"
    For version < 0.13.0, `cmd` is only valid for Mac OS X which represents the Apple key (<kbd>&#8984;</kbd>). And `super` is only valid for Windows, which represents the Windows key (<kbd>Windows</kbd>).

    Since 0.13.0, `cmd` represents different keys on all platforms: Windows key (<kbd>Windows</kbd>) on Windows and Linux, Apple key (<kbd>&#8984;</kbd>) on Mac. `super` and `command` are aliases of `cmd`.

## Event: click

Emitted when user activates the menu item.
