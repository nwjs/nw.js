#include <node.h>
#include <v8.h>

using namespace v8;

static uv_timer_t timer;

void callback(uv_timer_t* handle, int status) {
  v8::String::New("Force create handle without handle scope");
}

void init(Handle<Object> target) {
  uv_timer_init(uv_default_loop(), &timer);
  uv_timer_start(&timer, callback, 0, 0);
}

NODE_MODULE(test, init)
