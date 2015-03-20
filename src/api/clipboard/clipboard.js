// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

var clipboardInstance = null;

function Clipboard() {
  nw.allocateObject(this, {});
}
require('util').inherits(Clipboard, exports.Base);

Clipboard.prototype.set = function(data, type) {
  if (typeof type == 'undefined')
    type = 'text';

  if (type != 'text')
    throw new TypeError("Type of '" + type + "' is not supported");

  nw.callObjectMethod(this, 'Set', [ data, type ]);
}

Clipboard.prototype.get = function(type) {
  if (typeof type == 'undefined')
    type = 'text';

  if (type != 'text')
    throw new TypeError('Only support getting plain text from Clipboard');

  var result = nw.callObjectMethodSync(this, 'Get', [ type ]);
  if (type == 'text')
    return String(result);
}

Clipboard.prototype.clear = function() {
  nw.callObjectMethod(this, 'Clear', [ ]);
}

exports.Clipboard = {
  get: function() {
    if (clipboardInstance == null) {
      clipboardInstance = new Clipboard();
    }

    return clipboardInstance;
  }
};
