# File Dialogs {: .doctitle}
---

!!! important "Available"
    Since 0.2.5

[TOC]

HTML5 does provided limited support for file dialogs with `<input type="file" />` element. NW.js extended the file input to better support native apps.

!!! note
    NW.js extended features are only enabled in Node frames for security reasons. See [Frames](Frames.md) for the differences of Node and normal frame.

## Features Inherited from Chromium

### Attribute: `multiple`

This Boolean attribute indicates whether the user can select more than one files.

```html
<input type="file" multiple>
```

And the `fileinput.value` will return all selected files's paths separated by `;`. See [fileinput.value](#fileinputvalue) for NW.js extensions.

### Attribute: `accept`

This attribute indicates the types of files that the server accepts; otherwise it is ignored. The value must be a comma-separated list of unique content type specifiers:

* A file extension starting with the STOP character (U+002E). (E.g.: ".jpg,.png,.doc")
* A valid MIME type with no extensions
* audio/* representing sound files
* video/* representing video files
* image/* representing image files

For example, with following code you can filter to show only MS Word files:

```html
<input type="file" accept=".doc,.docx,application/msword">
```

### Attribute: `webkitdirectory`

This attribute enables users to select a directory in "Select folder" dialog.

```html
<input type="file" webkitdirectory />
```

The `fileinput.value` is not the path of directory we selected, but paths of all files under the directory. If you want the path of the selected directory, you can use [`nwdirectory` Attribute](#attribute-nwdirectory).

## Featured Extended by NW.js

### fileinput.value

!!! important "Available"
    Since 0.2.5

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

!!! important "Available"
    Since 0.2.5

HTML5 provides a `files` attribute to return all files selected in a `<input>` tag. NW.js provided an extra property `fileitem.path` to each file item in `files`, which is the native path of the selected file.

```javascript
var fileinput = document.querySelector('input[type=file]');
var files = fileinput.files;

for (var i = 0; i < files.length; ++i) {
  console.log(files[i].path);
}
```

### Attribute: `nwdirectory`

!!! important "Available"
    Since 0.2.5

`nwdirectory` is a bit similar to `webkitdirectory`, but it returns the path of directory.

For example:

```html
<input type="file" nwdirectory>
```

### Attribute: `nwsaveas`

!!! important "Available"
    Since 0.2.5

`nwsaveas` will open a 'Save as' dialog, which lets user enter the path of a file. It's possible to select a non-existing file, which is different from the default file input tag.

For example:

```html
<input type="file" nwsaveas>
```

Starting from v0.7.3 you can specify a value for the default file name to save:

```html
<input type="file" nwsaveas="filename.txt">
```

### Attribute: `nwworkingdir`

!!! important "Available"
    Since 0.5.0

With `nwworkingdir`, the file dialog starts in the given directory when the element is activated.

For example, following code enables the file dialog opening in `/home/path/` by default:

```html
<input type="file" nwworkingdir="/home/path/">
```
