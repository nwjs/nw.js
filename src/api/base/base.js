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

var v8_util = process.binding('v8_util');

function Base() {
  throw new String("It's forbidden to instantialize a Base class.");
}
require('util').inherits(Base, require('events').EventEmitter);

// Silent unhandled events
Base.prototype.handleEvent = function() {
  this.emit.apply(this, arguments);
}

// Generic getter and setter
Base.prototype.handleGetter = function(name) {
  return v8_util.getHiddenValue(this, 'option')[name];
}

Base.prototype.handleSetter = function(name, setter, type, value) {
  value = type(value);
  v8_util.getHiddenValue(this, 'option')[name] = value;
  nw.callObjectMethod(this, setter, [ value ]);
}

exports.Base = Base;
