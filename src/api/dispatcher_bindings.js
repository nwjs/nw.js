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
  native function GetIDWeakMapConstructor();
  native function GetNextObjectId();
  native function GetAbsolutePath();

  native function AllocateObject();
  native function DeallocateObject();
  native function CallObjectMethod();
  native function CallObjectMethodSync();
  native function CallStaticMethod();

  native function GetConstructorName();
  native function GetHiddenValue();
  native function SetHiddenValue();
  native function SetDestructor();

  var IDWeakMap = GetIDWeakMapConstructor();

  // Store id2object map
  var objectsRegistry = new IDWeakMap();

  // Request a new object from browser
  nwDispatcher.allocateObject = function(object, option) {
    var id = GetNextObjectId();
    AllocateObject(id, object.getConstructorName(), option);

    // Store object id and make it readonly
    Object.defineProperty(object, 'id', {
      value: id,
      writable: false
    });

    // Deallcoate on destroy
    object.setDestructor(nwDispatcher.deallocateObject);

    // Store id to object relations, there is no delete in deallocateObject
    // since this is a weak map.
    objectsRegistry.set(id, object);
  }

  // Free a object in browser
  nwDispatcher.deallocateObject = function(object) {
    DeallocateObject(object.id);
  };

  // Call method of a object in browser.
  nwDispatcher.callObjectMethod = function(object, method, args) {
    CallObjectMethod(object.id, object.getConstructorName(), method, args);
  };

  // Call sync method of a object in browser and return results.
  nwDispatcher.callObjectMethodSync = function(object, method, args) {
    return CallObjectMethodSync(
        object.id, object.getConstructorName(), method, args);
  };

  // Call a static method
  nwDispatcher.callStaticMethod = CallStaticMethod;

  nwDispatcher.handleEvent = function(object_id, ev, args) {
    var object = objectsRegistry.get(object_id);
    args.splice(0, 0, ev);
    object.handleEvent.apply(object, args);
  }

  nwDispatcher.requireNwGui = RequireNwGui;

  nwDispatcher.getAbsolutePath = GetAbsolutePath;

  // Extended prototype of objects.
  nwDispatcher.getConstructorName = GetConstructorName;
  nwDispatcher.getHiddenValue = GetHiddenValue;
  nwDispatcher.setHiddenValue = SetHiddenValue;
  nwDispatcher.setDestructor = SetDestructor;
  nwDispatcher.inherits = function(ctor, superCtor) {
    ctor.prototype = Object.create(superCtor.prototype, {
      constructor: {
        value: ctor,
        enumerable: false,
        writable: true,
        configurable: true
      }
    });
  };
})();
