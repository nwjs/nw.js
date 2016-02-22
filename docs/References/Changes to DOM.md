# Changes to DOM {: .doctitle}
---

[TOC]

## &lt;input type="file"&gt;

HTML5 does provided limited support for file dialogs with `<input type="file" />` element, such as [`multiple`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#attr-multiple), [`accept`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#attr-accept) and `webkitdirectory`. NW.js extended the file input to better support native apps.

!!! note
    NW.js extended features are only enabled in Node frames for security reasons. See [Security in NW.js](../For Users/Advanced/Security in NW.js.md) for the differences of Node and normal frame.

### fileinput.value

The property contains native path of the local file.

For example, you can read the file selected by user with Node.js API:

```javascript
// Get the native path of the file selected by user
var fileinput = document.querySelector('input[type=file]');
var path = fileinput.value;

// Read file with Node.js API
var fs = nw.require('fs');
fs.readFile(path, 'utf8', function(err, txt) {
  if (err) {
    console.error(err);
    return;
  }

  console.log(txt);
});
```

### fileitem.path

HTML5 provides a `files` attribute to return all files selected in a `<input>` tag. NW.js provided an extra property `fileitem.path` to each file item in `files`, which is the native path of the selected file.

```javascript
var fileinput = document.querySelector('input[type=file]');
var files = fileinput.files;

for (var i = 0; i < files.length; ++i) {
  console.log(files[i].path);
}
```

### Attribute: `nwdirectory`

`nwdirectory` is a bit similar to `webkitdirectory`, but it returns the path of directory instead of returning files in it.

For example:

```html
<input type="file" nwdirectory>
```

### Attribute: `nwsaveas`

`nwsaveas` will open a 'Save as' dialog, which lets user enter the path of a file. It's possible to select a non-existing file, which is different from the default file input tag.

For example:

```html
<input type="file" nwsaveas>
```

And you can specify a value for the default file name to save:

```html
<input type="file" nwsaveas="filename.txt">
```

### Attribute: `nwworkingdir`

With `nwworkingdir`, the file dialog starts in the given directory when the element is activated.

For example, following code enables the file dialog opening in `/home/path/` by default:

```html
<input type="file" nwworkingdir="/home/path/">
```


## &lt;iframe&gt;

NW.js extended `<iframe>` tag for easier developing native apps. Thoses features enables apps to bypass the restrictions of Sandbox and Same Origin Policy etc.

See also [webview Tag](webview Tag.md) for the new `<webview>` tag.

### Attribute: nwdisable

Make the frame and subframes normal frames.

!!! note
    This attribute doesn't prevent the page in normal frames accessing its parent and top frames. Then they can still get access to Node.js APIs. So usually you may want to use this attribute with `nwfaketop` togather.

### Attribute: nwfaketop

Prevent web page in the frame accessing the real `window.parent` or `window.top`. They will get the faked parent and top, which is its own `window` object of the frame. Subframes will also be affected.

This attribute is usually used with `nwdisable`.

### Attribute: nwUserAgent

Override the default user-agent when loading the web page in the frame and sub frames. See [`user-agent` in manifest](Manifest Format.md#user-agent) for details.

## webview tag

added some methods to [webview tag]((webview Tag.md)