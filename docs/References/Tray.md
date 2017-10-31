# Tray {: doctitle}
---

[TOC]

`Tray` is an abstraction of different controls on different platforms, usually it's a small icon shown on the OS's notification area. On Mac OS X it's called `Status Item`, on GTK it's `Status Icon`, and on Windows it's `System Tray Icon`.

## Synopsis

```javascript
// Create a tray icon
var tray = new nw.Tray({ title: 'Tray', icon: 'img/icon.png' });

// Give it a menu
var menu = new nw.Menu();
menu.append(new nw.MenuItem({ type: 'checkbox', label: 'box1' }));
tray.menu = menu;

// Remove the tray
tray.remove();
tray = null;
```

!!! warning "Using Tray With Page Navigation"
    Trays created in the page that can be navigated will not be functional after a reload or navigation. The reason is that the tray and even the web page will be garbage collected by JS engine after navigation to prevent memory leak. So it's recommended to use trays in **background page**, which is existed for the life cycle of your app. See [`bg-script`](Manifest Format.md#bg-script) and [`main`](Manifest Format.md#main) for how to execute scripts in the background page.


## new Tray(option)

* `option` `{Object}`
    - `title` `{String}` title
    - `tooltip` `{String}` tooltip
    - `icon` `{String}` icon
    - `alticon` `{String}` alternate
    - `iconsAreTemplates` `{Boolean}` whether icons are templates
    - `menu` `{Menu}` popup menu

Create a new `Tray`, `option` is an object contains initial settings for the `Tray`. Every field has its own property in the `Tray`, see documentation of each property for details.

## tray.title

Get or set the `title` of the tray.

On Mac OS X `title` will be showed on status bar along with its `icon`, but it doesn't have effects on GTK and Windows, since the latter two platforms only support tray to be showed as icons.

## tray.tooltip

Get or set the `tooltip` of the tray. `tooltip` shows when you hover the `Tray` with mouse.

!!! note
    `tooltip` is showed on all three platforms. Should be set as `Tray` property rather from `option` object constructor.

## tray.icon

Get or set the `icon` of the tray, `icon` must a path to your icon file. It can be a relative path which points to an icon in your app, or an absolute path pointing to a file in user's system.

!!! note "Mac"
    Mac OS X caveat: when used in notification context, png icon is not sized down like in windows notification area, it is rather displayed in 1:1 ratio.

## tray.alticon (Mac)

Get or set the alternate (active) tray icon.

## tray.iconsAreTemplates (Mac)

Get or set whether `icon` and `alticon` images are treated as "templates" (`true` by default). When the property is set to `true` the images are treated as "templates" and the system automatically ensures proper styling according to the various states of the status item (e.g. dark menu, light menu, etc.). Template images should consist only of black and clear colours and can use the alpha channel in the image to adjust the opacity of black content.

## tray.menu

Get or set the `menu` of the tray, `menu` will be showed when you click on the tray icon.

On Mac OS X the menu will be showed when you click on the tray (which is the only action available for tray icons on Mac OS X). On Windows and Linux, the menu will be showed when you single click on the tray with right mouse button, clicking with left mouse button sends the `click` event and does not show a menu.

In order to reduce differences from different platforms, setting `menu` property is the only way to bind a menu to tray, there's no way to popup a menu with left mouse button click on Linux and Windows.

## tray.remove()

Remove the tray.

Once removed, you will not be able to show it again and you should set your tray variable to `null` to make it garbage collected. There is no way temporarily hide a tray icon.

## Event: click

Emitted when user clicks the menu item with left mouse button.

You cannot catch click with right mouse button, because it is used to show menu. Even if you're not binding a menu to a tray, you still can not listen to it. Double click events are also ignored.

!!! note "Mac"
    NW.js does not support menulet (<kbd>&#8984;</kbd>+drag) since it will prohibit NW.js apps from being distributed in the App Store.
