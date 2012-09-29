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

#include "third_party/node/src/node.h"

#include <map>

namespace api {

// Key is int type and Value is weak pointer
class IDWeakMap : node::ObjectWrap {
 public:
  static void Init();
  static v8::Handle<v8::Function> GetContructor();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Set(const v8::Arguments& args);
  static v8::Handle<v8::Value> Get(const v8::Arguments& args);
  static v8::Handle<v8::Value> Has(const v8::Arguments& args);
  static v8::Handle<v8::Value> Delete(const v8::Arguments& args);

 private:
  explicit IDWeakMap();
  virtual ~IDWeakMap();

  void Erase(int key);

  static void WeakCallback(v8::Persistent<v8::Value> value, void *data);

  static v8::Persistent<v8::Function> constructor_;

  std::map< int, v8::Persistent<v8::Value> > map_;
};

}  // namespace api
