#include "v8/src/v8.h"

using namespace v8;


void FixSourceNWBin(Isolate* v8_isolate, Handle<UnboundScript> script) {
  i::Isolate* isolate = reinterpret_cast<i::Isolate*>(v8_isolate);
  i::Handle<i::HeapObject> obj =
    i::Handle<i::HeapObject>::cast(v8::Utils::OpenHandle(*script));
  i::Handle<i::SharedFunctionInfo>
      function_info(i::SharedFunctionInfo::cast(*obj), obj->GetIsolate());
  reinterpret_cast<i::Script*>(function_info->script())->set_source(isolate->heap()->undefined_value());
}
