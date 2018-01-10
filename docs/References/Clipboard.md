# Clipboard {: .doctitle}

---

[TOC]

`Clipboard` is an abstraction of clipboard for Windows, Linux and Mac.

## Synopsis

```javascript
// get the system clipboard
var clipboard = nw.Clipboard.get();

// Read from clipboard
var text = clipboard.get('text');
console.log(text);

// Or write something
clipboard.set('I love NW.js :)', 'text');

// And clear it!
clipboard.clear();
```

## Clipboard.get()

* Returns `{Clipboard}` the clipboard object

!!! note
    The Selection Clipboard in X11 is not supported.

## clip.set(data, [type, [raw]])

* `data` `{String}` the data to write to the clipboard
* `type` `{String}` _Optional_ the type of the data. Support `text`, `png`, `jpeg`, `html` and `rtf`. By default, `type` is set to `"text"`.
* `raw`  `{Boolean}` _Optional_ requiring raw image data. This option is only valid if type is `png` or `jpeg`. By default, `raw` is set to `false`.

Write `data` of `type` to the clipboard. This method will clear the clipboard and replace with the given `data`. Hence another call to this method will overwrite with the new one. To write multiple types of data to clipboard simultaneously, you will need to use [clip.set(clipboardDataList)](#clipsetclipboarddatalist) below.

!!! tip "Format of Image"
    Images read or written from clipboard can be either JPEG or PNG. When `raw` is not set or set to `false`, the data is expected to be a valid [data URI](https://developer.mozilla.org/en-US/docs/Web/HTTP/data_URIs) encoded with Base64. When `raw` is set to `true`, the data is only the Base64 encoded image data not including the `data:<mime-type>;base64,` part.

## clip.set(clipboardData)

* `clipboardData` `{Object}` JSON object containing `data`, `type` and `raw` to be written to clipboard. See [clip.set(data, [type, [raw]])](#clipsetdata-type-raw) for detailed arguments specification.

## clip.set(clipboardDataList)

* `clipboardDataList` `{Array}` Array of `clipboardData` to be written to clipboard. See [clip.set(clipboardData)](#clipgetclipboarddata) and [clip.set(data, [type, [raw]])](#clipsetdata-type-raw) for detailed arguments specification.

Multiple types of data can be written to clipboard simultaneously with this method.

Example: write an image and a `<img>` pointing to the image to the clipboard

```javascript
var fs = require('fs');
var path = require('path');

// resolve path as absolute path in order to be used by other applications
var pngPath = path.resolve('nw.png');
// read the image from file system as base64 encoded string
var data = fs.readFileSync(pngPath).toString('base64');
// transform file path to URL
var html = '<img src="file:///' + encodeURI(data.replace(/^\//, '')) + '">';

var clip = nw.Clipboard.get();
// write both PNG and HTML to clipboard
clip.set([
  {type: 'png', data: data, raw: true},
  {type: 'html', data: html}
]);
```

## clip.get([type, [raw]])

* `type` `{String}` _Optional_ the type of the data. Support `text`, `png`, `jpeg`, `html` and `rtf`. By default, `type` is set to `"text"`.
* `raw`  `{Boolean}` _Optional_ requiring raw image data. This option is only valid if type is `png` or `jpeg`.
* Returns `{String}` the data retrieved from the clipboard

Get the data of `type` from clipboard.

## clip.get(clipboardData)

* `clipboardData` `{Object}` JSON object containing `type` and `raw` argument for reading data from clipboard. See [clip.get([type, [raw]])](#clipgettype-raw) for detailed arguments specification.
* Returns `{String}` the data retrieved from the clipboard

## clip.get(clipboardDataList)

* `clipboardDataList` `{Array}` Array of `clipboardData` for reading data from clipboard. Multiple types of data can be read from clipboard simultaneously with this method. See [clip.get(clipboardData)](#clipgetclipboarddata) and [clip.get([type, [raw]])](#clipgettype-raw) for detailed arguments specification.
* Returns `{Array}` array of `clipboardData` retrieved from the clipboard. Each item contains `type`, `data` and `raw` (optional) attributes.

## clip.readAvailableTypes()

* Returns `{Array}` list of available types of data in clipboard currently. Each item is one of following types:
    - `text`: plain text. Can be read by `clip.get('text')`.
    - `html`: HTML text. Can be read by `clip.get('html')`.
    - `rtf`: RTF (Rich Text Format). Can be read by `clip.get('rtf')`.
    - `png`: PNG image. Can be read by `clip.get('png')`.
    - `jpeg`: JPEG image. Can be read by `clip.get('jpeg')`.

You can use the returned list as a suggestion to get the right data from clipboard.

## clip.clear()

Clear the clipboard.
