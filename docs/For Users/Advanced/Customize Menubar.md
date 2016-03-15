# Customize Menubar
---

[TOC]

Different platforms has different meanings of window menu, following discussions will show you the differences and provide best practice to keep window menu working decently on all platforms.

## Create and Set Menubar

To create a menubar, you just need to specify `type: 'menubar'` in the option when you create the menu:

```javascript
var your_menu = new nw.Menu({ type: 'menubar' });
```

And make sure every item appended to the menubar has a submenu. A normal text-only menu item on menubar makes no sense on most platforms.

```javascript
var submenu = new nw.Menu();
submenu.append(new nw.MenuItem({ label: 'Item A' }));
submenu.append(new nw.MenuItem({ label: 'Item B' }));

// the menu item appended should have a submenu
your_menu.append(new nw.MenuItem({
  label: 'First Menu',
  submenu: submenu
}));
```

Then you can set the window menu by setting the `menu` property of the window:

```javascript
nw.Window.get().menu = your_menu;
```

See both [Menu](../../References/Menu.md) and [Window](../../References/Window.md) for details of the API.

## Platform Differences

### Windows & Linux

On Windows and Linux, the menubars behave exactly the same. Each window can have one menubar and they all reside bellow the titlebar.

### Mac OS X

!!! warning "Behavior Changed"
    The behavior of the feature is changed since 0.13.0. Please see [Migration Notes from 0.12 to 0.13](../Migration/From 0.12 to 0.13.md#menu).

On Mac, one app can only have one menu, which is called application menu, no matter how many windows the app may have. And many key shortcuts rely on the existence of application menu, such as *Quit*, *Close* and *Copy*.

By default, NW.js app will start with default menus, including *your-app-name*, *Edit* and *Window*. You can get the default menus using the [menu.createMacBuiltin](../../References/Menu.md#menucreatemacbuiltinappname-options-mac) method and customize it as you need:

```javascript
var mb = new nw.Menu({type:"menubar"});
mb.createMacBuiltin("your-app-name");
// append, insert or delete items of `mb` to customize your own menu
// then ...
nw.Window.get().menu = mb;
```

!!! note "Fix the Title of Application Menu"
    The first item of application menu shows *nwjs* instead of *your-app-name*. To fix it, you need to set the value of `CFBundleName` in all files of `nwjs.app/Contents/Resources/*.lproj/InfoPlist.strings` to `your-app-name` instead of `nwjs`.

## Best practice

As mentioned above, on Windows and Linux each window can have one menubar, while on Mac an app can have only one application menu. So generally you should set menu for only your main window and avoid using window menu when you may have multiple main windows.

And it may happens that you want to design different menus for different platforms, then you can use [process.platform](http://nodejs.org/api/process.html#process_process_platform) to get which platform you're in.