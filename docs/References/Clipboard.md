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

## clip.set(data, [type])

* `data` `{String}` the data to write to the clipboard
* `type` `{String}` _Optional_ the type of the data. Currently only `"text"` (plain text) is support. By default, `type` is set to `"text"`.

Write `data` of `type` to the clipboard.

## clip.get([type])

* `type` `{String}` _Optional_ the type of the data. Currently only `"text"` (plain text) is support. By default, `type` is set to `"text"`.
* Returns `{String}` the data retrieved from the clipboard

Get the data of `type` from clipboard.

## clip.clear()

Clear the clipboard.
