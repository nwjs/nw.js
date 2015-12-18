# Shell {: .doctitle}
---

[TOC]

`Shell` is a collection of APIs that do desktop related jobs.

## Synopsis

```javascript
// Open URL with default browser.
nw.Shell.openExternal('https://github.com/nwjs/nw.js');

// Open a text file with default text editor.
nw.Shell.openItem('test.txt');

// Show a file in parent folder with file manager.
nw.Shell.showItemInFolder('test.txt');
```

## Shell.openExternal(uri)

* `uri` `{String}` A URL to open in system default manner.

Open the given external URI in the desktop's default manner. For example, mailto: URLs in the default mail user agent.

## Shell.openItem(file_path)

* `file_path` `{String}` path to a local file

Open the given `file_path` in the desktop's default manner.

## Shell.showItemInFolder(file_path)

* `file_path` `{String}` path to a local file

Show the given `file_path` in the parent folder with file manager. If possible, select the file.