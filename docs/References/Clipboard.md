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

Write `data` of `type` to the clipboard.

!!! tip "Format of Image"
    Images read or written from clipboard can be either JPEG or PNG. When `raw` is not set or set to `false`, the data is expected to be a valid [data URI](https://developer.mozilla.org/en-US/docs/Web/HTTP/data_URIs) encoded with Base64. When `raw` is set to `true`, the data is only the Base64 encoded image data not including the `data:<mime-type>;base64,` part.

## clip.get([type])

* `type` `{String}` _Optional_ the type of the data. Support `text`, `png`, `jpeg`, `html` and `rtf`. By default, `type` is set to `"text"`.
* `raw`  `{Boolean}` _Optional_ requiring raw image data. This option is only valid if type is `png` or `jpeg`.
* Returns `{String}` the data retrieved from the clipboard

Get the data of `type` from clipboard.

## clip.readAvailableTypes()

* Returns `{Array}` list of available types of data in clipboard currenly. Each item is one of following types:
    - `text`: plain text. Can be read by `clip.get('text')`.
    - `html`: HTML text. Can be read by `clip.get('html')`.
    - `rtf`: RTF (Rich Text Format). Can be read by `clip.get('rtf')`.
    - `png`: PNG image. Can be read by `clip.get('png')`.
    - `jpeg`: JPEG image. Can be read by `clip.get('jpeg')`.

You can use the returned list as a suggestion to get the right data from clipboard.

## clip.clear()

Clear the clipboard.
