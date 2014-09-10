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

var nwDispatcher = nwDispatcher || {};

(function() {
  native function RequireNwGui();
  native function GetAbsolutePath();

  native function GetShellIdForCurrentContext();
  native function GetRoutingIDForCurrentContext();
  native function CreateShell();

  native function AllocateId();
  native function AllocateObject();
  native function DeallocateObject();
  native function CallObjectMethod();
  native function CallObjectMethodSync();
  native function CallStaticMethod();
  native function CallStaticMethodSync();
  native function CrashRenderer();
  native function SetCrashDumpDir();

  native function GetNSStringWithFixup();
  native function GetNSStringFWithFixup();

  nwDispatcher.requireNwGui = RequireNwGui;

  // Request a new object from browser
  nwDispatcher.allocateObject = function(object, option) {
    var v8_util = process.binding('v8_util');

    var id = AllocateId();
    AllocateObject(id, v8_util.getConstructorName(object), option);

    // Store object id and make it readonly
    Object.defineProperty(object, 'id', {
      value: id,
      writable: false
    });

    // Deallcoate on destroy
    v8_util.setDestructor(object, nwDispatcher.deallocateObject);

    // Store id to object relations, there is no delete in deallocateObject
    // since this is a weak map.
    global.__nwObjectsRegistry.set(id, object);
  }

  // Free a object in browser
  nwDispatcher.deallocateObject = function(object) {
    DeallocateObject(object.id);
  };

  // Call method of a object in browser.
  nwDispatcher.callObjectMethod = function(object, method, args) {
    CallObjectMethod(object.id,
                     process.binding('v8_util').getConstructorName(object),
                     method,
                     args);
  };

  // Call sync method of a object in browser and return results.
  nwDispatcher.callObjectMethodSync = function(object, method, args) {
    return CallObjectMethodSync(
        object.id,
        process.binding('v8_util').getConstructorName(object),
        method,
        args);
  };

  // Call a static method.
  nwDispatcher.callStaticMethod = CallStaticMethod;

  // Call a sync method of static class in browse and return.
  nwDispatcher.callStaticMethodSync = CallStaticMethodSync;

  nwDispatcher.getAbsolutePath = GetAbsolutePath;
  nwDispatcher.getShellIdForCurrentContext = GetShellIdForCurrentContext;
  nwDispatcher.getRoutingIDForCurrentContext = GetRoutingIDForCurrentContext;
  nwDispatcher.createShell = CreateShell;

  nwDispatcher.crashRenderer = CrashRenderer;
  nwDispatcher.setCrashDumpDir = SetCrashDumpDir;
  nwDispatcher.allocateId = AllocateId;

  nwDispatcher.getNSStringWithFixup = GetNSStringWithFixup;
  nwDispatcher.getNSStringFWithFixup = GetNSStringFWithFixup;

})();
