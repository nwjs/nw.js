#ifndef _NWEBKIT_CONTEXT_WRAP
#define _NWEBKIT_CONTEXT_WRAP

#include <node.h>

namespace nwebkit {
class ContextWrap : node::ObjectWrap {
 public:
  static void Initialize(v8::Handle<v8::Object> target);
  static v8::Handle<v8::Value> New(const v8::Arguments& args);

  v8::Persistent<v8::Context> GetV8Context();
  static v8::Local<v8::Object> NewInstance();
  static bool InstanceOf(v8::Handle<v8::Value> value);
  static v8::Handle<v8::Value> OnContextCreationFromWebKit(const v8::Arguments& args);
 protected:

  static v8::Persistent<v8::FunctionTemplate> constructor_template;

  ContextWrap();
  ~ContextWrap();

  v8::Persistent<v8::Context> context_;
  bool context_valid;
};
}
#endif
