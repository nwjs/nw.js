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

// OS X and Linux only, Windows does not have a concept of workspaces
var canSetVisibleOnAllWorkspaces = /(darwin|linux)/.test(require('os').platform());

exports.Window = {
  get: function(other) {
    // Return other window.
    if (typeof other != 'undefined' && other.hasOwnProperty('nwDispatcher'))
      return other.nwDispatcher.requireNwGui().Window.get();

    var id;
    // See if this window context has requested Shell's id before.
    if (typeof window.__nwWindowId == 'number')
      id = window.__nwWindowId;
    else // else see if the Shell has a corresponding js object.
      id = nw.getShellIdForCurrentContext();

    // Return API's window object from id.
    var ret;
    if (id > 0) {
      window.__nwWindowId = id;
      ret = global.__nwWindowsStore[window.__nwWindowId];
      if (!ret) {
        ret = new global.Window(nw.getRoutingIDForCurrentContext(), true, id);
      }
      return ret;
    }

    return new global.Window(nw.getRoutingIDForCurrentContext());
  },
  open: function(url, options) {
    // Conver relative url to full url.
    var protocol = url.match(/^[a-z]+:\/\//i);
    if (protocol == null || protocol.length == 0) {
      var href = window.location.href.split(/\?|#/)[0];
      url = href.substring(0, href.lastIndexOf('/') + 1) + url;
    }

    // Force no empty options.
    if (typeof options != 'object')
      options = {};

    if (!('focus' in options)) {
        options.focus = false;
    }
    // Create new shell and get it's routing id.
    var id = nw.allocateId();
    options.object_id = id;
    options.nw_win_id = id;
    var routing_id = nw.createShell(url, options);

    return new global.Window(routing_id, true, id);
  },
  canSetVisibleOnAllWorkspaces: function() {
    return canSetVisibleOnAllWorkspaces;
  }
};
